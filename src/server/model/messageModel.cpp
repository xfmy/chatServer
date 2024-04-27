#include "db/MysqlDataBase.h"
#include "model/messageModel.h"
#include <fmt/core.h>

void messageModel::storingMessage(int id, std::string msg)
{
    //auto statement = mysqlConnPool::getObject()->getStatement();
    std::string sql = fmt::format("insert into offlinemessage values({}, '{}')", id, msg);
    CMysqlDataBase::m_pMysql->RunSqlExec(sql);
}
std::vector<std::string> messageModel::pullOfflineMessages(int id)
{
    std::vector<std::string> resArr;
    //auto statement = mysqlConnPool::getObject()->getStatement();
    std::string sql = fmt::format("select * from offlinemessage where userid={}", id);
    sql::ResultSet *res = CMysqlDataBase::m_pMysql->RunSqlQuery(sql);
    res->beforeFirst();
    while (res->next())
    {
        resArr.push_back(res->getString("message"));
        //res->rowDeleted();
    }
    res->close();
    return resArr;
}