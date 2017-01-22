
#pragma once

#include "basetype.h"
#include <string.h>

namespace SQLITE{

    typedef UTILITY::BYTE*  PBLOB;
    typedef UTILITY::BYTE   BLOB;
    typedef UTILITY::CHAR*  TEXT;
    typedef UTILITY::INT    INTEGER;
    typedef UTILITY::DOUBLE REAL;

    template<class T>
    class CSqlite3Value {
        public:
            CSqlite3Value(T data):m_Data(data){}
            virtual~CSqlite3Value(){}
            virtual UTILITY::SIZE_T GetSize() const = 0
            {
                return sizeof(T);
            }
            const T& GetData() const
            {
                return m_Data;
            }
        
        protected:
            T m_Data;
    };

    class CSqlite3Blob : public CSqlite3Value<PBLOB> {
        public:
            CSqlite3Blob(const PBLOB data, UTILITY::SIZE_T size)
                : CSqlite3Value(data), m_Size(size){}

            virtual ~CSqlite3Blob(){}
            UTILITY::SIZE_T GetSize() const
            {
                return m_Size;
            }
        private:
            UTILITY::SIZE_T m_Size;
    };

    class CSqlite3Text : public CSqlite3Value<TEXT> {
        public:
            explicit CSqlite3Text(const TEXT data) : CSqlite3Value(data), m_Length(strlen(data)){};
            virtual ~CSqlite3Text(){}
            UTILITY::SIZE_T GetSize() const
            {
                return m_Length;
            }
        private:
            UTILITY::SIZE_T m_Length;
    };

    class CSqlite3Integer : public CSqlite3Value<INTEGER> {
        public:
            explicit CSqlite3Integer(INTEGER data):CSqlite3Value(data){}
            virtual ~CSqlite3Integer(){}

            UTILITY::SIZE_T GetSize() const
            {
                return CSqlite3Value::GetSize();
            }
    };

    class CSqlite3Real : public CSqlite3Value<REAL> {
    public:
        explicit CSqlite3Real(REAL data) : CSqlite3Value(data){}
        virtual ~CSqlite3Real(){}
        UTILITY::SIZE_T GetSize() const
        {
            return CSqlite3Value::GetSize();
        }
    };
    class CSqlite3Helper{
        class CSqlite3Wrapper;

    public:
        class CSqlite3Token;

        enum OpenMode{
            openCreate = 0x01, /* create to read and write */
            openRead = 0x02, /* read only */
            openRW = 0x04  /* read and write, create DB does not exists */
        };

        enum ShareMode{
             sharedNoMutex,
             sharedFullMutex,
             sharedCache,
        };
        
    public:
        CSqlite3Helper();
        virtual ~CSqlite3Helper();
        UTILITY::BOOL Open(const UTILITY::CHAR *pPath, OpenMode openMode, ShareMode eSharedMode = sharedNoMutex);
        UTILITY::BOOL OpenMemoryDB();
        UTILITY::VOID CloseDB();

        CSqlite3Token* BeginInsertRecord(const UTILITY::CHAR *tabName, UTILITY::SIZE_T size, const UTILITY::CHAR* pRecords[]);
        CSqlite3Token* BeginUpdateRecord(const UTILITY::CHAR *tabName, UTILITY::SIZE_T size, const UTILITY::CHAR* pRecords[]);
        UTILITY::BOOL  End(CSqlite3Token* pToken);

        UTILITY::BOOL Bind(CSqlite3Token& pToken, UTILITY::INT32 index, const CSqlite3Integer &data);
        UTILITY::BOOL Bind(CSqlite3Token& pToken, UTILITY::INT32 index, const CSqlite3Real &data);
        UTILITY::BOOL Bind(CSqlite3Token& pToken, UTILITY::INT32 index, const CSqlite3Blob &data);
        UTILITY::BOOL Bind(CSqlite3Token& pToken, UTILITY::INT32 index, const CSqlite3Text &data);

        UTILITY::BOOL Reset(CSqlite3Token& pToken);
        UTILITY::BOOL Step(CSqlite3Token& pToken);

        UTILITY::BOOL  ExcuteSql(const UTILITY::CHAR *pSqlStatement);
    private:
        CSqlite3Helper(const CSqlite3Wrapper&);
        CSqlite3Helper& operator= (const CSqlite3Wrapper&);

    private:
        CSqlite3Wrapper *m_pWrapper;
    };
}

