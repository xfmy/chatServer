//网络层
#pragma once

#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>

#include <memory>
using namespace muduo;
using namespace muduo::net;

typedef std::function<void(EventLoop *)> ThreadInitCallback;
using businessCallback = std::function<void(const TcpConnectionPtr &,const std::string&, Timestamp)>;

class chatNetworkLayer: public muduo::noncopyable
{
public:
    chatNetworkLayer(uint16_t port);
    ~chatNetworkLayer();
    void start();
    void init();

    //void sendmsg(std::string msg);

    void setWriteCompleteCallback(const WriteCompleteCallback cb) { server.setWriteCompleteCallback(cb); }
    void setBusinessMessageCallback(const businessCallback cb) { this->businessMsgCallback = cb; }
    void setConnectCallback(const ConnectionCallback cb) { server.setConnectionCallback(cb); }
    void setThreadInitCallback(const ThreadInitCallback cb) { server.setThreadInitCallback(cb); }
    void setThreadNum(int count) { server.setThreadNum(count); }

private:
     void onMessageCallback(const TcpConnectionPtr &ptr, Buffer *buf, Timestamp);
     void onThreadInitCallback(EventLoop *);
    // void onConnectCallback(const TcpConnectionPtr &ptr);
    // void onWriteCompleteCallback(const TcpConnectionPtr &ptr);

    //void onMessageCompleteCallback(const TcpConnectionPtr &, const std::string &);
    //设置业务层的消息处理回调函数
    businessCallback businessMsgCallback;


     const std::string serverName = "char_server";
     InetAddress addr;
     EventLoop loop;
     TcpServer server;

     
};
