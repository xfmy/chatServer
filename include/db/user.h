#pragma once
#include <string>

namespace chatdb
{
    class user
    {
    public:
        user(int id_, std::string name_, std::string password_, std::string state_):
            id(id),
            name(name_),
            password(password_),
            state(state_){}
        user():id(-1),name("null"),password("null"),state("null"){}
        ~user(){}

        int getId() const { return id; }
        std::string getName() const { return name; }
        std::string getPassword() const { return password; }
        std::string getState() const { return state; }

        void setId(int id_) { id = id_; }
        void setName(std::string name_) { name = name_; }
        void setPassword(std::string password_) { password = password_; }
        void setState(std::string state_) { state = state_; }

    private:
        int id;                 // 用户id
        std::string name;       //用户名
        std::string password;   //用户密码
        std::string state;      //当前登录状态
    };
}