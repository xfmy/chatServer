#pragma once
#include <functional>
#include <muduo/base/Timestamp.h>
#include <mutex>

#include "redis_route.h"
#include "msg_type.h"
#include "network_service.hpp"

#include "model/user_model.h"
#include "model/offline_message_model.h"
#include "model/group_user_model.h"
#include "model/friend_model.h"

using namespace muduo;
using namespace muduo::net;

using SessionEventCallback = std::function<void(
    const NetworkService &network, const nlohmann::json &js, Timestamp time)>;

class ChatSession
{
public:
    // 网络连接回调
    void onConnectCallback(const TcpConnectionPtr &ptr);

    ChatSession();
    ~ChatSession();

    // 分发业务
    void distribute(const TcpConnectionPtr &ptr, const std::string &mes,
                    Timestamp time);
    // 从redis消息队列中获取订阅的消息
    void handleRedisSubscribeMessage(int userid, string msg);

private:
    // 登录业务
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
    void clientCloseException(const NetworkService &conn);

private:
    // 记录所有用户连接的map
    std::unordered_map<int, NetworkService> userConnectMap_;
    //事件注册器
    std::unordered_map<EnMsgType, SessionEventCallback> userEventCallbackMap_;
    // 对于map操作保证其线程安全性
    std::mutex mtx_;

    // 数据库ORM操作类
    UserModel userModel_;
    MessageModel offlineMessageModel_;
    GroupModel groupModel_;
    FriendModel friendModel_;

    // redis消息路由类
    RedisRoute redisRoute_;
};