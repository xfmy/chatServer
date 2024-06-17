#include <nlohmann/json.hpp>
#include <fmt/core.h>
#include <vector>
#include <thread>
#include <muduo/base/Logging.h>
#include "chat_session.h"
#include "error_event.h"
#include "msg_type.h"

void ChatSession::onConnectCallback(const TcpConnectionPtr &conn)
{
    // 客户端断开链接
    // 尝试做异常断开连接处理
    if (conn->disconnected())
    {
        clientCloseException(conn);
        conn->shutdown();
    }
}

ChatSession::ChatSession()
{
    //注册相关事件回调
    userEventCallbackMap_.emplace(
        std::make_pair<EnMsgType, SessionEventCallback>(
            EnMsgType::LOGIN_MSG,
            std::bind(&ChatSession::login, this, _1, _2, _3)));
    userEventCallbackMap_.emplace(
        std::make_pair<EnMsgType, SessionEventCallback>(
            EnMsgType::REG_MSG,
            std::bind(&ChatSession::registerUser, this, _1, _2, _3)));
    userEventCallbackMap_.emplace(
        std::make_pair<EnMsgType, SessionEventCallback>(
            EnMsgType::ONE_CHAT_MSG,
            std::bind(&ChatSession::oneChat, this, _1, _2, _3)));
    userEventCallbackMap_.emplace(
        std::make_pair<EnMsgType, SessionEventCallback>(
            EnMsgType::LOGINOUT_MSG,
            std::bind(&ChatSession::logout, this, _1, _2, _3)));
    userEventCallbackMap_.emplace(
        std::make_pair<EnMsgType, SessionEventCallback>(
            EnMsgType::ADD_GROUP_MSG,
            std::bind(&ChatSession::addGroup, this, _1, _2, _3)));
    userEventCallbackMap_.emplace(
        std::make_pair<EnMsgType, SessionEventCallback>(
            EnMsgType::ADD_FRIEND_MSG,
            std::bind(&ChatSession::addFriend, this, _1, _2, _3)));
    userEventCallbackMap_.emplace(
        std::make_pair<EnMsgType, SessionEventCallback>(
            EnMsgType::GROUP_CHAT_MSG,
            std::bind(&ChatSession::groupChat, this, _1, _2, _3)));

    // 设置上报消息的回调
    redisRoute_.InitNotifyHandler(
        std::bind(&ChatSession::handleRedisSubscribeMessage, this, _1, _2));
    // 单独线程处理redis消息订阅
    std::thread th(&RedisRoute::ObserverChannelMessage, &redisRoute_);
    th.detach();
}

void ChatSession::distribute(const TcpConnectionPtr &ptr,
                             const std::string &mes, Timestamp time)
{
    try
    {
        nlohmann::json js = nlohmann::json::parse(mes);
        EnMsgType msgType = static_cast<EnMsgType>(js["msgType"].get<int>());

        const auto &it = userEventCallbackMap_.find(msgType);
        if (it != userEventCallbackMap_.end())
        {
            it->second(ptr, js, time);
        }
        else
        {
            ErrorEvent::protocolParseError(ptr);
        }
    }
    catch (nlohmann::json::exception &e)
    {
        LOG_ERROR << fmt::format("message:{} \\  exception id: {}", e.what(),
                                 e.id);
        ErrorEvent::protocolParseError(ptr);
    }
}

void ChatSession::handleRedisSubscribeMessage(int userid, string msg)
{
    std::lock_guard<std::mutex> lock(mtx_);
    auto it = userConnectMap_.find(userid);
    if (it != userConnectMap_.end())
    {
        nlohmann::json json = nlohmann::json::parse(msg);
        it->second.send(json);
        return;
    }

    // 存储该用户的离线消息
    offlineMessageModel_.insert(userid, msg);
}

//用户登录
void ChatSession::login(const NetworkService &conn, const nlohmann::json &js,
                        Timestamp)
{
    LOG_INFO << "do login service";
    int id = js["id"].get<int>();
    string pwd = js["password"];
    User user = userModel_.query(id);

    try
    {
        if (user.GetId() == id && user.GetPassword() == pwd)
        {
            if (user.GetState() == "online")
            {
                // 该用户已经登陆，不允许重复登陆
                nlohmann::json response;
                response["msgType"] = LOGIN_MSG_ACK;
                response["code"] = 2;
                response["errmsg"] = "User name has been logged in";
                conn.send(response);
            }
            else
            {
                // 登陆成功,记录用户连接信息  要注意线程安全
                {
                    std::lock_guard<std::mutex> lock(mtx_);
                    userConnectMap_.insert({user.GetId(), conn});
                }

                // id 用户登陆成功后，向redis订阅channel id
                redisRoute_.subscribe(id);

                // 更新用户状态信息
                user.SetState("online");
                userModel_.UpdateState(user);

                // 登陆成功
                nlohmann::json response;
                response["msgType"] = LOGIN_MSG_ACK;
                response["code"] = 200;
                response["id"] = user.GetId();
                response["name"] = user.GetName();

                // 查询用户是否有离线消息
                std::vector<std::string> vec = offlineMessageModel_.query(id);
                if (!vec.empty())
                {
                    response["offlinemessage"] = vec;
                    // 读取该用户的离线消息后，把该用户的所有离线消息删除掉
                    offlineMessageModel_.remove(id);
                }
                // 查询用户的好友信息，并返回
                std::vector<std::string> userVec = friendModel_.query(id);
                if (!userVec.empty())
                {
                    response["friends"] = userVec;
                }
                // 查询用户的群组信息，并返回
                std::vector<Group> groupuserVec = groupModel_.QueryGroups(id);
                if (!groupuserVec.empty())
                {
                    std::vector<std::string> groupInfo;
                    for (Group group : groupuserVec)
                    {
                        nlohmann::json groupjs;
                        groupjs["id"] = group.GetId();
                        groupjs["name"] = group.GetName();
                        groupjs["desc"] = group.GetDesc();
                        std::vector<std::string> userInfo;
                        for (GroupUser user : group.GetUsers())
                        {
                            nlohmann::json js;
                            js["id"] = user.GetId();
                            js["name"] = user.GetName();
                            js["state"] = user.GetState();
                            js["role"] = user.GetRole();
                            userInfo.push_back(js.dump());
                        }
                        groupjs["users"] = userInfo;
                        groupInfo.push_back(groupjs.dump());
                    }
                    response["groups"] = groupInfo;
                }
                conn.send(response);
            }
        }
        else
        {
            // 登陆失败，该用户不存在，用户存在但是密码错误
            nlohmann::json response;
            response["msgType"] = LOGIN_MSG_ACK;
            response["code"] = 1;
            response["errmsg"] = "Wrong account or password";
            conn.send(response);
        }
    }
    catch (nlohmann::json::exception &e)
    {
        LOG_ERROR << fmt::format("message:{} \\  exception id: {}", e.what(),
                                 e.id);
        ErrorEvent::protocolParseError(conn);
    }
}

//退出登录
void ChatSession::logout(const NetworkService &conn, const nlohmann::json &js,
                         Timestamp time)
{
    int userid = js["id"].get<int>();

    {
        std::lock_guard<std::mutex> lock(mtx_);
        auto it = userConnectMap_.find(userid);
        if (it != userConnectMap_.end())
        {
            userConnectMap_.erase(it);
        }
    }

    // 用户注销，相当于就是下线，在redis中取消订阅通道
    redisRoute_.unsubscribe(userid);

    // 更新用户的状态信息
    User user(userid, "", "", "offline");
    userModel_.UpdateState(user);

    LOG_INFO << user.GetId() + ":退出登录";
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
        int toid = js["toid"].get<int>();
        {
            std::lock_guard<std::mutex> lock(mtx_);
            auto it = userConnectMap_.find(toid);
            if (it != userConnectMap_.end())
            { // 当前服务器在线
                NetworkService(it->second).send(js);
            }
            else
            {
                // 查询toid是否在线
                User user = userModel_.query(toid);
                if (user.GetState() == "online")
                {
                    redisRoute_.publish(toid, js.dump());
                }
                // 不在线
                offlineMessageModel_.insert(toid, js.dump());
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
        ErrorEvent::protocolParseError(conn);
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

        int res = userModel_.insert(user);
        nlohmann::json retJs;
        if (!res)
        {
            retJs["code"] = 404;
            retJs["msg"] = "该账户已经注册,请重新输入";
        }
        else
        {
            // userModel_.insertUser(user);
            // user = userModel_.queryUser(name, password);
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
        ErrorEvent::protocolParseError(conn);
    }
}

// 加入群组业务
void ChatSession::addGroup(const NetworkService &conn, const nlohmann::json &js,
                           Timestamp time)
{
    int userid = js["id"].get<int>();
    int groupid = js["groupid"].get<int>();
    groupModel_.AddGroup(userid, groupid, "normal");
}

// 添加好友业务 msgid id friendid
void ChatSession::addFriend(const NetworkService &conn,
                            const nlohmann::json &js, Timestamp time)
{
    int userid = js["id"].get<int>();
    int friendid = js["friendid"].get<int>();

    // 存储好友信息
    friendModel_.insert(userid, friendid);
}

// 群组聊天业务
void ChatSession::groupChat(const NetworkService &conn,
                            const nlohmann::json &js, Timestamp time)
{
    int userid = js["id"].get<int>();
    int groupid = js["groupid"].get<int>();
    std::vector<int> useridVec = groupModel_.QueryGroupUsers(
        userid, groupid); // useridVec为当前group中除当前userid外其他所有userid

    std::lock_guard<std::mutex> lock(mtx_);
    for (int id : useridVec)
    {
        auto it = userConnectMap_.find(id);
        if (it != userConnectMap_.end())
        {
            // 转发群消息
            it->second.send(js);
        }
        else
        {
            // 查询toid是否在线
            User user = userModel_.query(id);
            if (user.GetState() == "online")
            {
                redisRoute_.publish(id, js.dump());
            }
            else
            {
                // 存储离线群消息
                offlineMessageModel_.insert(id, js.dump());
            }
        }
    }
}

ChatSession::~ChatSession()
{
    for (auto &it : userConnectMap_)
    {
        clientCloseException(it.second);
    }
    userConnectMap_.clear();
}

// 处理客户端异常退出
void ChatSession::clientCloseException(const NetworkService &conn)
{
    User user;
    {
        std::lock_guard<std::mutex> lock(mtx_);
        for (auto it = userConnectMap_.begin(); it != userConnectMap_.end();
             ++it)
        {
            if (it->second == conn)
            {
                // 从map表删除用户的链接信息
                user.SetId(it->first);
                userConnectMap_.erase(it);
                break;
            }
        }
    }

    // 用户注销，相当于就是下线，在redis中取消订阅通道
    redisRoute_.unsubscribe(user.GetId());

    // 更新用户的状态信息
    if (user.GetId() != -1)
    {
        user.SetState("offline");
        userModel_.UpdateState(user);
    }
}