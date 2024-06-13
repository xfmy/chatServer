#pragma once
//业务层
/*
思路1:  写一个基类,然后写派生类,不同的派生类代表不同的协议 version1.0/1.1
        以后则根据不同的version交由不同的派生类处理
        *handleRead:入口->细分派发任务

        全部static?  / or   map分发管理器?
{
    version:1.0
    {
        type:注册业务
        name:admin
        password:admin
    }
}


std::map<version,eventHandle>  void(baseVersion* , )

编写返回数据的封装
*/
#include <functional>
#include "model/user_model.h"
#include "model/offline_message_model.h"
#include "msgType.h"
#include "network_service.hpp"
#include "model/group_user_model.h"
#include "model/friend_model.h"
#include <muduo/base/Timestamp.h>
#include <mutex>
#include "redis_route.h"

using SessionEventCallback = std::function<void(
    const NetworkService &network, const nlohmann::json &js, Timestamp time)>;

class ChatSession
{
public:
    ChatSession();
    ~ChatSession();

    //分发业务
    void distribute(const TcpConnectionPtr &ptr, const std::string &mes,
                    Timestamp time);
    // 从redis消息队列中获取订阅的消息
    void handleRedisSubscribeMessage(int userid, string msg);

private:
    // bool relayMessage(const networkService &network, const nlohmann::json
    // &js, Timestamp time); 登录业务
    void login(const NetworkService &network, const nlohmann::json &js,
               Timestamp time);
    // 用户下线
    void logout(const NetworkService &network, const nlohmann::json &js,
                Timestamp time);

    // 消息业务
    void oneChat(const NetworkService &network, const nlohmann::json &js,
                 Timestamp time);

    // 注册业务
    void registerUser(const NetworkService &network, const nlohmann::json &js,
                      Timestamp time);

    //添加群组
    void addGroup(const NetworkService &conn, const nlohmann::json &js,
                  Timestamp time);

    //添加好友
    void addFriend(const NetworkService &conn, const nlohmann::json &js,
                   Timestamp time);
    // 群组聊天业务
    void groupChat(const NetworkService &conn, const nlohmann::json &js,
                   Timestamp time);

    // 处理客户端异常退出
    void clientCloseException(const TcpConnectionPtr &conn);

private:
    // 记录所有用户连接的map
    std::unordered_map<int, NetworkService> userConnectMap;
    //事件注册器
    std::unordered_map<EnMsgType, SessionEventCallback> userEventCallbackMap;
    // 对于map操作保证其线程安全性
    std::mutex m_mutex;

    UserModel userSql;
    MessageModel offlineSql;
    GroupModel groupModel;
    FriendModel friendModel;

    RedisRoute redisRoute;
};