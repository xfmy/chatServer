#include "mysqlConnPool.h"
#include <libconfig.h++>
#include <iostream>
#include <thread>
#include <muduo/base/Logging.h>
using namespace libconfig;

mysqlConnPool *mysqlConnPool::obj = nullptr;
mysqlConnPool::singleton mysqlConnPool::SINGLETON;

// chatdb *chatdb::obj = nullptr;
// chatdb::singleton chatdb::SINGLETON;



// chatdb::chatdb()
// {
//     driver = nullptr;
//     conn = nullptr;
// }

// chatdb::~chatdb()
// {
//     if(conn) delete conn;
//     conn = nullptr;
//     driver = nullptr;
// }

// std::unique_ptr<sql::Statement> chatdb::getStatement()
// {
//     return std::make_unique<sql::Statement>(conn->createStatement());
// }

// void chatdb::init()
// {
//     Config cfg;

//     /*
//       解析配置文件。
//     */
//     try
//     {
//         cfg.readFile("database.conf");
//     }
//     catch (const FileIOException &fioex)
//     {
//         std::cerr << "I/O error while reading file." << std::endl;
//         exit(-1);
//     }
//     catch (const ParseException &pex)
//     {
//         std::cerr << "Parse error at " << pex.getFile() << ":" << pex.getLine()
//                   << " - " << pex.getError() << std::endl;
//         exit(-1);
//     }
    
//     //解析mysql配置文件
//     try
//     {
//         std::string host = cfg.lookup("log.host");
//         std::string userName = cfg.lookup("log.userName");
//         std::string password = cfg.lookup("log.password");
//         std::string databaseName = cfg.lookup("log.databaseName");

//         driver = get_driver_instance();
//         assert(driver != NULL);
//         conn = driver->connect(host, userName, password);
//         assert(conn != NULL);
//         conn->setSchema(databaseName);//选择数据库
//     }
//     catch (const SettingNotFoundException &nfex)
//     {
//         std::cerr << "No 'version' setting in configuration file." << std::endl;
//         exit(-1);
//     }
// }















std::unique_ptr<sql::Statement, DeleterStatementFun> mysqlConnPool::getStatement()
{
    sql::Statement* ret;
    //如果队列有足够的对象就直接取出
    if(!this->statementQueue.empty()){
        ret = this->statementQueue.front();
        this->statementQueue.pop();
    }
    else
    {
        //如果数量还未大道阙值,则可以创建,否则需要等待
        if (this->statementQueue.size() < STATEMENT_COUNT_THRESHOLD){
            this->statementQueue.emplace(conn->createStatement());
        }else{
            std::this_thread::yield();
        }
        return getStatement(); // 迭代
    }

    return std::unique_ptr<sql::Statement, DeleterStatementFun>(ret, [this](sql::Statement *sql) -> void
                                                                { this->statementQueue.push(sql);
                                                                sql = nullptr; });
}

void mysqlConnPool::init()
{
    Config cfg;
    /*
      解析配置文件。
    */
    try
    {
        cfg.readFile("../conf/database.conf");
    }
    catch (const FileIOException &fioex)
    {
        LOG_FATAL << "I/O error while reading file.";
        exit(-1);
    }
    catch (const ParseException &pex)
    {
        LOG_FATAL << "Parse error at " << pex.getFile() << ":" << pex.getLine()
                  << " - " << pex.getError() ;
        exit(-1);
    }

    // 解析mysql配置文件
    try
    {
        std::string host = cfg.lookup("database.host");
        std::string userName = cfg.lookup("database.userName");
        std::string password = cfg.lookup("database.password");
        std::string databaseName = cfg.lookup("database.databaseName");

        Driver *driver = get_driver_instance();
        assert(driver != NULL);
        conn = driver->connect(host, userName, password);
        assert(conn != NULL);
        conn->setSchema(databaseName); // 选择数据库
    }
    catch (const SettingNotFoundException &nfex)
    {
        LOG_FATAL << nfex.what();
        exit(-1);
    }
}

mysqlConnPool::mysqlConnPool()
{
    conn = nullptr;
}

mysqlConnPool::~mysqlConnPool()
{
    // 释放全部的Statement
    while(!statementQueue.empty()){
        sql::Statement* temp = statementQueue.front();
        statementQueue.pop();
        temp->close();
    }

    if (conn)
        conn->close();
    conn = nullptr;
}
