#pragma once
#include "user.h"

// 群组用户，多了一个role角色信息，从User类直接继承，复用User的其它信息
class GroupUser : public User
{
public:
    void SetRole(std::string role) { this->role = role; }
    std::string GetRole() { return this->role; }

private:
    std::string role;
};