#pragma once
#include "group_user.hpp"
#include <string>
#include <vector>

/**
 * @brief 对应mysql数据库group user表
 *
 */
class Group
{
public:
    Group(int id = -1, std::string name = "", std::string desc = "")
    {
        this->id = id;
        this->name = name;
        this->desc = desc;
    }

    void SetId(int id) { this->id = id; }
    void SetName(std::string name) { this->name = name; }
    void SetDesc(std::string desc) { this->desc = desc; }

    int GetId() { return this->id; }
    std::string GetName() { return this->name; }
    std::string GetDesc() { return this->desc; }
    std::vector<GroupUser> &GetUsers() { return this->users; }

private:
    /// @brief 群id
    int id;
    /// @brief 群名称
    std::string name;
    /// @brief 群描述
    std::string desc;
    /// @brief 群成员
    std::vector<GroupUser> users;
};