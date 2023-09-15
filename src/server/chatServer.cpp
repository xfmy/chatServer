#include "chatServer.h"
#include "mysqlConnPool.h"
#include <thread>
#include <functional>
chatServer::chatServer(int port) : netWork(port)
{
}

void chatServer::init()
{
    // 设置workLOOP数量以及相应的回调函数
    netWork.setThreadNum(std::thread::hardware_concurrency());
    netWork.setBusinessMessageCallback(std::bind(&chatBusiness::distribute, &this->business, _1, _2, _3));
    netWork.setConnectCallback(std::bind(&chatBusiness::onConnectCallback, &this->business, _1));
    netWork.setWriteCompleteCallback(std::bind(&chatBusiness::onConnectCallback, &this->business, _1));

    //初始化数据库
    mysqlConnPool::getObject()->init();
    // 设置业务版本号
    // distributor.
}

void chatServer::start()
{
    netWork.start();
}
