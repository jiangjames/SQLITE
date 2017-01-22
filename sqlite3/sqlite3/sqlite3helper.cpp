#include "sqlite3helper.h"
#include "sqlite3.h"
#include <memory>
#include <string>
#include <iostream>

using namespace SQLITE;

UTILITY::BOOL IsSqliteOK(int result)
{
    return SQLITE_OK == result ? UTILITY::TRUE : UTILITY::FALSE;
}

class CSqlite3Helper::CSqlite3Token{
public:
    CSqlite3Token(sqlite3_stmt* pStmt) :
     m_pStmt(pStmt)
    {
    }

    ~CSqlite3Token()
    {
        Release();
    }

public:
    operator sqlite3_stmt*()
    {
        return m_pStmt;
    }

    UTILITY::BOOL Release()
    {
        if(m_pStmt)
            return  IsSqliteOK(sqlite3_finalize(m_pStmt));
        return UTILITY::TRUE;
    }

private:
    sqlite3_stmt *m_pStmt;
};

class CSqlite3Helper::CSqlite3Wrapper{
public:
    CSqlite3Wrapper() :
     m_pSqlite3(UTILITY_NULL)
    {
    }

    ~CSqlite3Wrapper()
    {
        if(m_pSqlite3)
        {
            sqlite3_close(m_pSqlite3);
        }
        m_pSqlite3 = UTILITY_NULL;
    }

    UTILITY::BOOL Open(const char *pPath, OpenMode openMode, ShareMode eShared)
    {
        if( m_pSqlite3 )
            return UTILITY::TRUE;

        UTILITY::INT flags = 0;
        switch (openMode)
        {
        case openCreate:
            flags |= SQLITE_OPEN_CREATE;
            break;

        case openRead:
            flags |= SQLITE_OPEN_READONLY;
            break;

        case openRW:
            flags |= SQLITE_OPEN_READWRITE|SQLITE_OPEN_CREATE;
            break;

        default:
            break;
        }

        switch (eShared)
        {
        case sharedCache:
            flags |= SQLITE_OPEN_SHAREDCACHE;
            break;

        case sharedNoMutex:
            flags |= SQLITE_OPEN_NOMUTEX;
            break;

        case sharedFullMutex:
            flags |= SQLITE_OPEN_FULLMUTEX;
            break;
        }
        m_path = pPath;
        return IsSqliteOK(sqlite3_open_v2(pPath, &m_pSqlite3, flags, UTILITY_NULL));
    }

    CSqlite3Token* Prepare(const UTILITY::CHAR *sql)
    {
        sqlite3_stmt *pStmt = UTILITY_NULL;
        if (IsSqliteOK(sqlite3_prepare_v2(m_pSqlite3, sql, -1, &pStmt, UTILITY_NULL)))
            return new CSqlite3Token(pStmt);

        return UTILITY_NULL;
    }

    UTILITY::BOOL Bind(sqlite3_stmt *pStmt, UTILITY::INT index, const UTILITY::VOID *blob, UTILITY::SIZE_T size)
    {
        return IsSqliteOK(sqlite3_bind_blob(pStmt,index, blob, size, SQLITE_STATIC));
    }

    UTILITY::BOOL Bind(sqlite3_stmt *pStmt, UTILITY::INT index, const UTILITY::CHAR *text, UTILITY::SIZE_T length)
    {
        return IsSqliteOK(sqlite3_bind_text(pStmt, index, text, length, SQLITE_STATIC));
    }

    UTILITY::BOOL Bind(sqlite3_stmt *pStmt, UTILITY::INT index, UTILITY::DOUBLE data)
    {
        return IsSqliteOK(sqlite3_bind_double(pStmt, index, data));
    }

    UTILITY::BOOL Bind(sqlite3_stmt *pStmt, UTILITY::INT index, UTILITY::INT data)
    {
        return IsSqliteOK(sqlite3_bind_int(pStmt, index, data));
    }

    UTILITY::BOOL Step(sqlite3_stmt *pStmt)
    {
        return IsSqliteOK(sqlite3_step(pStmt));
    }

    UTILITY::BOOL Reset(sqlite3_stmt *pStmt)
    {
        return IsSqliteOK(sqlite3_reset(pStmt));
    }

    UTILITY::BOOL Excute(const UTILITY::CHAR* sql)
    {
        // TODO 'callback' and 'error message'
        return IsSqliteOK(sqlite3_exec(m_pSqlite3,sql,UTILITY_NULL, UTILITY_NULL,NULL));
    }
private:
    sqlite3     *m_pSqlite3;
    std::string  m_path; 
};

CSqlite3Helper::CSqlite3Helper() :
 m_pWrapper(new CSqlite3Wrapper)
{
}

CSqlite3Helper::~CSqlite3Helper()
{
    CloseDB();
}

UTILITY::BOOL CSqlite3Helper::Open(const UTILITY::CHAR* pPath, OpenMode openMode, ShareMode eSharedMode /*= sharedNoMutex*/)
{
    UTILITY_ASSERT(pPath);
    if( !m_pWrapper )
    {
        m_pWrapper = new CSqlite3Wrapper;
        if( !m_pWrapper )
            return UTILITY::FALSE;
    }
    return m_pWrapper->Open(pPath, openMode, eSharedMode);
}

UTILITY::BOOL CSqlite3Helper::OpenMemoryDB()
{
    return UTILITY::TRUE;
}

UTILITY::VOID CSqlite3Helper::CloseDB()
{
    if( m_pWrapper )
        delete m_pWrapper;
    m_pWrapper = UTILITY_NULL;
}

CSqlite3Helper::CSqlite3Token* CSqlite3Helper::BeginInsertRecord(const UTILITY::CHAR *tabName, UTILITY::SIZE_T size, const UTILITY::CHAR* pRecords[])
{
    UTILITY_ASSERT(tabName && strlen(tabName) && size && pRecords && m_pWrapper);

    std::string sql("insert or ignore into ");
    std::string values(") values (");
    sql += tabName;
    while (size--) 
    {
        sql += *(pRecords++);
        sql += ",";
        values += "?";
    }

    values.pop_back();
    sql.pop_back();
    sql += values + ")";

    return m_pWrapper->Prepare(sql.c_str());
}

CSqlite3Helper::CSqlite3Token* CSqlite3Helper::BeginUpdateRecord(const UTILITY::CHAR *tabName, UTILITY::SIZE_T size, const UTILITY::CHAR* pRecords[])
{
    UTILITY_ASSERT(tabName && strlen(tabName) && size && pRecords);
    return UTILITY_NULL;
}

UTILITY::BOOL CSqlite3Helper::Bind(CSqlite3Token& pToken, UTILITY::INT32 index, const CSqlite3Integer &data)
{
    return m_pWrapper->Bind(pToken, index,data.GetData());
}

UTILITY::BOOL CSqlite3Helper::Bind(CSqlite3Token& pToken, UTILITY::INT32 index, const CSqlite3Real &data)
{
    return m_pWrapper->Bind(pToken, index, data.GetData());
}

UTILITY::BOOL CSqlite3Helper::Bind(CSqlite3Token& pToken, UTILITY::INT32 index, const CSqlite3Blob &data)
{
    return m_pWrapper->Bind(pToken, index, data.GetData(),data.GetSize());
}

UTILITY::BOOL CSqlite3Helper::Bind(CSqlite3Token& pToken, UTILITY::INT32 index, const CSqlite3Text &data)
{
    return m_pWrapper->Bind(pToken, index, data.GetData(), data.GetSize());
}

UTILITY::BOOL CSqlite3Helper::Reset(CSqlite3Token& pToken)
{
    return m_pWrapper->Reset(pToken);
}

UTILITY::BOOL CSqlite3Helper::Step(CSqlite3Token& pToken)
{
    return m_pWrapper->Step(pToken);
}

UTILITY::BOOL  CSqlite3Helper::End(CSqlite3Token* pToken)
{
    UTILITY::BOOL bRet = pToken->Release();
    delete pToken;
    return bRet;
}

UTILITY::BOOL  CSqlite3Helper::ExcuteSql(const UTILITY::CHAR *pSqlStatement)
{
    UTILITY_ASSERT(pSqlStatement && strlen(pSqlStatement));
    return m_pWrapper->Excute(pSqlStatement);
}
