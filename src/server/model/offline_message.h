#pragma once
#include <string>

/**
 * @brief 对应mysql离线消息表字段
 * 
 */
class OffLineMessage
{
private:
    int userId_;          //用户id
    std::string message_; //离线存储的消息(json格式)
};
