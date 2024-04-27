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
#include "muduoPublic.h"
#include "model/userModel.h"
#include "baseVersion.h"
#include "model/messageModel.h"
#include "msgType.h"
#include "networkService.h"
#include <muduo/base/Timestamp.h>
#include <mutex>

using businessEventCallback = std::function<void(const networkService& network, const nlohmann::json& js, Timestamp time)>;

class chatBusiness
{
public:
    // void entrance(const TcpConnectionPtr &ptr, const nlohmann::json& js) override;
    // void CallbackMessage(std::function<void(const TcpConnectionPtr &, Buffer *, Timestamp)>);

    // virtual void onMessageCallback(const TcpConnectionPtr &ptr, const nlohmann::json &js, Timestamp) override;
    // //virtual void onThreadInitCallback(EventLoop *) override;
    void onConnectCallback(const TcpConnectionPtr &ptr);
    void onWriteCompleteCallback(const TcpConnectionPtr &ptr);

//private:
    //void encapsulation(const nlohmann::json &js)
    chatBusiness();

    //分发业务
    void distribute(const TcpConnectionPtr &ptr, const std::string &mes, Timestamp time);

    

private:
    // bool relayMessage(const networkService &network, const nlohmann::json &js, Timestamp time);
    // 登录业务
    void login(const networkService &network, const nlohmann::json &js, Timestamp time);
    // 用户下线
    void logout(const networkService &network, const chatdb::user &js, Timestamp time);

    // 消息业务
    void signalMessage(const networkService &network, const nlohmann::json &js, Timestamp time);

    // 注册业务
    void registerUser(const networkService &network, const nlohmann::json &js, Timestamp time);

    // 记录所有用户连接的map
    std::unordered_map<int, TcpConnectionPtr> userConnectMap;
    //事件注册器
    std::unordered_map<EnMsgType, businessEventCallback> userEventCallbackMap;
    // 对于map操作保证其线程安全性
    std::mutex m_mutex;

    userModel userSql;
    messageModel offlineSql; 
};