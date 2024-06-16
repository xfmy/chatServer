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
    // 先在缓冲区中解析出包,在调用业务层消息处理回调
    std::string view(buf->peek(), buf->readableBytes());
    std::string msg;
    int index = package::parse(view,msg);
    if(index == 0) return;
    buf->retrieve(index);
    businessMsgCallback(ptr, msg, time);
}

void TcpService::onThreadInitCallback(EventLoop *)
{
    LOG_INFO << "work loop thread start";
}
