#pragma once
#include <string>
#include <vector>
#include "group.hpp"

/**
 * @brief 维护群组信息的操作接口方法
 *
 */
class GroupModel
{
public:
    /**
     * @brief 创建群组
     * 
     * @param group 群组字段类
     * @return true 成功
     * @return false 失败
     */
    bool CreateGroup(Group &group);

    /**
     * @brief 加入群组
     * 
     * @param userid 待加入用户id
     * @param groupid 群组id
     * @param role 群成员属性
     */
    void AddGroup(int userid, int groupid, std::string role);

    /**
     * @brief 查询用户所在群组信息
     *
     * @param userid 用户id
     * @return std::vector<Group> 返回用户加入的所有的群组信息类
     */
    std::vector<Group> QueryGroups(int userid);

    /**
     * @brief
     * 根据指定的groupid查询群组用户id列表，除userid自己，主要用户群聊业务给群组其它成员群发消息
     *
     * @param userid 用户id
     * @param groupid 群id
     * @return std::vector<int> 群员列表
     */
    std::vector<int> QueryGroupUsers(int userid, int groupid);
};