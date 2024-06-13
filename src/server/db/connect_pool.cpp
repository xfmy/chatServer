#include <muduo/base/Logging.h>
#include <fmt/core.h>
#include "connect_pool.h"
#include "mysql_driver.h"

// ConnectPool *ConnectPool::connectPool_ = nullptr;

// ConnectPool::ConnectPool(std::string pUrl, std::string pDB,
//                          std::string pUserName, std::string pPwd):
//                          strUrl_(pUrl),strDB_(pDB),strUserName_(pUserName),strPwd_(pPwd)
// {
// }

//ConnectPool::~ConnectPool() {}

std::shared_ptr<sql::Connection> ConnectPool::CreateConnection()
{
    try
    {
        //建立连接
        std::shared_ptr<sql::Connection> conn(
            driver_->connect(this->strUrl_.c_str(), this->strUserName_.c_str(),
                             this->strPwd_.c_str()));
        return conn;
    }
    catch (sql::SQLException &e)
    {
        LOG_ERROR << fmt::format("创建mysql连接出错", e.what());
        return nullptr;
    }
    catch (std::runtime_error &e)
    {
        LOG_ERROR << fmt::format("mysql运行时出错:{}", e.what());
        return nullptr;
    }
}

std::shared_ptr<ConnectPool> ConnectPool::GetInstance(std::string pUrl,
                                                      std::string pDB,
                                                      std::string pUserName,
                                                      std::string pPwd)
{
    // 获取单例
    static std::weak_ptr<ConnectPool> instanceWeakPtr;
    static std::mutex mutex;
    std::lock_guard<std::mutex> lock(mutex);
    auto instanceSharedPtr = instanceWeakPtr.lock();
    if (!instanceSharedPtr)
    {
        instanceSharedPtr = std::shared_ptr<ConnectPool>(
            new ConnectPool(pUrl, pDB, pUserName, pPwd));

        instanceWeakPtr = instanceSharedPtr;
    }
    return instanceSharedPtr;
}

void ConnectPool::SetPoolSize(int nMinSize, int nMaxSize)
{
    maxSize_ = nMaxSize;
    minSize_ = nMinSize;
}

bool ConnectPool::InitConnection()
{
    // sql::Connection *conn = nullptr;
    std::lock_guard<std::mutex> lock(mtx_);
    size_t nSum = 0;
    size_t nCount = maxSize_ > minSize_ ? maxSize_ : minSize_;
    for (size_t i = 0; i < nCount; ++i)
    {
        std::shared_ptr<sql::Connection> conn = this->CreateConnection();
        if (conn)
        {
            listConn_.push_back(conn);
            nSum++;
        }
    }
    if (nSum < nCount)
    {
        return false;
    }
    return true;
}

std::shared_ptr<sql::Connection> ConnectPool::GetConnection()
{
    std::lock_guard<std::mutex> lock(mtx_);
    if (listConn_.size() == 0)
    {
        return this->CreateConnection();
    }
    std::shared_ptr<sql::Connection> con(listConn_.front());
    listConn_.pop_front();

    if (!con->isClosed() && con->isValid())
    {
        return con;
    }
    else
    {
        return this->CreateConnection();
    }
}

void ConnectPool::ReleaseConnection(std::shared_ptr<sql::Connection> conn)
{
    // 用户使用连接后销毁接口
    std::unique_lock<std::mutex> lock(mtx_);
    if (conn)
    {
        listConn_.push_back(conn);
        lock.unlock();
        variable_.notify_one();
    }
}

void ConnectPool::DestroyPool()
{
    std::unique_lock<std::mutex> lock(mtx_);
    for (auto it = listConn_.begin(); it != listConn_.end(); ++it)
    {
        //销毁连接池中的连接
        this->DestroyConnection(*it);
    }
    //清空连接池中的连接
    listConn_.clear();
}

void ConnectPool::DestroyConnection(std::shared_ptr<sql::Connection> conn)
{
    if (conn)
    {
        try
        {
            conn->close();
        }
        catch (sql::SQLException &e)
        {
            LOG_ERROR << fmt::format("mysql销毁一个连接出错:{}", e.what());
        }
        catch (std::exception &e)
        {
            LOG_ERROR << e.what();
        }
        conn.reset();
    }
}

ConnectPool::ConnectPool(std::string pUrl, std::string pDB,
                         std::string pUserName, std::string pPwd)
{
    this->strUrl_ = pUrl;
    this->strDB_ = pDB;
    this->strUserName_ = pUserName;
    this->strPwd_ = pPwd;
    try
    {
        //获取mysql数据库驱动
        this->driver_ = sql::mysql::get_driver_instance();
    }
    catch (sql::SQLException &e)
    {
        LOG_ERROR << fmt::format("mysql驱动连接出错:{}", e.what());
    }
    catch (std::runtime_error &e)
    {
        LOG_ERROR << fmt::format("mysql运行出错:{}", e.what());
    }
}
