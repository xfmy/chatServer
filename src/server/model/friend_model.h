#pragma once
#include <vector>
#include "user.h"

/**
 * @brief 维护好友信息的操作接口方法
 *
 */
class FriendModel
{
public:

    /**
     * @brief 添加好友关系
     *
     * @param userid 用户id
     * @param friendid 待添加好友id
     */
    void insert(int userid, int friendid);

    /**
     * @brief 返回用户好友列表
     *
     * @param userid 用户id
     * @return std::vector<std::string> 用户好友列表
     */
    std::vector<std::string> query(int userid);
};