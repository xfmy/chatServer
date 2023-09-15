#include "db/userModel.h"
#include <cppconn/statement.h>
#include <fmt/core.h>
#include "mysqlConnPool.h"

// int userModel::executeUpdata(std::string sql, sql::Statement *stmt)
// {
//     return stmt->executeUpdate(sql);
// }

int userModel::insertUser(const chatdb::user &user)
{
    std::string sql = fmt::format("insert into user(name, password, state) values('{}', '{}', '{}')", user.getName().c_str(), user.getPassword().c_str(), user.getState().c_str());
    return mysqlConnPool::getObject()->getStatement()->executeUpdate(sql);
}
int userModel::updataAll(const chatdb::user &user)
{
    std::string sql = fmt::format("updata user set name = '{}' password = '{}' state = '{}' where id = {}", user.getName().c_str(), user.getPassword().c_str(), user.getState().c_str(), user.getId());
    return mysqlConnPool::getObject()->getStatement()->executeUpdate(sql);
}
int userModel::updataState(const chatdb::user &user)
{
    std::string sql = fmt::format("updata user set state = '{}' where id = {}", user.getState().c_str(), user.getId());
    return mysqlConnPool::getObject()->getStatement()->executeUpdate(sql);
}



int userModel::queryId(const std::string name)
{
    std::string sql = fmt::format("select id from user where name='{}'", name);
    sql::ResultSet *res = mysqlConnPool::getObject()->getStatement()->executeQuery(sql);
    res->last();
    size_t id = -1;
    if (res->getRow() != 0)
    {
        res->first();
        id = res->getInt("id");
    }
    res->close();
    return id;
}
// chatdb::user userModel::query(int id, sql::Statement *stmt)
// {
//     std::string sql = fmt::format("select * from user where id={}", id);
//     sql::ResultSet *res = stmt->executeQuery(sql);
//     chatdb::user user;
//     if(res->first()){
//         user.setId(res->getInt("id"));
//         user.setName(res->getString("name"));
//         user.setPassword(res->getString("password"));
//         user.setState(res->getString("state"));
//     }
//     res->close();
//     return user;
// }

chatdb::user userModel::queryUser(const std::string name, const std::string password)
{
    std::string sql = fmt::format("select * from user where name = '{}' and password = '{}'", name, password);

    sql::ResultSet *res = mysqlConnPool::getObject()->getStatement()->executeQuery(sql);
    chatdb::user user;
    if (res->first())
    {
        user.setId(res->getInt("id"));
        user.setName(res->getString("name"));
        user.setPassword(res->getString("password"));
        user.setState(res->getString("state"));
    }
    res->close();
    return user;
}
