/**
 * @file network_service.hpp
 * @brief 对muduo TcpConnection进行简单包装
 * 
 */

#pragma once

#include <nlohmann/json.hpp>
#include "package.h"
#include "muduo/net/TcpConnection.h"


class NetworkService
{
public:
    NetworkService(const muduo::net::TcpConnectionPtr ptr_) : ptr(ptr_) {}
    void send(const nlohmann::json& js) const { send(js.dump()); }
    void setContext(const boost::any& context) {ptr->setContext(context);}

    bool operator==(NetworkService obj)
    {
        if (this->ptr->peerAddress().toIpPort() ==
            obj.ptr->peerAddress().toIpPort())
            return true;
        else
            return false;
    }

private:
    void send(const std::string& msg) const
    {
        std::string sendMsg = package::encapsulation(msg);
        ptr->send(sendMsg);
    }
    

private:
    const muduo::net::TcpConnectionPtr ptr;
};
