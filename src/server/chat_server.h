/**
 * @file chat_server.h
 * @brief 聊天服务器类
 * 
 */
#pragma once
#include "tcp_service.h"
#include "chat_session.h"

class ChatServer{
public:
    ChatServer(int port);

    /// @brief 初始化
    void init();

    /// @brief 启动服务器 
    void start();

    /// @brief 初始化数据库
    void initDB();
private:
    /// @brief 业务类
    ChatSession session;
    /// @brief 网络服务类
    TcpService netWork;
};