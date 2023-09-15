#include "db/groupUserModel.h"
#include "mysqlConnPool.h"
#include <fmt/core.h>

// 创建群组
bool GroupModel::createGroup(Group &group)
{
    std::string sql = fmt::format("insert into allgroup(groupname, groupdesc) values('{}', '{}')", group.getName(), group.getDesc());
    int count = mysqlConnPool::getObject()->getStatement()->executeUpdate(sql);

    if (count)
    {
        sql = fmt::format("select id from allgroup where groupname='{}'", group.getName());
        sql::ResultSet *res = mysqlConnPool::getObject()->getStatement()->executeQuery(sql);
        group.setId(res->getInt("id"));
        return true;
    }
    return false;
}

// 加入群组
void GroupModel::addGroup(int userid, int groupid, std::string role)
{
    std::string sql = fmt::format("insert into groupuser values({}, {}, '{}')", groupid, userid, role);
    mysqlConnPool::getObject()->getStatement()->executeUpdate(sql);
}

// 查询用户所在群组信息
vector<Group> GroupModel::queryGroups(int userid)
{
    /*
    1. 先根据userid在groupuser表中查询出该用户所属的群组信息
    2. 在根据群组信息，查询属于该群组的所有用户的userid，并且和user表进行多表联合查询，查出用户的详细信息
    */
    std::string sql = fmt::format("select a.id,a.groupname,a.groupdesc from allgroup a inner join \
         groupuser b on a.id = b.groupid where b.userid={}",
                                  userid);
    vector<Group> groupVec;

    const auto &statement = mysqlConnPool::getObject()->getStatement();
    sql::ResultSet *res = statement->executeQuery(sql);
    res->beforeFirst();
    while (res->next())
    {
        Group group;
        group.setId(res->getInt("id"));
        group.setName(res->getString("groupname"));
        group.setDesc(res->getString("groupdesc"));
        groupVec.push_back(group);
    }
    res->close();

    // 查询群组的用户信息
    for (Group &group : groupVec)
    {
        sql = fmt::format("select a.id,a.name,a.state,b.grouprole from user a \
            inner join groupuser b on b.userid = a.id where b.groupid=%d",
                          group.getId());
        res = statement->executeQuery(sql);
        res->beforeFirst();
        while (res->next())
        {
            GroupUser user;
            user.setId(res->getInt("id"));
            user.setName(res->getString("name"));
            user.setState(res->getString("state"));
            user.setRole(res->getString("grouprole"));
            group.getUsers().push_back(user);
        }
    }
    res->close();
    return groupVec;
}

// 根据指定的groupid查询群组用户id列表，除userid自己，主要用户群聊业务给群组其它成员群发消息
vector<int> GroupModel::queryGroupUsers(int userid, int groupid)
{
    std::string sql = fmt::format("select userid from groupuser where groupid = {} and userid != {}", groupid, userid);
    vector<int> idVec;
    sql::ResultSet *res = mysqlConnPool::getObject()->getStatement()->executeQuery(sql);
    res->beforeFirst();
    while (res->next())
    {
        idVec.push_back(res->getInt("userid"));
    }
    res->close();
    return idVec;
}