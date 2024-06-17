/**
 * @file tcp_service.h
 * @brief 封装muduo tcp server
 * 
 */
#pragma once

#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <memory>
using namespace muduo;
using namespace muduo::net;

typedef std::function<void(EventLoop *)> ThreadInitCallback;
using businessCallback = std::function<void(const TcpConnectionPtr &,const std::string&, Timestamp)>;

class TcpService: public muduo::noncopyable
{
public:
    TcpService(uint16_t port);
    ~TcpService();
    void start();
    void init();
    void setWriteCompleteCallback(const WriteCompleteCallback cb) { server.setWriteCompleteCallback(cb); }
    void setBusinessMessageCallback(const businessCallback cb) { this->businessMsgCallback = cb; }
    void setConnectCallback(const ConnectionCallback cb) { server.setConnectionCallback(cb); }
    void setThreadInitCallback(const ThreadInitCallback cb) { server.setThreadInitCallback(cb); }
    void setThreadNum(int count) { server.setThreadNum(count); }

private:
     void onMessageCallback(const TcpConnectionPtr &ptr, Buffer *buf, Timestamp);
     void onThreadInitCallback(EventLoop *);
     
    /// @brief 设置业务层的消息处理回调函数
    businessCallback businessMsgCallback;

     const std::string serverName = "char_server";
     InetAddress addr;
     EventLoop loop;
     TcpServer server;
};
