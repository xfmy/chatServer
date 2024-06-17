/**
 * @file tcp_client.h
 * @author xf
 * @brief 封装muduo tcp client通信
 */
#pragma once
#include <memory>
#include <map>
#include <mutex>
#include <condition_variable>
#include <functional>
#include "muduo/net/TcpClient.h"
#include "muduo/net/InetAddress.h"
#include "muduo/net/EventLoop.h"
#include "muduo/net/EventLoopThread.h"
#include "muduo/net/Buffer.h"
#include "muduo/base/Timestamp.h"
#include "network_service.hpp"
using SessionHandle =
    std::function<void(const muduo::net::TcpConnectionPtr&, std::string)>;

class TcpClient
{
public:
    TcpClient(muduo::net::InetAddress peer_addr);
    ~TcpClient();

    /**
     * @brief 向服务端发送消息
     *
     * @param msg 消息内容
     */
    void send(nlohmann::json msg);

    /**
     * @brief 消息回调
     *
     * @param ptr muduo tcp链接类
     * @param buf 缓冲区
     */
    void MessageCallback(const muduo::net::TcpConnectionPtr& ptr,
                         muduo::net::Buffer* buf, muduo::Timestamp);
    void SetSessionHandle(SessionHandle handle) { sessionhandle_ = handle; }
    /**
     * @brief 连接回调
     *
     * @param ptr muduo tcp链接类
     */
    void connectCallbcak(const muduo::net::TcpConnectionPtr& ptr);

private:
    /// @brief 事件循环线程类
    muduo::net::EventLoopThread loopThread_;
    /// @brief 事件循环
    muduo::net::EventLoop* loop_;
    /// @brief tcp客户端类
    muduo::net::TcpClient tcpClient_;
    /// @brief 连接管理类
    muduo::net::TcpConnectionPtr connectionPtr_;
    std::unique_ptr<NetworkService> conn_;
    /// @brief 业务层回调
    SessionHandle sessionhandle_;
};
