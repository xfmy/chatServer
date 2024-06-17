#pragma once
#include <string>
#include <vector>

/**
 * @brief 离线消息ORM类
 * 
 */
class MessageModel
{
public:
    /**
     * @brief 插入用户离线消息
     *
     * @param userid 用户id
     * @param msg 离线消息
     */
    void insert(int userid, std::string msg);

    /**
     * @brief  删除用户离线消息
     *
     * @param userid 用户id
     */
    void remove(int userid);

    /**
     * @brief 查询用户离线消息
     *
     * @param userid 用户id
     * @return std::vector<std::string> 返回离线消息列表
     */
    std::vector<std::string> query(int userid);
};