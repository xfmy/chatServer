#include <fmt/core.h>
#include <nlohmann/json.hpp>
#include "friend_model.h"
#include "mysql_database.h"


// 添加好友关系
void FriendModel::insert(int userid, int friendid)
{
    // 1.组装sql语句
    std::string sql = fmt::format("insert into friend values({}, {})", userid, friendid);
    //mysqlConnPool::getObject()->getStatement()->executeUpdate(sql);
    MysqlDataBase::GetInstance()->RunSqlExec(sql);
}

// 返回用户好友列表
std::vector<std::string> FriendModel::query(int userid)
{
    // 1.组装sql语句
    std::string sql = fmt::format("select a.id,a.name,a.state from user a inner join friend b on b.friendid = a.id where b.userid={}", userid);

    std::vector<std::string> vec;
    sql::ResultSet* res = MysqlDataBase::GetInstance()->RunSqlQuery(sql);

    res->beforeFirst();
    while (res->next())
    {
        nlohmann::json js;
        js["id"] = res->getInt("id");
        js["name"] = res->getString("name");
        js["state"] = res->getString("state");
        vec.push_back(js.dump());
    }
    res->close();
    return vec;
}