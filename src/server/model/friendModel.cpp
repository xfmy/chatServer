#include "model/friendModel.h"
#include "db/MysqlDataBase.h"
#include <fmt/core.h>
// 添加好友关系
void FriendModel::insert(int userid, int friendid)
{
    // 1.组装sql语句
    std::string sql = fmt::format("insert into friend values({}, {})", userid, friendid);
    //mysqlConnPool::getObject()->getStatement()->executeUpdate(sql);
    CMysqlDataBase::m_pMysql->RunSqlExec(sql);
}

// 返回用户好友列表
vector<chatdb::user> FriendModel::query(int userid)
{
    // 1.组装sql语句
    std::string sql = fmt::format("select a.id,a.name,a.state from user a inner join friend b on b.friendid = a.id where b.userid={}", userid);
    vector<chatdb::user> vec;
    sql::ResultSet *res = CMysqlDataBase::m_pMysql->RunSqlQuery(sql);
    res->beforeFirst();
    while(res->next()){
        chatdb::user user;
        user.setId(res->getInt("id"));
        user.setName(res->getString("name"));
        user.setState(res->getString("state"));
        vec.push_back(user);
    }
    res->close(); // 把userid用户的所有离线消息放入vec中返回
    return vec;
}