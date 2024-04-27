//TODO:考虑引入连接池
#pragma once

#include "model/userModel.h"
#include <cppconn/driver.h>
#include <mysql_connection.h>
#include <mysql_driver.h>
#include <cppconn/statement.h>
#include <memory>
#include <queue>
using namespace sql;



// class chatdb
// {
// public:
//     static chatdb *getObject() { return obj; }
//     std::unique_ptr<sql::Statement> getStatement();
//     void init(); // 初始化数据库

// private:
//     class singleton{
//     public:
//         singleton(){
//             obj = new chatdb;
//         }
//         ~singleton(){
//             delete obj;
//             obj = nullptr;
//         }
//     };

//     chatdb();
//     ~chatdb();

//     static chatdb *obj;
//     static singleton SINGLETON;

//     userModel cmd;
//     Driver *driver;
//     Connection *conn;
// };

constexpr int STATEMENT_COUNT_THRESHOLD = 10;//流对象阙值
using DeleterStatementFun = std::function<void(sql::Statement *sql)>;

class mysqlConnPool
{
public:
    static mysqlConnPool *getObject() { return obj; }
    std::unique_ptr<sql::Statement, DeleterStatementFun> getStatement();
    void init();

private:
    class singleton
    {
    public:
        singleton()
        {
            obj = new mysqlConnPool;
        }
        ~singleton()
        {
            delete obj;
            obj = nullptr;
        }
    };

    mysqlConnPool();
    ~mysqlConnPool();

    std::queue<Statement*> statementQueue;
    Connection *conn;
    static mysqlConnPool *obj;
    static singleton SINGLETON;
};