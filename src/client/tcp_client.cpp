#include <iostream>
#include <functional>
#include "tcp_client.h"
#include "muduo/base/Logging.h"
#include "package.h"
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
    tcpClient_.connect();
}

TcpClient::~TcpClient() {}

void TcpClient::MessageCallback(const muduo::net::TcpConnectionPtr& ptr,
                                muduo::net::Buffer* buf, muduo::Timestamp)
{
    // 先在缓冲区中解析出包,在调用业务层消息处理回调
    std::string view(buf->peek(), buf->readableBytes());
    std::string msg;
    int index = package::parse(view, msg);
    buf->retrieve(index);
    sessionhandle_(ptr, msg);
}
void TcpClient::connectCallbcak(const muduo::net::TcpConnectionPtr& ptr)
{
    if (ptr->connected()){
        std::cout << "已成功和服务器连接" << std::endl;
        connectionPtr_ = ptr;
        conn_ = std::make_unique<NetworkService>(connectionPtr_);
    }
    else
        std::cout << "已经和服务器断开连接" << std::endl;
}

void TcpClient::send(nlohmann::json json)
{
    //std::cout << "send message:" << msg << std::endl;
    conn_->send(json);
}
