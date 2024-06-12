#include <nlohmann/json.hpp>
#include <fmt/core.h>
#include <vector>
#include <muduo/base/Logging.h>
#include "chat_session.h"
#include "errorEvent.h"
#include "msgType.h"

void ChatSession::onConnectCallback(const TcpConnectionPtr &ptr)
{
    if (ptr->connected())
    { //客户端已经连接
        // TODO: 编写客户端连接代码
    }
    else if (ptr->disconnected())
    { //客户已经断开
        // logout(ptr, boost::any_cast<User>(ptr->getContext()),
        //        Timestamp::now());
    }
}

void ChatSession::onWriteCompleteCallback(const TcpConnectionPtr &ptr)
{
    // TODO:编写客户端数据发送完毕代码
}

ChatSession::ChatSession()
{
    userEventCallbackMap.emplace(
        std::make_pair<EnMsgType, SessionEventCallback>(EnMsgType::LOGIN_MSG,
        std::bind(&ChatSession::login, this, _1, _2, _3)));
    userEventCallbackMap.emplace(
        std::make_pair<EnMsgType, SessionEventCallback>(
            EnMsgType::REG_MSG,
            std::bind(&ChatSession::registerUser, this, _1, _2, _3)));
    userEventCallbackMap.emplace(
        std::make_pair<EnMsgType, SessionEventCallback>(
            EnMsgType::ONE_CHAT_MSG,
            std::bind(&ChatSession::oneChat, this, _1, _2, _3)));
    userEventCallbackMap.emplace(
        std::make_pair<EnMsgType, SessionEventCallback>(
            EnMsgType::LOGINOUT_MSG,
            std::bind(&ChatSession::logout, this, _1, _2, _3)));
    userEventCallbackMap.emplace(
        std::make_pair<EnMsgType, SessionEventCallback>(
            EnMsgType::ADD_GROUP_MSG,
            std::bind(&ChatSession::addGroup, this, _1, _2, _3)));
    userEventCallbackMap.emplace(
        std::make_pair<EnMsgType, SessionEventCallback>(
            EnMsgType::ADD_FRIEND_MSG,
            std::bind(&ChatSession::addFriend, this, _1, _2, _3)));
    userEventCallbackMap.emplace(
        std::make_pair<EnMsgType, SessionEventCallback>(
            EnMsgType::GROUP_CHAT_MSG,
            std::bind(&ChatSession::groupChat, this, _1, _2, _3)));
}

void ChatSession::distribute(const TcpConnectionPtr &ptr,
                              const std::string &mes, Timestamp time)
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
        LOG_ERROR << fmt::format("message:{} \\  exception id: {}", e.what(),
                                 e.id);
        errorEvent::protocolParseError(ptr);
    }
}

//用户登录
void ChatSession::login(const NetworkService &conn, const nlohmann::json &js,
                        Timestamp)
{
    try
    {
        //User user = userSql.queryUser(
        //    js["name"].get<std::string>(), js["password"].get<std::string>());
        User user = userSql.query(js["id"].get<int>());

        nlohmann::json responseJs;
        if (user.GetId() == -1 || js["password"].get<std::string>() != user.GetPassword())
        {
            responseJs["code"] = 404;
            responseJs["msg"] = "账号或者密码错误,请重新输入";
        }
        else
        {
            responseJs["code"] = 200;
            responseJs["msg"] = "登陆成功";
            responseJs["user"]["id"] = user.GetId();
            responseJs["user"]["name"] = user.GetName();

            //将有户信息保存到上下文中
            const_cast<NetworkService*>(&conn)->setContext(user);
            //用户历史消息
            responseJs["offlineMessage"] =
                offlineSql.query(user.GetId());

            // 将连接添加到在线用户中
            std::lock_guard<std::mutex> lock(m_mutex);
            userConnectMap.emplace(user.GetId(), conn);
            LOG_INFO << user.GetName() +
                            ":登录成功";
            user.SetState("online");
            userSql.UpdateState(user);
        }
        responseJs["msgType"] = EnMsgType::LOGIN_MSG_ACK;
        conn.send(responseJs);
    }
    catch (nlohmann::json::exception &e)
    {
        LOG_ERROR << fmt::format("message:{} \\  exception id: {}", e.what(),
                                 e.id);
        errorEvent::protocolParseError(conn);
    }
}

//退出登录
void ChatSession::logout(const NetworkService &conn, const nlohmann::json &js,
                         Timestamp time)
{
    userConnectMap.erase(js["id"].get<int>());
    User user;
    user.SetId(js["id"].get<int>());
    user.SetState("offline");
    userSql.UpdateState(user);
    LOG_INFO << user.GetName() +
                    ":退出登录";
}

void ChatSession::oneChat(const NetworkService &conn, const nlohmann::json &js,
                          Timestamp)
{
    // 1.用户发送消息,
    // 2.转发消息
    // 1.转发单聊消息,转发群聊消息
    // 调用转发消息函数,将转发结果返回给客户端
    // 如果对方在线,则直接发送,不在则存储服务器
    nlohmann::json retJs;
    try
    {
        int id = js["toid"];
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            auto it = userConnectMap.find(id);
            if (it != userConnectMap.end())
            { // 在线
                NetworkService(it->second).send(js);
            }
            else
            { // 不在线
                offlineSql.insert(id, js.dump());
            }
        }
        retJs["code"] = 200;
        retJs["msg"] = "消息发送成功";
        retJs["msgType"] = EnMsgType::ONE_CHAT_MSG_ACK;
        LOG_INFO << js.dump();
    }
    catch (nlohmann::json::exception &e)
    {
        LOG_ERROR << fmt::format("message:{} \\  exception id: {}", e.what(),
                                 e.id);
        errorEvent::protocolParseError(conn);
    }
    conn.send(retJs);
}

//注册用户
void ChatSession::registerUser(const NetworkService &conn,
                                const nlohmann::json &js, Timestamp)
{
    try
    {
        std::string name = js["name"];
        std::string password = js["password"];

        User user;
        user.SetName(name);
        user.SetPassword(password);
        user.SetState("offline");

        int res = userSql.insert(user);
        nlohmann::json retJs;
        if (!res)
        {
            retJs["code"] = 404;
            retJs["msg"] = "该账户已经注册,请重新输入";
        }
        else
        {
            // userSql.insertUser(user);
            // user = userSql.queryUser(name, password);
            retJs["code"] = 200;
            retJs["msg"] = "注册成功";
            retJs["id"] = user.GetId();
        }
        retJs["msgType"] = EnMsgType::REG_MSG_ACK;
        conn.send(retJs);
        LOG_INFO << "新用户注册:name=" + name;
    }
    catch (nlohmann::json::exception &e)
    {
        LOG_ERROR << fmt::format("message:{} \\  exception id: {}", e.what(),
                                 e.id);
        errorEvent::protocolParseError(conn);
    }
}

// 加入群组业务
void ChatSession::addGroup(const NetworkService &conn,
                            const nlohmann::json &js, Timestamp time)
{
    int userid = js["id"].get<int>();
    int groupid = js["groupid"].get<int>();
    groupModel.AddGroup(userid, groupid, "normal");
}

// 添加好友业务 msgid id friendid
void ChatSession::addFriend(const NetworkService &conn,
                             const nlohmann::json &js, Timestamp time)
{
    int userid = js["id"].get<int>();
    int friendid = js["friendid"].get<int>();

    // 存储好友信息
    friendModel.insert(userid, friendid);
}

// 群组聊天业务
void ChatSession::groupChat(const NetworkService &conn,
                             const nlohmann::json &js, Timestamp time)
{
    int userid = js["id"].get<int>();
    int groupid = js["groupid"].get<int>();
    vector<int> useridVec = groupModel.QueryGroupUsers(
        userid, groupid); // useridVec为当前group中除当前userid外其他所有userid

    lock_guard<mutex> lock(m_mutex);
    for (int id : useridVec)
    {
        auto it = userConnectMap.find(id);
        if (it != userConnectMap.end())
        {
            // 转发群消息
            it->second.send(js);
        }
        else
        {
            // 查询toid是否在线
            User user = userSql.query(id);
            if (user.GetState() == "online")
            {
                //TODO redis
                //_redis.publish(id, js.dump());
            }
            else
            {
                // 存储离线群消息
                offlineSql.insert(id, js.dump());
            }
        }
    }
}

// bool ChatSession::relayMessage(const NetworkService &network, const
// nlohmann::json &js, Timestamp time)
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
//                 NetworkService(it->second).response(js);
//             }
//             else
//             { // 不在线
//                 offlineSql.storingMessage(id, js.dump());
//             }
//         }
//     }
//     catch (nlohmann::json::exception &e)
//     {
//         LOG_ERROR << "message: " << e.what() << '\n' << "exception id: " <<
//         e.id; errorEvent::protocolParseError(network); ret = false;
//     }
//     return ret;
// }
