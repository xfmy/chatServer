#include <muduo/base/Logging.h>
#include "mysql_database.h"
// MysqlDataBase* MysqlDataBase::instance = nullptr;

MysqlDataBase::MysqlDataBase() {}

MysqlDataBase::~MysqlDataBase() {}
MysqlDataBase* MysqlDataBase::GetInstance()
{
    static MysqlDataBase* instance = new MysqlDataBase();
    // if (nullptr == instance)
    // {
    //     instance = new MysqlDataBase();
    // }
    return instance;
}

bool MysqlDataBase::Login(int nMinPoolSize, int nMaxPoolSize)
{
    // std::stringstream ss;
    // ss << "tcp://" << m_strHostIp << ":" << m_nPort << "/" << m_strDbName;

    std::string strUrl = url_ + "/" + strDbName_;

    connPool_ =
        ConnectPool::GetInstance(strUrl, strDbName_, strUserName_, strPwd_);
    if (!connPool_)
    {
        LOG_ERROR << "获取连接池实例失败";
        return false;
    }

    connPool_->SetPoolSize(nMinPoolSize, nMaxPoolSize);

    bool bResult = connPool_->InitConnection();
    if (bResult)
    {
        //初始化成功
        //启动消息队列
    }
    else
        return false;
    return bResult;
}
void MysqlDataBase::Logout() { connPool_->DestroyPool(); }

bool MysqlDataBase::RunSqlExec(const std::string& strsql)
{
    std::shared_ptr<sql::Connection> pConn = connPool_->GetConnection();
    if (!pConn)
    {
        LOG_ERROR << "运行时获取连接失败" ;
        return false;
    }
    sql::Statement* state = pConn->createStatement();
    // true：sql命令的提交（commit）由驱动程序负责
    // false：sql命令的提交由应用程序负责，程序必须调用commit或者rollback方法
    pConn->setAutoCommit(false);
    try
    {
        state->execute("set character set gbk;");
        /// ddl存储过程中有返回是否成功的结果集，不能用 executupdate
        state->execute(strsql.c_str());
        do
        {
            state->getResultSet();
        } while (state->getMoreResults());

        pConn->commit();
        pConn->setAutoCommit(true);
        state->close();
        delete state;
    }
    catch (sql::SQLException& e)
    {
        delete state;
        connPool_->ReleaseConnection(pConn);

        LOG_ERROR << "Exec执行失败" << e.what();
        return false;
    }
    catch (const std::exception& e)
    {
        delete state;
        connPool_->ReleaseConnection(pConn);
        LOG_ERROR << "Exec执行失败" <<  e.what();
        return false;
    }
    connPool_->ReleaseConnection(pConn);
    return true;
}

sql::ResultSet* MysqlDataBase::RunSqlQuery(const std::string strsql)
{
    std::shared_ptr<sql::Connection> pConn = connPool_->GetConnection();
    if (!pConn)
    {
        LOG_ERROR << "获取连接失败";
        return nullptr;
    }

    sql::Statement* state = pConn->createStatement();
    sql::ResultSet* result = nullptr;

    try
    {
        result = state->executeQuery(strsql.c_str()); //用来查询数据库信息

        state->close();
        delete state;
    }
    catch (sql::SQLException& e)
    {
        delete state;
        connPool_->ReleaseConnection(pConn);
        LOG_ERROR << "查询数据库失败:" << e.what();
        return nullptr;
    }
    catch (const std::exception& e)
    {
        delete state;
        connPool_->ReleaseConnection(pConn);
        LOG_ERROR << "查询数据库失败:" << e.what();
        return nullptr;
    }

    connPool_->ReleaseConnection(pConn);
    return result;
}

bool MysqlDataBase::RunSqlExecTrans(const std::vector<std::string>& vecstrsql)
{
    std::shared_ptr<sql::Connection> pConn = connPool_->GetConnection();
    if (!pConn)
    {
        LOG_ERROR << "获取数据库连接失败";
        return false;
    }
    sql::Statement* state = pConn->createStatement();
    pConn->setAutoCommit(false);
    state->execute("set character set gbk;");
    for (const auto& str : vecstrsql)
    {
        try
        {
            state->execute(str.c_str());
            do
            {
                state->getResultSet();
            } while (state->getMoreResults());
        }
        catch (sql::SQLException& e)
        {
            LOG_ERROR << "ExecTrans执行失败:" << e.what();
        }
        catch (const std::exception& e)
        {
            LOG_ERROR << "ExecTrans执行失败:" << e.what();
        }
    }
    pConn->commit();
    pConn->setAutoCommit(true);
    state->close();
    delete state;
    connPool_->ReleaseConnection(pConn);
    return true;
}
void MysqlDataBase::SetDbSvrInfo(std::string url_, std::string pDbName)
{
    url_ = url_;
    strDbName_ = pDbName;
}
void MysqlDataBase::SetUserLogInfo(std::string pUserName, std::string pPwd)
{
    strUserName_ = pUserName;
    strPwd_ = pPwd;
}
