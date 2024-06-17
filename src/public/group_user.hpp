#pragma once
#include "user.h"

/**
 * @brief 群组用户，多了一个role角色信息，从User类直接继承，复用User的其它信息
 *
 */
class GroupUser : public User
{
public:
    void SetRole(std::string role) { this->role = role; }
    std::string GetRole() { return this->role; }

private:
    /// @brief 群成员属性,分为创建者和普通群员
    std::string role;
};