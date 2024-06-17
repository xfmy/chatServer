#include <muduo/base/Logging.h>
#include "redis_route.h"

using namespace std::placeholders;

RedisRoute::RedisRoute(std::string ip, int port)
{
    ConnectionOptions opts1;
    opts1.host = ip;
    opts1.port = port;
    opts1.socket_timeout = std::chrono::milliseconds(100);
    redisPtr_ = std::make_unique<Redis>(Redis(opts1));

    subscriberPtr_ = std::make_unique<Subscriber>(redisPtr_->subscriber());
}

RedisRoute::~RedisRoute() {}

bool RedisRoute::publish(int channel, std::string message)
{
    return redisPtr_->publish(std::to_string(channel), message);
}

void RedisRoute::subscribe(int channel)
{
    subscriberPtr_->on_message(
        std::bind(&RedisRoute::MessageHandle, this, _1, _2));
    subscriberPtr_->subscribe(std::to_string(channel));
}

bool RedisRoute::unsubscribe(int channel)
{
    subscriberPtr_->unsubscribe(std::to_string(channel));
    return true;
}

void RedisRoute::ObserverChannelMessage() 
{
    // Consume messages in a loop.
    while (true)
    {
        try
        {
            subscriberPtr_->consume();
        }
        catch (const TimeoutError &e)
        {
            continue;
        }
        catch (const Error &err)
        {
            LOG_ERROR << "Found unknown error, error message:"<<err.what();
        }
    }
}

void RedisRoute::MessageHandle(std::string channel, std::string msg)
{
    notifyMessageHandler_(std::atoi(channel.c_str()), msg);
}
