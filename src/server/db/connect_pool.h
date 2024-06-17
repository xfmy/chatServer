#pragma once
#include <cppconn/connection.h>
#include <cppconn/driver.h>
#include <condition_variable>
/**
 * @brief 连接池
 * 
 */
class ConnectPool
{
private:
    // ConnectPool(std::string pUrl, std::string pDB, std::string pUserName,
    //             std::string pPwd);
    //~ConnectPool();

public:
    //创建一个连接,返回一个Connection
    std::shared_ptr<sql::Connection> CreateConnection();
    //创建连接池实例
    static std::shared_ptr<ConnectPool> GetInstance(std::string pUrl,
                                                    std::string pDB,
                                                    std::string pUserName,
                                                    std::string pPwd);
    //设置连接池大小
    void SetPoolSize(int nMinSize, int nMaxSize);
    //初始化连接
    bool InitConnection();
    //在连接池中获得一个连接
    std::shared_ptr<sql::Connection> GetConnection();
    //回收数据库连接
    void ReleaseConnection(std::shared_ptr<sql::Connection> conn);
    //销毁连接池,首先要销毁连接池中的连接
    void DestroyPool();
    //销毁一个连接
    void DestroyConnection(std::shared_ptr<sql::Connection> conn);

private:
    //连接池的构造函数
    ConnectPool(std::string pUrl, std::string pDB, std::string pUserName,
                 std::string pPwd);

private:
    /// @brief 连接池最小/最大连接数量
    int minSize_;
    int maxSize_;

    /// @brief  数据库URL
    std::string strUrl_;
    /// @brief 数据库登录用户名
    std::string strUserName_;
    /// @brief 数据库登录密码
    std::string strPwd_;
    /// @brief 指定数据库名
    std::string strDB_;

    /// @brief 连接池容器队列
    //std::list<sql::Connection *> listConn_;
    std::list<std::shared_ptr<sql::Connection>> listConn_;

    /// @brief 线程同步
    std::condition_variable variable_;
    std::mutex mtx_;

    // 连接池单例对象指针
    //static ConnectPool *connectPool_;
    sql::Driver *driver_;
};
