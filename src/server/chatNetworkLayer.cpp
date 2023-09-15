#include "package.h"
#include "distributor.h"
#include "chatNetworkLayer.h"
#include <nlohmann/json.hpp>
#include <muduo/base/TimeZone.h>
#include <muduo/base/Logging.h>
#include <string_view>
#include <string>
#include <optional>
#include <thread>
#include <functional>



void chatNetworkLayer::init(){
    //设置tcpserver的网络层相关回调
    server.setMessageCallback(std::bind(&chatNetworkLayer::onMessageCallback, this, _1, _2, _3));
    server.setThreadInitCallback(std::bind(&chatNetworkLayer::onThreadInitCallback, this, _1));
}

chatNetworkLayer::chatNetworkLayer(uint16_t port)
    : addr(port),
      loop(),
      server(&loop, addr, serverName)
{
    //设置东八区
    TimeZone time(8 * 1000 * 60 * 60,"loaclTime");
    Logger::setTimeZone(time);

    //初始化tcpSercer
    init();
}

chatNetworkLayer::~chatNetworkLayer()
{
}

void chatNetworkLayer::start(){
    
    server.start();
    loop.loop();
}



// void chatNetworkLayer::onWriteCompleteCallback(const TcpConnectionPtr &ptr)
// {
//     LOG_INFO <<ptr->peerAddress().toIpPort() + "消息发送完毕";
// }

void chatNetworkLayer::onMessageCallback(const TcpConnectionPtr &ptr, Buffer *buf, Timestamp time)
{
    // 首先解析一个完整的包,然后调用onMessageCompleteCallback处理
    //TODO:先关闭协议解析
    // std::string_view view(buf->peek(), buf->readableBytes());
    // std::optional<package> res = package::parse(view);
    // if(res.has_value())
    // {
    //     //回收缓冲区的内存
    //     buf->retrieve(res->GetSize());
    //     businessMsgCallback(ptr, res->data, time);
    // }

    std::string view(buf->peek(), buf->readableBytes());
    buf->retrieveAll();
    businessMsgCallback(ptr, view, time);
}

// void chatNetworkLayer::onConnectCallback(const TcpConnectionPtr &ptr)
// {
//     if (ptr->connected()){
//         LOG_INFO << ptr->peerAddress().toIpPort() + "客户发起来了连接";
//     }
//     else{
//         LOG_INFO << ptr->peerAddress().toIpPort() + "客户断开了连接";
//     }
    
// }

void chatNetworkLayer::onThreadInitCallback(EventLoop *)
{
    //TODO:都需要处理
    LOG_INFO << "work loop thread start";
}
