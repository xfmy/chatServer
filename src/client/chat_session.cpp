#include <iostream>
#include <functional>

#include "chat_session.h"
#include "msg_type.h"

using namespace std::placeholders;

// 系统支持的客户端命令列表
std::unordered_map<std::string, std::string> commandMap = {
    {"help", "显示所有支持的命令，格式help"},
    {"chat", "一对一聊天，格式chat:friendid:message"},
    {"addfriend", "添加好友，格式addfriend:friendid"},
    {"creategroup", "创建群组，格式creategroup:groupname:groupdesc"},
    {"addgroup", "加入群组，格式addgroup:groupid"},
    {"groupchat", "群聊，格式groupchat:groupid:message"},
    {"loginout", "注销，格式loginout"}};

ChatSession::ChatSession(std::string ip, short port)
    : tcpClient_(muduo::net::InetAddress(ip, port))
{
    tcpClient_.SetSessionHandle(
        std::bind(&ChatSession::MessageCallbackHandle, this, _1, _2));

    // 注册事件回调
    commandHandlerMap_["help"] = std::bind(&ChatSession::help, this, _1);
    commandHandlerMap_["chat"] = std::bind(&ChatSession::chat, this, _1);
    commandHandlerMap_["addfriend"] =
        std::bind(&ChatSession::AddFriend, this, _1);
    commandHandlerMap_["creategroup"] =
        std::bind(&ChatSession::CreateGroup, this, _1);
    commandHandlerMap_["addgroup"] =
        std::bind(&ChatSession::AddGroup, this, _1);
    commandHandlerMap_["groupchat"] =
        std::bind(&ChatSession::GroupChat, this, _1);
    commandHandlerMap_["loginout"] = std::bind(&ChatSession::Logout, this, _1);

    // 保证后续登录以及注册业务的同步进行
    mtx_.lock();
}

void ChatSession::MessageCallbackHandle(const muduo::net::TcpConnectionPtr &ptr,
                                        std::string msg)
{
    // 接收ChatServer转发的数据，反序列化生成json数据对象
    json js = json::parse(msg);
    int msgtype = js["msgType"].get<int>();
    if (EnMsgType::ONE_CHAT_MSG == msgtype)
    {
        std::cout << js["time"].get<std::string>() << " [" << js["id"] << "]"
                  << js["name"].get<std::string>()
                  << " said: " << js["msg"].get<std::string>() << std::endl;
    }

    if (EnMsgType::GROUP_CHAT_MSG == msgtype)
    {
        std::cout << "群消息[" << js["groupid"]
                  << "]:" << js["time"].get<std::string>() << " [" << js["id"]
                  << "]" << js["name"].get<std::string>()
                  << " said: " << js["msg"].get<std::string>() << std::endl;
    }

    if (LOGIN_MSG_ACK == msgtype)
    {
        doLoginResponse(js); // 处理登录响应的业务逻辑
    }

    if (REG_MSG_ACK == msgtype)
    {
        doRegResponse(js); // 处理注册响应的业务逻辑
    }
}

// 获取系统时间（聊天信息需要添加时间信息）
std::string ChatSession::GetCurrentTime()
{
    auto tt =
        std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    struct tm *ptm = localtime(&tt);
    char date[60] = {0};
    sprintf(date, "%d-%02d-%02d %02d:%02d:%02d", (int)ptm->tm_year + 1900,
            (int)ptm->tm_mon + 1, (int)ptm->tm_mday, (int)ptm->tm_hour,
            (int)ptm->tm_min, (int)ptm->tm_sec);
    return std::string(date);
}

void ChatSession::Login(int ID, std::string password)
{
    json js;
    js["msgType"] = LOGIN_MSG;
    js["id"] = ID;
    js["password"] = password;

    tcpClient_.send(js);
    // 加锁保证登录响应逻辑处理完毕后执行
    mtx_.lock();
    if (isLoginSuccess_)
    {
        isMainMenuRunning_ = true;
        mainMenu();
    }
}

//"loginout" command handler
void ChatSession::Logout(std::string)
{
    json js;
    js["msgType"] = EnMsgType::LOGINOUT_MSG;
    js["id"] = currentUser_.GetId();
    tcpClient_.send(js);
    isMainMenuRunning_ = false;
}

void ChatSession::registerUser(std::string userName, std::string password)
{
    json js;
    js["msgType"] = EnMsgType::REG_MSG;
    js["name"] = userName;
    js["password"] = password;

    tcpClient_.send(js);
    // 加锁保证注册响应逻辑处理完毕后执行
    mtx_.lock();
}

void ChatSession::doRegResponse(json &responsejs)
{
    if (200 != responsejs["code"].get<int>()) // 注册失败
    {
        std::cerr << "name is already exist, register error!" << std::endl;
    }
    else // 注册成功
    {
        std::cout << "name register success, userid is " << responsejs["id"]
                  << ", do not forget it!" << std::endl;
    }
    mtx_.unlock();
}

// 处理登录的响应逻辑
void ChatSession::doLoginResponse(json &responsejs)
{
    if (200 != responsejs["code"].get<int>()) // 登录失败
    {
        std::cout << responsejs["errmsg"] << std::endl;
        isLoginSuccess_ = false;
    }
    else // 登录成功
    {
        // 记录当前用户的id和name
        currentUser_.SetId(responsejs["id"].get<int>());
        currentUser_.SetName(responsejs["name"]);

        // 记录当前用户的好友列表信息
        if (responsejs.contains("friends"))
        {
            // 初始化
            currentUserFriendList_.clear();

            std::vector<std::string> vec = responsejs["friends"];
            for (std::string &str : vec)
            {
                json js = json::parse(str);
                User user;
                user.SetId(js["id"].get<int>());
                user.SetName(js["name"]);
                user.SetState(js["state"]);
                currentUserFriendList_.push_back(user);
            }
        }

        // 记录当前用户的群组列表信息
        if (responsejs.contains("groups"))
        {
            // 初始化
            currentUserGroupList_.clear();

            std::vector<std::string> vec1 = responsejs["groups"];
            for (std::string &groupstr : vec1)
            {
                json grpjs = json::parse(groupstr);
                Group group;
                group.SetId(grpjs["id"].get<int>());
                group.SetName(grpjs["name"]);
                group.SetDesc(grpjs["desc"]);

                std::vector<std::string> vec2 = grpjs["users"];
                for (std::string &userstr : vec2)
                {
                    GroupUser user;
                    json js = json::parse(userstr);
                    user.SetId(js["id"].get<int>());
                    user.SetName(js["name"]);
                    user.SetState(js["state"]);
                    user.SetRole(js["role"]);
                    group.GetUsers().push_back(user);
                }

                currentUserGroupList_.push_back(group);
            }
        }

        // 显示登录用户的基本信息
        ShowCurrentUserData();

        // 显示当前用户的离线消息  个人聊天信息或者群组消息
        if (responsejs.contains("offlinemessage"))
        {
            std::cout << "----------------------历史消息---------------------"
                      << std::endl;
            std::vector<std::string> vec = responsejs["offlinemessage"];
            for (std::string &str : vec)
            {
                json js = json::parse(str);
                // time + [id] + name + " said: " + xxx
                if (ONE_CHAT_MSG == js["msgType"].get<int>())
                {
                    std::cout << js["time"].get<std::string>() << " ["
                              << js["id"] << "]"
                              << js["name"].get<std::string>()
                              << " said: " << js["msg"].get<std::string>()
                              << std::endl;
                }
                else
                {
                    std::cout << "群消息[" << js["groupid"]
                              << "]:" << js["time"].get<std::string>() << " ["
                              << js["id"] << "]"
                              << js["name"].get<std::string>()
                              << " said: " << js["msg"].get<std::string>()
                              << std::endl;
                }
            }
            std::cout << "---------------------------------------------------"
                      << std::endl;
        }

        isLoginSuccess_ = true;
    }
    mtx_.unlock();
}

// 显示当前登录成功用户的基本信息
void ChatSession::ShowCurrentUserData()
{
    std::cout << "======================login user======================"
              << std::endl;
    std::cout << "current login user => id:" << currentUser_.GetId()
              << " name:" << currentUser_.GetName() << std::endl;
    std::cout << "----------------------friend list---------------------"
              << std::endl;
    if (!currentUserFriendList_.empty())
    {
        for (User &user : currentUserFriendList_)
        {
            std::cout << user.GetId() << " " << user.GetName() << " "
                      << user.GetState() << std::endl;
        }
    }
    std::cout << "----------------------group list----------------------"
              << std::endl;
    if (!currentUserGroupList_.empty())
    {
        for (Group &group : currentUserGroupList_)
        {
            std::cout << group.GetId() << " " << group.GetName() << " "
                      << group.GetDesc() << std::endl;
            for (GroupUser &user : group.GetUsers())
            {
                std::cout << user.GetId() << " " << user.GetName() << " "
                          << user.GetState() << " " << user.GetRole()
                          << std::endl;
            }
        }
    }
    std::cout << "======================================================"
              << std::endl;
}

void ChatSession::AddFriend(std::string friendid)
{
    int _friendid = atoi(friendid.c_str());
    json js;
    js["msgType"] = ADD_FRIEND_MSG;
    js["id"] = currentUser_.GetId();
    js["friendid"] = _friendid;

    tcpClient_.send(js);
}

void ChatSession::CreateGroup(std::string info)
{
    int idx = info.find(":");
    if (-1 == idx)
    {
        std::cerr << "创建组命令无效!" << std::endl;
        return;
    }

    std::string groupname = info.substr(0, idx);
    std::string groupdesc = info.substr(idx + 1, info.size() - idx);

    json js;
    js["msgType"] = CREATE_GROUP_MSG;
    js["id"] = currentUser_.GetId();
    js["groupname"] = groupname;
    js["groupdesc"] = groupdesc;

    tcpClient_.send(js);
}

void ChatSession::AddGroup(std::string info)
{
    int groupid = atoi(info.c_str());
    json js;
    js["msgType"] = ADD_GROUP_MSG;
    js["id"] = currentUser_.GetId();
    js["groupid"] = groupid;
    tcpClient_.send(js);
}

void ChatSession::GroupChat(std::string msg)
{
    int idx = msg.find(":");
    if (-1 == idx)
    {
        std::cerr << "groupchat command invalid!" << std::endl;
        return;
    }

    int groupid = atoi(msg.substr(0, idx).c_str());
    std::string message = msg.substr(idx + 1, msg.size() - idx);

    json js;
    js["msgType"] = GROUP_CHAT_MSG;
    js["id"] = currentUser_.GetId();
    js["name"] = currentUser_.GetName();
    js["groupid"] = groupid;
    js["msg"] = message;
    js["time"] = GetCurrentTime();

    tcpClient_.send(js);
}

void ChatSession::chat(std::string msg)
{
    int idx = msg.find(":"); // friendid:message
    if (-1 == idx)
    {
        std::cerr << "chat command invalid!" << std::endl;
        return;
    }

    int friendid = atoi(msg.substr(0, idx).c_str());
    std::string message = msg.substr(idx + 1, msg.size() - idx);

    json js;
    js["msgType"] = ONE_CHAT_MSG;
    js["id"] = currentUser_.GetId();
    js["name"] = currentUser_.GetName();
    js["toid"] = friendid;
    js["msg"] = message;
    js["time"] = GetCurrentTime();

    tcpClient_.send(js);
}

void ChatSession::help(std::string)
{
    std::cout << "show command list >>> " << std::endl;
    std::cout << "help"
                 "显示所有支持的命令，格式help\n"
                 "chat"
                 "一对一聊天，格式chat:friendid:message\n"
                 "addfriend"
                 "添加好友，格式addfriend:friendid\n"
                 "creategroup"
                 "创建群组，格式creategroup:groupname:groupdesc\n"
                 "addgroup"
                 "加入群组，格式addgroup:groupid\n"
                 "groupchat"
                 "群聊，格式groupchat:groupid:message\n"
                 "loginout"
                 "注销，格式loginout\n";
}

// 主聊天页面程序
void ChatSession::mainMenu()
{
    help("");

    while (isMainMenuRunning_)
    {
        std::string commandbuf;
        std::cin >> commandbuf;
        std::string command; // 存储命令
        int idx = commandbuf.find(":");
        if (-1 == idx)
        {
            command = commandbuf;
        }
        else
        {
            command = commandbuf.substr(0, idx);
        }
        auto it = commandHandlerMap_.find(command);

        if (it == commandHandlerMap_.end())
        {
            std::cerr << "invalid input command!" << std::endl;
            continue;
        }

        // 调用相应命令的事件处理回调，mainMenu对修改封闭，添加新功能不需要修改该函数
        it->second(commandbuf.substr(idx + 1, commandbuf.size() - idx));
        // tcpClient_.send(commandbuf.substr(idx + 1, commandbuf.size() - idx));
    }
}
