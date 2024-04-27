#include <iostream>
#include "db/ConnectPool.h"
#include "mysql_driver.h"

CConnectPool *CConnectPool::pConnectPool = nullptr;

CConnectPool::CConnectPool() {}

CConnectPool::~CConnectPool() {}

sql::Connection *CConnectPool::CreateConnection()
{
    sql::Connection *ptrConn = nullptr;
    try
    {
        //建立连接
        ptrConn =
            driver->connect(this->strUrl.c_str(), this->strUserName.c_str(),
                            this->strPwd.c_str());
        return ptrConn;
    }
    catch (sql::SQLException &e)
    {
        std::cout << "创建mysql连接出错:" << e.what() << std::endl;
        return nullptr;
    }
    catch (std::runtime_error &e)
    {
        std::cout << "运行时出错:" << e.what() << std::endl;
        return nullptr;
    }
}

CConnectPool *CConnectPool::GetInstance(std::string pUrl, std::string pDB,
                                        std::string pUserName, std::string pPwd) {
    if (nullptr == pConnectPool)
    {
        pConnectPool = new CConnectPool(pUrl, pDB, pUserName, pPwd);
    }
    return pConnectPool;
}

void CConnectPool::SetPoolSize(int nMinSize, int nMaxSize)
{
    m_nMaxSize = nMaxSize;
    m_nMinSize = nMinSize;
}

bool CConnectPool::InitConnection()
{
    sql::Connection *conn = nullptr;
    std::lock_guard<std::mutex> lock(m_mtx);
    size_t nSum = 0;
    size_t nCount = m_nMaxSize > m_nMinSize ? m_nMaxSize : m_nMinSize;
    for (size_t i = 0; i < nCount; ++i)
    {
        conn = this->CreateConnection();
        if (conn)
        {
            m_listConn.push_back(conn);
            nSum++;
        }
    }
    if (nSum < nCount)
    {
        return false;
    }
    return true;
}

sql::Connection *CConnectPool::GetConnection()
{
    sql::Connection *con = nullptr;
    std::lock_guard<std::mutex> lock(m_mtx);
    if (m_listConn.size() == 0)
    {
        return this->CreateConnection();
    }
    con = m_listConn.front();
    m_listConn.pop_front();

    if (!con->isClosed() && con->isValid())
    {
        return con;
    }
    else
    {
        return this->CreateConnection();
    }
}

void CConnectPool::ReleaseConnection(sql::Connection *conn)
{
    std::unique_lock<std::mutex> lock(m_mtx);
    if (conn)
    {
        m_listConn.push_back(conn);
        lock.unlock();
        m_variable.notify_one();
    }
}

void CConnectPool::DestroyPool()
{
    std::unique_lock<std::mutex> lock(m_mtx);
    for (auto it = m_listConn.begin(); it != m_listConn.end(); ++it)
    {
        //销毁连接池中的连接
        this->DestroyConnection(*it);
    }
    //清空连接池中的连接
    m_listConn.clear();
}

void CConnectPool::DestroyConnection(sql::Connection *conn)
{
    if (conn)
    {
        try
        {
            conn->close();
        }
        catch (sql::SQLException &e)
        {
            std::cout << "销毁一个连接错误:" << e.what() << std::endl;
        }
        catch (std::exception &e)
        {
            std::cout << e.what() << std::endl;
        }
        delete conn;
        conn = nullptr;
    }
}

CConnectPool::CConnectPool(std::string pUrl, std::string pDB,
                           std::string pUserName, std::string pPwd) {
    this->strUrl = pUrl;
    this->strDB = pDB;
    this->strUserName = pUserName;
    this->strPwd = pPwd;
    try
    {
        //获取mysql数据库驱动
        this->driver = sql::mysql::get_driver_instance();
    }
    catch (sql::SQLException &e)
    {
        std::cout << "mysql驱动连接出错:" << e.what() << std::endl;
        //可由自己的日志模块进行输出
    }
    catch (std::runtime_error &e)
    {
        std::cout << "mysql运行出错:" << e.what() << std::endl;
    }

    if (!this->InitConnection())
    {
        std::cout << "初始化连接池出错" << std::endl;
    }
}
