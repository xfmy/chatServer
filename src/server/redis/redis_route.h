#pragma once
#include <functional>
#include <string>
#include <sw/redis++/redis.h>

using namespace sw::redis;
using NotifyMessageHandle = std::function<void(int, std::string)>;

/**
 * @brief 封装redis消息发布订阅相关操作
 * 
 */
class RedisRoute
{
public:
    RedisRoute(std::string ip = "127.0.0.1", int port = 6379);
    ~RedisRoute();

    /**
     * @brief 向redis指定的通道channel发布消息
     *
     * @param channel 通道(监听的用户id)
     * @param message 消息
     * @return success/fail
     */
    bool publish(int channel, std::string message);

    /**
     * @brief 向redis指定的通道subscribe订阅消息
     *
     * @param channel 通道(监听的用户id)
     */
    void subscribe(int channel);

    // 向redis指定的通道unsubscribe取消订阅消息

    /**
     * @brief 向redis指定的通道unsubscribe取消订阅消息
     *
     * @param channel 通道(监听的用户id)
     * @return success/fail
     */
    bool unsubscribe(int channel);

    /**
     * @brief 在独立线程中接收订阅通道中的消息
     *
     */
    void ObserverChannelMessage();

    /**
     * @brief 初始化向业务层上报通道消息的回调对象
     *
     * @param cb 回调对象
     */
    void InitNotifyHandler(NotifyMessageHandle cb) { notifyMessageHandler_ = cb;}

    /**
     * @brief 接收到订阅通道的消息,给service层上报
     *
     * @param channel 通道(监听的用户id)
     * @param msg 消息
     */
    void MessageHandle(std::string channel, std::string msg);

private:
    /// @brief 回调操作，收到订阅的消息，给service层上报
    NotifyMessageHandle notifyMessageHandler_;

    /// @brief redis操作类指针
    std::unique_ptr<Redis> redisPtr_;

    /// @brief redis订阅者
    std::unique_ptr<Subscriber> subscriberPtr_;
};
