#include <fmt/core.h>
#include "group_user_model.h"
#include "mysql_database.h"

// 创建群组
bool GroupModel::CreateGroup(Group &group)
{
    std::string sql = fmt::format("insert into allgroup(groupname, groupdesc) values('{}', '{}')", group.GetName(), group.GetDesc());
    int count = MysqlDataBase::GetInstance()->RunSqlExec(sql);

    if (count)
    {
        sql = fmt::format("select id from allgroup where groupname='{}'", group.GetName());
        sql::ResultSet *res = MysqlDataBase::GetInstance()->RunSqlQuery(sql);
        group.SetId(res->getInt("id"));
        return true;
    }
    return false;
}

// 加入群组
void GroupModel::AddGroup(int userid, int groupid, std::string role)
{
    std::string sql = fmt::format("insert into groupuser values({}, {}, '{}')", groupid, userid, role);
    MysqlDataBase::GetInstance()->RunSqlExec(sql);
}

// 查询用户所在群组信息
std::vector<Group> GroupModel::QueryGroups(int userid)
{
    /*
    1. 先根据userid在groupuser表中查询出该用户所属的群组信息
    2. 在根据群组信息，查询属于该群组的所有用户的userid，并且和user表进行多表联合查询，查出用户的详细信息
    */
    std::string sql = fmt::format("select a.id,a.groupname,a.groupdesc from allgroup a inner join \
         groupuser b on a.id = b.groupid where b.userid={}",
                                  userid);
    std::vector<Group> groupVec;

    //const auto &statement = mysqlConnPool::getObject()->getStatement();
    sql::ResultSet *res = MysqlDataBase::GetInstance()->RunSqlQuery(sql);
    res->beforeFirst();
    // 查出userid所有的群组信息
    while (res->next())
    {
        Group group;
        group.SetId(res->getInt("id"));
        group.SetName(res->getString("groupname"));
        group.SetDesc(res->getString("groupdesc"));
        groupVec.push_back(group);
    }
    res->close();

    // 查询群组的用户信息
    for (Group &group : groupVec)
    {
        sql = fmt::format("select a.id,a.name,a.state,b.grouprole from user a \
            inner join groupuser b on b.userid = a.id where b.groupid={}",
                          group.GetId());
        res = MysqlDataBase::GetInstance()->RunSqlQuery(sql);
        res->beforeFirst();
        while (res->next())
        {
            GroupUser user;
            user.SetId(res->getInt("id"));
            user.SetName(res->getString("name"));
            user.SetState(res->getString("state"));
            user.SetRole(res->getString("grouprole"));
            group.GetUsers().push_back(user);
        }
        res->close();
    }
    
    return groupVec;
}

// 根据指定的groupid查询群组用户id列表，除userid自己，主要用户群聊业务给群组其它成员群发消息
std::vector<int> GroupModel::QueryGroupUsers(int userid, int groupid)
{
    std::string sql = fmt::format("select userid from groupuser where groupid = {} and userid != {}", groupid, userid);
    std::vector<int> idVec;
    sql::ResultSet *res = MysqlDataBase::GetInstance()->RunSqlQuery(sql);
    res->beforeFirst();
    while (res->next())
    {
        idVec.push_back(res->getInt("userid"));
    }
    res->close();
    return idVec;
}