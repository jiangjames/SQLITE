#pragma once

#include <assert.h>

namespace UTILITY {
    typedef char                CHAR;
    typedef unsigned char       BYTE;

    typedef char                INT8;
    typedef unsigned char       UINT8;

    typedef int                 INT;
    typedef unsigned int        UINT;

    typedef short               INT16;
    typedef unsigned short      UINT16;

    typedef long                INT32;
    typedef unsigned long       UINT32;

    typedef long long           INT64;
    typedef unsigned long long  UINT64;

    typedef void                VOID;

    typedef float               FLOAT;
    typedef double              DOUBLE;

    typedef UINT32              EVENT_ID;
    typedef UINT32              STATE_ID;

    typedef INT32               RESULT;

    typedef UINT32              SIZE_T;


    enum BOOL {
        FALSE = 0,
        TRUE
    };

    const class nullptr_t_t {  
        public:
            template<class T>           operator T* () const {return 0;}
            template<class C, class T>  operator T C::*() const { return 0; }
        private:
            void operator& () const;
    } nullptr_t = {};

    const UINT32 INVALID_ID = 0xFFFFFFFF;
}


#define UTILITY_NULL      UTILITY::nullptr_t
#define UTILITY_ASSERT(x) assert( (x) )