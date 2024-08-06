/*******************************************************************************
File      : function.hpp
By        : Arun Lal K M 
Date      : 18th Jun 2019 
*******************************************************************************/

/*include*/
#include "stdtime.h"
#include "stdtime_mms_utctime.h"

/*#define*/
#define _ASSERT_(x) assert(x)
#define DllExport __declspec(dllexport)

/*typedef struct*/
typedef struct
    {
    char ip[64];
    int uId;
    int keepConnected;
    }GET_IED_THREAD_DATA_STRUCT;;

/*function declarations*/
int addToDibTable (char* name, char* ip);

/*extern*/
extern STDTIME_RC MmsUtcTimeToStdTime (const STDTIME_MMS_UTC_TIME * /*I*/ pMmsUtcTime, STDTIME * /*O*/ pStdTime);
extern ST_VOID init_log_cfg (ST_VOID);
ST_VOID init_mem (ST_VOID);
