#include <cppconn/statement.h>
#include <fmt/core.h>
#include "mysql_database.h"
#include "user_model.h"

bool UserModel::insert(User &user)
{
    std::string sql = fmt::format(
        "insert into user(name, password, state) values('{}', '{}', '{}')",
        user.GetName().c_str(), user.GetPassword().c_str(),
        user.GetState().c_str());
    bool res = MysqlDataBase::GetInstance()->RunSqlExec(sql);
    if(res){
        sql = fmt::format("select id from user where name='{}' and password='{}';",user.GetName(),user.GetPassword());
        int id = MysqlDataBase::GetInstance()->RunSqlQuery(sql)->getInt("id");
        user.SetId(id);
    }
    return res; 
}
void UserModel::ResetState()
{
    std::string sql = "update user set state='offline' where state='online'";
    MysqlDataBase::GetInstance()->RunSqlExec(sql);
}
bool UserModel::UpdateState(User user)
{
    std::string sql = fmt::format("update user set state = '{}' where id = {}",
                                  user.GetState().c_str(), user.GetId());
    return MysqlDataBase::GetInstance()->RunSqlExec(sql);
}


User UserModel::query(int id)
{
    std::string sql = fmt::format("select * from user where id = {}", id);
    sql::ResultSet *res = MysqlDataBase::GetInstance()->RunSqlQuery(sql);
    User user;
    res->beforeFirst();
    if (res->next())
    {
        user.SetId(res->getInt("id"));
        user.SetName(res->getString("name"));
        user.SetPassword(res->getString("password"));
        user.SetState(res->getString("state"));
    }
    res->close();
    return user;
}

// int userModel::queryId(const std::string name)
// {
//     std::string sql = fmt::format("select id from user where name='{}'",
//     name); sql::ResultSet *res = CMysqlDataBase::m_pMysql->RunSqlQuery(sql);
//     res->last();
//     size_t id = -1;
//     if (res->getRow() != 0)
//     {
//         res->first();
//         id = res->getInt("id");
//     }
//     res->close();
//     return id;
// }


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

// chatdb::user userModel::queryUser(const std::string name, const std::string
// password)
// {
//     std::string sql = fmt::format("select * from user where name = '{}' and
//     password = '{}'", name, password);

//     sql::ResultSet *res = CMysqlDataBase::m_pMysql->RunSqlQuery(sql);
//     chatdb::user user;
//     if (res->first())
//     {
//         user.setId(res->getInt("id"));
//         user.setName(res->getString("name"));
//         user.setPassword(res->getString("password"));
//         user.setState(res->getString("state"));
//     }
//     res->close();
//     return user;
// }
