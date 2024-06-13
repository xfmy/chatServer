#include <thread>
#include <functional>
#include <libconfig.h++>
#include <muduo/base/Logging.h>
#include "chatServer.h"
#include "db/mysql_database.h"

using namespace libconfig;

ChatServer::ChatServer(int port) : netWork(port) {}

void ChatServer::init()
{
    // 设置workLOOP数量以及相应的回调函数
    netWork.setThreadNum(std::thread::hardware_concurrency());
    netWork.setBusinessMessageCallback(
        std::bind(&ChatSession::distribute, &this->session, _1, _2, _3));

    //初始化数据库
    initDB();
}

void ChatServer::start() { netWork.start(); }

void ChatServer::initDB()
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
                  << " - " << pex.getError();
        exit(-1);
    }

    // 解析mysql配置文件
    try
    {
        std::string host = cfg.lookup("database.host");
        std::string userName = cfg.lookup("database.userName");
        std::string password = cfg.lookup("database.password");
        std::string databaseName = cfg.lookup("database.databaseName");

        MysqlDataBase::GetInstance()->SetUserLogInfo(userName, password);
        MysqlDataBase::GetInstance()->SetDbSvrInfo(host, databaseName);
        MysqlDataBase::GetInstance()->Login(
            1, std::thread::hardware_concurrency() * 2);
    }
    catch (const SettingNotFoundException &nfex)
    {
        LOG_FATAL << nfex.what();
        exit(-1);
    }
}
