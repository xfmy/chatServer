#pragma once
#include "model/user.h"
#include <cppconn/resultset.h>
class userModel
{
private:
    //int executeUpdata(std::string sql, sql::Statement *stmt);
    
public:
    int insertUser(const chatdb::user &user);
    int updataAll(const chatdb::user &user);
    int updataState(const chatdb::user &user);

    
     int queryId(const std::string name);
    // chatdb::user query(int id);
    chatdb::user queryUser(const std::string name, const std::string password);



};
