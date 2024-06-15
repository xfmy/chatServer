#include <nlohmann/json.hpp>
#include <muduo/base/TimeZone.h>
#include <muduo/base/Logging.h>
#include <string_view>
#include <string>
#include <optional>
#include <thread>
#include <functional>

#include "package.h"
#include "tcp_service.h"

void TcpService::init()
{
    //设置tcpserver的网络层相关回调
    server.setMessageCallback(
        std::bind(&TcpService::onMessageCallback, this, _1, _2, _3));
    server.setThreadInitCallback(
        std::bind(&TcpService::onThreadInitCallback, this, _1));
}

TcpService::TcpService(uint16_t port)
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

TcpService::~TcpService() {}

void TcpService::start()
{
    server.start();
    loop.loop();
}

void TcpService::onMessageCallback(const TcpConnectionPtr &ptr, Buffer *buf,
                                   Timestamp time)
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

void TcpService::onThreadInitCallback(EventLoop *)
{
    LOG_INFO << "work loop thread start";
}
