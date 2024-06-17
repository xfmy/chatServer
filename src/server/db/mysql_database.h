/**
 * @file mysql_database.h
 * @brief 提供数据库操作接口,内置连接池
 * 
 */
#pragma once
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <vector>
#include "connect_pool.h"

class MysqlDataBase
{
public:
    MysqlDataBase();
    ~MysqlDataBase();

public:
    // 单例接口
    static MysqlDataBase* GetInstance();
    //static void CreateDataBase();
    //登录数据库
    virtual bool Login(int nMinPoolSize, int nMaxPoolSize);
    //登出数据库
    virtual void Logout();
    //操作数据库
    virtual bool RunSqlExec(const std::string& strsql);
    virtual sql::ResultSet* RunSqlQuery(const std::string strsql);
    virtual bool RunSqlExecTrans(const std::vector<std::string>& vecstrsql);
    //设置数据库信息
    virtual void SetDbSvrInfo(std::string url, std::string pDbName);
    //设置用户登录信息
    virtual void SetUserLogInfo(std::string pUserName, std::string pPwd);

private:
    /// @brief 连接池管理类
    std::shared_ptr<ConnectPool> connPool_;

    /// @brief mysql数据库名
    std::string strDbName_;
    /// @brief 数据库url
    std::string url_;
    /// @brief mysql登录用户名
    std::string strUserName_;
    /// @brief mysql登录密码
    std::string strPwd_;
};
