#include "db/MysqlDataBase.h"
#include <sstream>

CMysqlDataBase* CMysqlDataBase::m_pMysql = nullptr;

CMysqlDataBase::CMysqlDataBase() {}

CMysqlDataBase::~CMysqlDataBase() {}
void CMysqlDataBase::CreateDataBase()
{
    if (nullptr == m_pMysql)
    {
        m_pMysql = new CMysqlDataBase();
    }
}

bool CMysqlDataBase::Login(int nMinPoolSize, int nMaxPoolSize)
{
    //std::stringstream ss;
    //ss << "tcp://" << m_strHostIp << ":" << m_nPort << "/" << m_strDbName;

    std::string strUrl = url + "/" + m_strDbName;

    m_pConnPool =
        CConnectPool::GetInstance(strUrl.c_str(), m_strDbName.c_str(),
                                  m_strUserName.c_str(), m_strPwd.c_str());
    if (!m_pConnPool)
    {
        std::cout << "获取连接池实例失败" << std::endl;
        return false;
    }

    m_pConnPool->SetPoolSize(nMinPoolSize, nMaxPoolSize);

    bool bResult = m_pConnPool->InitConnection();
    if (bResult)
    {
        //初始化成功
        //启动消息队列
    }
    else
        return false;
    return bResult;
}
void CMysqlDataBase::Logout() { m_pConnPool->DestroyPool(); }

bool CMysqlDataBase::RunSqlExec(const std::string& strsql)
{
    sql::Connection* pConn = m_pConnPool->GetConnection();
    if (!pConn)
    {
        std::cout << "运行时获取连接失败" << std::endl;
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
        m_pConnPool->ReleaseConnection(pConn);
        std::cout << "Exec执行失败" << std::endl;
        return false;
    }
    catch (const std::exception&)
    {
        delete state;
        m_pConnPool->ReleaseConnection(pConn);
        std::cout << "Exec执行失败" << std::endl;
        return false;
    }
    m_pConnPool->ReleaseConnection(pConn);
    return true;
}

sql::ResultSet* CMysqlDataBase::RunSqlQuery(const std::string strsql)
{
    sql::Connection* pConn = m_pConnPool->GetConnection();
    if (!pConn)
    {
        std::cout << "获取连接失败" << std::endl;
        return nullptr;
    }

    sql::Statement* state = pConn->createStatement();
    sql::ResultSet* result = nullptr;

    try
    {
        state->execute("set character set gbk;");
        result = state->executeQuery(strsql.c_str()); //用来查询数据库信息
        do
        {
            state->getResultSet();
        } while (state->getMoreResults());
        state->close();
        delete state;
    }
    catch (sql::SQLException& e)
    {
        delete state;
        m_pConnPool->ReleaseConnection(pConn);
        std::cout << "查询数据库失败:" << e.what() << std::endl;
        return nullptr;
    }
    catch (const std::exception& e)
    {
        delete state;
        m_pConnPool->ReleaseConnection(pConn);
        std::cout << "查询数据库失败:" << e.what() << std::endl;
        return nullptr;
    }

    m_pConnPool->ReleaseConnection(pConn);
    return result;
}

bool CMysqlDataBase::RunSqlExecTrans(const std::vector<std::string>& vecstrsql)
{
    sql::Connection* pConn = m_pConnPool->GetConnection();
    if (!pConn)
    {
        std::cout << "获取数据库连接失败" << std::endl;
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
            std::cout << "ExecTrans执行失败:" << e.what() << std::endl;
        }
        catch (const std::exception& e)
        {
            std::cout << "ExecTrans执行失败:" << e.what() << std::endl;
        }
    }
    pConn->commit();
    pConn->setAutoCommit(true);
    state->close();
    delete state;
    m_pConnPool->ReleaseConnection(pConn);
    return true;
}
void CMysqlDataBase::SetDbSvrInfo(std::string url_,std::string pDbName) 
{
    // m_strHostIp = pHost;
    // m_nPort = nHostPort;
    url = url_;
    m_strDbName = pDbName;
}
void CMysqlDataBase::SetUserLogInfo(std::string pUserName, std::string pPwd) {
    m_strUserName = pUserName;
    m_strPwd = pPwd;
}
