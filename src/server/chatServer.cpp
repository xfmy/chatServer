#include <thread>
#include <functional>
#include <libconfig.h++>
#include "chatServer.h"
#include "db/MysqlDataBase.h"

using namespace libconfig;

chatServer::chatServer(int port) : netWork(port)
{
}

void chatServer::init()
{
    // 设置workLOOP数量以及相应的回调函数
    netWork.setThreadNum(std::thread::hardware_concurrency());
    netWork.setBusinessMessageCallback(std::bind(&chatBusiness::distribute, &this->business, _1, _2, _3));
    netWork.setConnectCallback(std::bind(&chatBusiness::onConnectCallback, &this->business, _1));
    netWork.setWriteCompleteCallback(std::bind(&chatBusiness::onConnectCallback, &this->business, _1));

    //初始化数据库
    initDB();
}

void chatServer::start()
{
    netWork.start();
}

void chatServer::initDB() 
{
    Config cfg;
    /*
      解析配置文件。
    */
    try {
        cfg.readFile("../conf/database.conf");
    } catch (const FileIOException &fioex) {
        LOG_FATAL << "I/O error while reading file.";
        exit(-1);
    } catch (const ParseException &pex) {

        LOG_FATAL << "Parse error at " << pex.getFile() << ":" << pex.getLine()
                  << " - " << pex.getError();
        exit(-1);
    }

    // 解析mysql配置文件
    try {
        std::string host = cfg.lookup("database.host");
        std::string userName = cfg.lookup("database.userName");
        std::string password = cfg.lookup("database.password");
        std::string databaseName = cfg.lookup("database.databaseName");

        CMysqlDataBase::CreateDataBase();
        CMysqlDataBase::m_pMysql->SetUserLogInfo(userName, password);
        CMysqlDataBase::m_pMysql->SetDbSvrInfo(host, databaseName);
        CMysqlDataBase::m_pMysql->Login(1,std::thread::hardware_concurrency()*2);

    } catch (const SettingNotFoundException &nfex) {
        LOG_FATAL << nfex.what();
        exit(-1);
    }
}
