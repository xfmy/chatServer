/**
 * @file user.h
 * @brief 对应mysql数据库user表字段
 * 
 */
#pragma once
#include <string>

class User
{
public:
    User(int id_, std::string name_, std::string password_, std::string state_)
        : id(id_),
          name(name_),
          password(password_),
          state(state_)
    {
    }
    User() : id(-1), name("null"), password("null"), state("offline") {}
    ~User() {}

    int GetId() const { return id; }
    std::string GetName() const { return name; }
    std::string GetPassword() const { return password; }
    std::string GetState() const { return state; }

    void SetId(int id_) { id = id_; }
    void SetName(std::string name_) { name = name_; }
    void SetPassword(std::string password_) { password = password_; }
    void SetState(std::string state_) { state = state_; }

private:
    int id;               // 用户id
    std::string name;     //用户名
    std::string password; //用户密码
    std::string state;    //当前登录状态
};
