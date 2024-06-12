#include "mysql_database.h"
#include "offline_message_model.h"
#include <fmt/core.h>

void MessageModel::insert(int id, std::string msg)
{
    // auto statement = mysqlConnPool::getObject()->getStatement();
    std::string sql =
        fmt::format("insert into offlinemessage values({}, '{}')", id, msg);
    MysqlDataBase::GetInstance()->RunSqlExec(sql);
}
std::vector<std::string> MessageModel::query(int id)
{
    std::vector<std::string> resArr;
    // auto statement = mysqlConnPool::getObject()->getStatement();
    std::string sql =
        fmt::format("select * from offlinemessage where userid={}", id);
    sql::ResultSet *res = MysqlDataBase::GetInstance()->RunSqlQuery(sql);
    res->beforeFirst();
    while (res->next())
    {
        resArr.push_back(res->getString("message"));
        // res->rowDeleted();
    }
    res->close();
    return resArr;
}
// 删除用户离线消息
void MessageModel::remove(int userid)
{
    // 1. 组装sql语句
    std::string sql =
        fmt::format("delete from offlinemessage where userid = {}", userid);

    MysqlDataBase::GetInstance()->RunSqlExec(sql);
}