#pragma once
#include <cppconn/resultset.h>
#include "user.h"

/**
 * @brief user表orm类
 * 
 */
class UserModel
{
public:
    // user表的增加方法
    bool insert(User& user);
    // user表的查找方法
    // 根据用户id查找用户信息
    User query(int id);
    // 更新用户状态信息
    bool UpdateState(User user);
    // 重置用户的状态信息
    void ResetState();
};
