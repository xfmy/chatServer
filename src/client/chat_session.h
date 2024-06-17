#pragma once
#include <atomic>
#include <string>
#include <nlohmann/json.hpp>
#include <mutex>

#include "user.h"
#include "group.hpp"
#include "tcp_client.h"

using json = nlohmann::json;

class ChatSession
{
public:
    ChatSession(std::string ip, short port);
    ~ChatSession() = default;

    void MessageCallbackHandle(const muduo::net::TcpConnectionPtr &ptr,
                               std::string jsMsg);

    /**
     * @brief 用户登录
     *
     * @param info 登录信息
     */
    void Login(int Id, std::string password);

    /**
     * @brief 登出
     *
     */
    void Logout(std::string);

    /**
     * @brief 用户注册
     *
     * @param userName 用户名
     * @param password 密码
     */
    void registerUser(std::string userName, std::string password);

    /**
     * @brief 主界面
     *
     */
    void mainMenu();

private:
    /**
     * @brief 获取当前时间
     *
     * @return 返回字符串格式
     */
    std::string GetCurrentTime();

    /**
     * @brief 处理注册的响应逻辑
     *
     * @param responsejs 响应的js数据
     */
    void doRegResponse(json &responsejs);

    /**
     * @brief 处理登录的响应逻辑
     *
     * @param responsejs 响应的js数据
     */
    void doLoginResponse(json &responsejs);

    /**
     * @brief 显示当前登录成功用户的基本信息
     *
     */
    void ShowCurrentUserData();

    /**
     * @brief 添加好友
     *
     * @param friendid 好友id
     */
    void AddFriend(std::string friendid);

    /**
     * @brief 创建群聊天室
     *
     * @param info 群聊天室信息
     */
    void CreateGroup(std::string info);

    /**
     * @brief 添加聊天室
     *
     * @param info 聊天室信息
     */
    void AddGroup(std::string info);

    /**
     * @brief 群聊消息
     *
     * @param msg 消息内容
     */
    void GroupChat(std::string msg);

    /**
     * @brief 单聊
     *
     * @param msg 聊天信息
     */
    void chat(std::string msg);

    /**
     * @brief 帮助信息
     *
     */
    void help(std::string);

private:
    TcpClient tcpClient_;
    // 记录当前系统登录的用户信息
    User currentUser_;
    // 记录当前登录用户的好友列表信息
    std::vector<User> currentUserFriendList_;
    // 记录当前登录用户的群组列表信息
    std::vector<Group> currentUserGroupList_;

    // 控制主菜单页面程序
    bool isMainMenuRunning_ = false;

    // 记录登录状态
    std::atomic_bool isLoginSuccess_{false};
    std::mutex mtx_;
    // 注册系统支持的客户端命令处理
    std::unordered_map<std::string, std::function<void(std::string)>>
        commandHandlerMap_;
};