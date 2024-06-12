
#include <iostream>
#include <functional>

#include "chat_session.h"
#include "msgType.h"

using namespace std::placeholders;

// 系统支持的客户端命令列表
std::unordered_map<string, string> commandMap = {
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
    commandHandlerMap["help"] = std::bind(&ChatSession::help, this, _1);
    commandHandlerMap["chat"] = std::bind(&ChatSession::chat, this, _1);
    commandHandlerMap["addfriend"] = std::bind(&ChatSession::AddFriend, this, _1);
    commandHandlerMap["creategroup"] = std::bind(&ChatSession::CreateGroup, this, _1);
    commandHandlerMap["addgroup"] = std::bind(&ChatSession::AddGroup, this, _1);
    commandHandlerMap["groupchat"] = std::bind(&ChatSession::GroupChat, this, _1);
    commandHandlerMap["loginout"] = std::bind(&ChatSession::Logout, this, _1);
    mtx.lock();
}

void ChatSession::MessageCallbackHandle(const muduo::net::TcpConnectionPtr &ptr,
                                        std::string msg)
{
    // 接收ChatServer转发的数据，反序列化生成json数据对象
    json js = json::parse(msg);
    int msgtype = js["msgType"].get<int>();
    if (EnMsgType::ONE_CHAT_MSG == msgtype)
    {
        cout << js["time"].get<string>() << " [" << js["id"] << "]"
             << js["name"].get<string>() << " said: " << js["msg"].get<string>()
             << endl;
    }

    if (EnMsgType::GROUP_CHAT_MSG == msgtype)
    {
        cout << "群消息[" << js["groupid"] << "]:" << js["time"].get<string>()
             << " [" << js["id"] << "]" << js["name"].get<string>()
             << " said: " << js["msg"].get<string>() << endl;
    }

    if (LOGIN_MSG_ACK == msgtype)
    {
        doLoginResponse(js); // 处理登录响应的业务逻辑
    }

    if (REG_MSG_ACK == msgtype)
    {
        doRegResponse(js);
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
    string request = js.dump();

    tcpClient_.send(request);
    mtx.lock();
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
    std::string buffer = js.dump();
    tcpClient_.send(buffer);
    isMainMenuRunning_ = false;
}

void ChatSession::registerUser(std::string userName, std::string password)
{
    json js;
    js["msgType"] = EnMsgType::REG_MSG;
    js["name"] = userName;
    js["password"] = password;
    string request = js.dump();

    tcpClient_.send(request);
    mtx.lock();
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
    mtx.unlock();
}

// 处理登录的响应逻辑
void ChatSession::doLoginResponse(json &responsejs)
{
    if (200 != responsejs["code"].get<int>()) // 登录失败
    {
        cerr << responsejs["errmsg"] << endl;
        isLoginSuccess_ = false;
    }
    else // 登录成功
    {
        // 记录当前用户的id和name
        currentUser_.SetId(responsejs["user"]["id"].get<int>());
        currentUser_.SetName(responsejs["user"]["name"]);

        // 记录当前用户的好友列表信息
        if (responsejs.contains("friends"))
        {
            // 初始化
            currentUserFriendList_.clear();

            vector<string> vec = responsejs["friends"];
            for (string &str : vec)
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

            vector<string> vec1 = responsejs["groups"];
            for (string &groupstr : vec1)
            {
                json grpjs = json::parse(groupstr);
                Group group;
                group.SetId(grpjs["id"].get<int>());
                group.SetName(grpjs["groupname"]);
                group.SetDesc(grpjs["groupdesc"]);

                vector<string> vec2 = grpjs["users"];
                for (string &userstr : vec2)
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
        if (responsejs.contains("offlinemsg"))
        {
            vector<string> vec = responsejs["offlinemsg"];
            for (string &str : vec)
            {
                json js = json::parse(str);
                // time + [id] + name + " said: " + xxx
                if (ONE_CHAT_MSG == js["msgType"].get<int>())
                {
                    cout << js["time"].get<string>() << " [" << js["id"] << "]"
                         << js["name"].get<string>()
                         << " said: " << js["msg"].get<string>() << endl;
                }
                else
                {
                    cout << "群消息[" << js["groupid"]
                         << "]:" << js["time"].get<string>() << " [" << js["id"]
                         << "]" << js["name"].get<string>()
                         << " said: " << js["msg"].get<string>() << endl;
                }
            }
        }

        isLoginSuccess_ = true;
    }
    mtx.unlock();
}

// 显示当前登录成功用户的基本信息
void ChatSession::ShowCurrentUserData()
{
    cout << "======================login user======================" << endl;
    cout << "current login user => id:" << currentUser_.GetId()
         << " name:" << currentUser_.GetName() << endl;
    cout << "----------------------friend list---------------------" << endl;
    if (!currentUserFriendList_.empty())
    {
        for (User &user : currentUserFriendList_)
        {
            cout << user.GetId() << " " << user.GetName() << " "
                 << user.GetState() << endl;
        }
    }
    cout << "----------------------group list----------------------" << endl;
    if (!currentUserGroupList_.empty())
    {
        for (Group &group : currentUserGroupList_)
        {
            cout << group.GetId() << " " << group.GetName() << " "
                 << group.GetDesc() << endl;
            for (GroupUser &user : group.GetUsers())
            {
                cout << user.GetId() << " " << user.GetName() << " "
                     << user.GetState() << " " << user.GetRole() << endl;
            }
        }
    }
    cout << "======================================================" << endl;
}

void ChatSession::AddFriend(string friendid)
{
    int _friendid = atoi(friendid.c_str());
    json js;
    js["msgType"] = ADD_FRIEND_MSG;
    js["id"] = currentUser_.GetId();
    js["friendid"] = _friendid;
    string buffer = js.dump();

    tcpClient_.send(buffer);
}

void ChatSession::CreateGroup(string info)
{
    int idx = info.find(":");
    if (-1 == idx)
    {
        cerr << "创建组命令无效!" << endl;
        return;
    }

    string groupname = info.substr(0, idx);
    string groupdesc = info.substr(idx + 1, info.size() - idx);

    json js;
    js["msgType"] = CREATE_GROUP_MSG;
    js["id"] = currentUser_.GetId();
    js["groupname"] = groupname;
    js["groupdesc"] = groupdesc;
    string buffer = js.dump();

    tcpClient_.send(buffer);
}

void ChatSession::AddGroup(string info)
{
    int groupid = atoi(info.c_str());
    json js;
    js["msgType"] = ADD_GROUP_MSG;
    js["id"] = currentUser_.GetId();
    js["groupid"] = groupid;
    string buffer = js.dump();

    tcpClient_.send(buffer);
}

void ChatSession::GroupChat(string msg)
{
    int idx = msg.find(":");
    if (-1 == idx)
    {
        cerr << "groupchat command invalid!" << endl;
        return;
    }

    int groupid = atoi(msg.substr(0, idx).c_str());
    string message = msg.substr(idx + 1, msg.size() - idx);

    json js;
    js["msgType"] = GROUP_CHAT_MSG;
    js["id"] = currentUser_.GetId();
    js["name"] = currentUser_.GetName();
    js["groupid"] = groupid;
    js["msg"] = message;
    js["time"] = GetCurrentTime();
    string buffer = js.dump();

    tcpClient_.send(buffer);
}

void ChatSession::chat(string msg)
{
    int idx = msg.find(":"); // friendid:message
    if (-1 == idx)
    {
        cerr << "chat command invalid!" << endl;
        return;
    }

    int friendid = atoi(msg.substr(0, idx).c_str());
    string message = msg.substr(idx + 1, msg.size() - idx);

    json js;
    js["msgType"] = ONE_CHAT_MSG;
    js["id"] = currentUser_.GetId();
    js["name"] = currentUser_.GetName();
    js["toid"] = friendid;
    js["msg"] = message;
    js["time"] = GetCurrentTime();
    string buffer = js.dump();

    tcpClient_.send(buffer);
}

void ChatSession::help(std::string)
{
    cout << "show command list >>> " << endl;
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
        string command; // 存储命令
        int idx = commandbuf.find(":");
        if (-1 == idx)
        {
            command = commandbuf;
        }
        else
        {
            command = commandbuf.substr(0, idx);
        }
        auto it = commandHandlerMap.find(command);

        if (it == commandHandlerMap.end())
        {
            cerr << "invalid input command!" << endl;
            continue;
        }

        // 调用相应命令的事件处理回调，mainMenu对修改封闭，添加新功能不需要修改该函数
        it->second(commandbuf.substr(idx + 1, commandbuf.size() - idx));
        // tcpClient_.send(commandbuf.substr(idx + 1, commandbuf.size() - idx));
    }
}
