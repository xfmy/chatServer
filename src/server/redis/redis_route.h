#pragma once
#include <functional>
#include <string>
#include <sw/redis++/redis.h>

using namespace sw::redis;
using NotifyMessageHandle = std::function<void(int, std::string)>;

class RedisRoute
{
public:
    RedisRoute(std::string ip = "127.0.0.1", int port = 6379);
    ~RedisRoute();
    // 连接redis服务器
    // bool connect();

    // 向redis指定的通道channel发布消息
    bool publish(int channel, std::string message);

    // 向redis指定的通道subscribe订阅消息
    void subscribe(int channel);

    // 向redis指定的通道unsubscribe取消订阅消息
    bool unsubscribe(int channel);

    // 在独立线程中接收订阅通道中的消息
    void ObserverChannelMessage();

    // 初始化向业务层上报通道消息的回调对象
    void InitNotifyHandler(NotifyMessageHandle cb) { notifyMessageHandler_ = cb;}

    void MessageHandle(std::string channel, std::string msg);

private:
    // 回调操作，收到订阅的消息，给service层上报
    NotifyMessageHandle notifyMessageHandler_;

    std::unique_ptr<Redis> redisPtr_;
    std::unique_ptr<Subscriber> subscriberPtr_;
};
