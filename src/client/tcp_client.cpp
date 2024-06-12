#include "tcp_client.h"
#include "muduo/base/Logging.h"
#include <iostream>
#include <functional>
// using namespace muduo;
// using namespace muduo::net;
using namespace std::placeholders;

TcpClient::TcpClient(muduo::net::InetAddress peer_addr)
    : loop_(loopThread_.startLoop()),
      tcpClient_(loop_, peer_addr, "tcp_client")
{
    tcpClient_.setMessageCallback(
        std::bind(&TcpClient::MessageCallback, this, _1, _2, _3));
    tcpClient_.setConnectionCallback(
        std::bind(&TcpClient::connectCallbcak, this, _1));
    tcpClient_.enableRetry();
    // mtx_.lock();
    tcpClient_.connect();
    // std::lock_guard<std::mutex> lock(mtx_);
}

TcpClient::~TcpClient() {}

void TcpClient::MessageCallback(const muduo::net::TcpConnectionPtr& ptr,
                                muduo::net::Buffer* buf, muduo::Timestamp)
{
    std::string view(buf->peek(), buf->readableBytes());
    buf->retrieveAll();
    sessionhandle_(ptr, view);
}
void TcpClient::connectCallbcak(const muduo::net::TcpConnectionPtr& ptr)
{
    if (ptr->connected()){
        std::cout << "已成功和服务器连接" << std::endl;
        connectionPtr = ptr;
    }
    else
        std::cout << "已经和服务器断开连接" << std::endl;
}

void TcpClient::send(std::string msg)
{
    std::cout << "send message:" << msg << std::endl;
    connectionPtr->send(msg);
}
