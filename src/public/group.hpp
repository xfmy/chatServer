#pragma once
#include "group_user.hpp"
#include <string>
#include <vector>
using namespace std;

// group user表的ORM类
class Group
{
public:
    Group(int id = -1, string name = "", string desc = "")
    {
        this->id = id;
        this->name = name;
        this->desc = desc;
    }

    void SetId(int id) { this->id = id; }
    void SetName(string name) { this->name = name; }
    void SetDesc(string desc) { this->desc = desc; }

    int GetId() { return this->id; }
    string GetName() { return this->name; }
    string GetDesc() { return this->desc; }
    vector<GroupUser> &GetUsers() { return this->users; }

private:
    /// @brief 群id
    int id;
    /// @brief 群名称
    string name;
    /// @brief 群描述
    string desc;
    /// @brief 群成员
    vector<GroupUser> users;
};