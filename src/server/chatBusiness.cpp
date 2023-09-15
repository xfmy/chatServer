#include <nlohmann/json.hpp>
#include <fmt/core.h>
#include "chatBusiness.h"
#include "errorEvent.h"
#include "msgType.h"

void chatBusiness::onConnectCallback(const TcpConnectionPtr &ptr)
{
    if(ptr->connected()){//客户端已经连接
        //TODO: 编写客户端连接代码
    }else if(ptr->disconnected()){//客户已经断开
        logout(ptr, boost::any_cast<chatdb::user>(ptr->getContext()) , Timestamp::now());
    }
}

void chatBusiness::onWriteCompleteCallback(const TcpConnectionPtr &ptr)
{
    //TODO:编写客户端数据发送完毕代码
}

chatBusiness::chatBusiness()
{
    userEventCallbackMap.emplace(EnMsgType::LOGIN_MSG,std::bind(&chatBusiness::login,this,_1,_2,_3));
    userEventCallbackMap.emplace(EnMsgType::REG_MSG, std::bind(&chatBusiness::registerUser, this, _1, _2, _3));
    userEventCallbackMap.emplace(EnMsgType::ONE_CHAT_MSG, std::bind(&chatBusiness::signalMessage, this, _1, _2, _3));
}

void chatBusiness::distribute(const TcpConnectionPtr &ptr, const std::string &mes, Timestamp time)
{
    try
    {
        nlohmann::json js = nlohmann::json::parse(mes);
        EnMsgType msgType = static_cast<EnMsgType>(js["msgType"].get<int>());

        const auto &it = userEventCallbackMap.find(msgType);
        if (it != userEventCallbackMap.end())
        {
            it->second(ptr, js, time);
        }
        else
        {
            errorEvent::protocolParseError(ptr);
        }
    }
    catch (nlohmann::json::exception &e)
    {
        LOG_ERROR << fmt::format("message:{} \\  exception id: {}", e.what(), e.id);
        errorEvent::protocolParseError(ptr);
    }
}

void chatBusiness::login(const networkService &network, const nlohmann::json &js, Timestamp)
{
    try
    {

        chatdb::user user = userSql.queryUser(js["name"].get<std::string>(), js["password"].get<std::string>());
        nlohmann::json responseJs;
        if (user.getId() == -1)
        {
            responseJs["code"] = 404;
            responseJs["msg"] = "账号或者密码错误,请重新输入";
        }
        else
        {
            responseJs["code"] = 200;
            responseJs["msg"] = "登陆成功";
            responseJs["user"]["id"] = user.getId();
            responseJs["user"]["name"] = user.getName();
            
            //将有户信息保存到上下文中
            network.ptr->setContext(user);
            //用户历史消息
            responseJs["offlineMessage"] = offlineSql.pullOfflineMessages(user.getId());

            // 将连接添加到在线用户中
            std::lock_guard<std::mutex> lock(m_mutex);
            userConnectMap.emplace(user.getId(), network.ptr);
            LOG_INFO << network.ptr->peerAddress().toIpPort() + user.getName() + ":登录成功";
        }
        responseJs["msgType"] = LOGIN_MSG_ACK;
        network.response(responseJs);
    }
    catch (nlohmann::json::exception &e)
    {
        LOG_ERROR << fmt::format("message:{} \\  exception id: {}", e.what(), e.id);
        errorEvent::protocolParseError(network);
    }
}

void chatBusiness::logout(const networkService &network, const chatdb::user &user, Timestamp time)
{
    userConnectMap.erase(user.getId());
    LOG_INFO << network.ptr->peerAddress().toIpPort() + user.getName() + ":退出登录";
}

void chatBusiness::signalMessage(const networkService &network, const nlohmann::json &js, Timestamp)
{
    // 1.用户发送消息,
    // 2.转发消息
    // 1.转发单聊消息,转发群聊消息
    // 调用转发消息函数,将转发结果返回给客户端
    // 如果对方在线,则直接发送,不在则存储服务器
    nlohmann::json retJs;
    try
    {
        std::string name = js["to"];
        int id = userSql.queryId(name);
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            auto it = userConnectMap.find(id);
            if (it != userConnectMap.end())
            { // 在线
                networkService(it->second).response(js);
            }
            else
            { // 不在线
                offlineSql.storingMessage(id, js.dump());
            }
        }
        retJs["code"] = 200;
        retJs["msg"] = "消息发送成功";
        retJs["msgType"] = ONE_CHAT_MSG_ACK;
        LOG_INFO << network.ptr->peerAddress().toIpPort() + js.dump();
    }
    catch (nlohmann::json::exception &e)
    {
        LOG_ERROR << fmt::format("message:{} \\  exception id: {}", e.what(), e.id);
        errorEvent::protocolParseError(network);
    }
    network.response(retJs);
}

void chatBusiness::registerUser(const networkService &network, const nlohmann::json &js, Timestamp)
{
    try
    {
        std::string name = js["name"];
        std::string password = js["password"];
        chatdb::user user;
        user.setName(name);
        user.setPassword(password);
        user.setState("offline");

        int res = userSql.queryId(name);
        nlohmann::json retJs;
        if (res != -1)
        {
            retJs["code"] = 404;
            retJs["msg"] = "该账户已经注册,请重新输入";
        }
        else
        {
            userSql.insertUser(user);
            user = userSql.queryUser(name, password);

            retJs["code"] = 200;
            retJs["msg"] = "注册成功";
            retJs["id"] = user.getId();
        }
        retJs["msgType"] = REG_MSG_ACK;
        network.response(retJs);
        LOG_INFO << network.ptr->peerAddress().toIpPort() + "新用户注册:name=" + name;
    }
    catch (nlohmann::json::exception &e)
    {
        LOG_ERROR << fmt::format("message:{} \\  exception id: {}", e.what(), e.id);
        errorEvent::protocolParseError(network);
    }
}

// bool chatBusiness::relayMessage(const networkService &network, const nlohmann::json &js, Timestamp time)
// {
//     // 如果对方在线,则直接发送,不在则存储服务器
//     bool ret = true;
//     // auto statement = mysqlConnPool::getObject()->getStatement();
//     try
//     {
//         std::string name = js["to"];
//         int id = userSql.queryId(name);
//         {
//             std::lock_guard<std::mutex> lock(m_mutex);
//             auto it = userConnectMap.find(id);
//             if (it != userConnectMap.end())
//             { // 在线
//                 networkService(it->second).response(js);
//             }
//             else
//             { // 不在线
//                 offlineSql.storingMessage(id, js.dump());
//             }
//         }
//     }
//     catch (nlohmann::json::exception &e)
//     {
//         LOG_ERROR << "message: " << e.what() << '\n' << "exception id: " << e.id;
//         errorEvent::protocolParseError(network);
//         ret = false;
//     }
//     return ret;
// }
