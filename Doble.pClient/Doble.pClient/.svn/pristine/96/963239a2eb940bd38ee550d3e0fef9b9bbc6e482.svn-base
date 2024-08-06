/*******************************************************************************
File      : acsi.hpp
By        : Arun Lal K M 
Date      : 18th Jun 2019 
Reference : iec61850-8-1{ed2.0}
6.2.1 Client/server services
Table 2 – Services requiring client/server Communication Profile
*******************************************************************************/
#ifndef asci_h
#define asci_h

#include <iostream> 
using namespace std;

/*include*/
#include "glbtypes.h"
#include "sysincs.h"
#include <mvl_defs.h>
#include <mvl_acse.h>
#include <scl.h>
#include <vector>
#include <map>
#include <algorithm>

/*define*/
#define DllExport __declspec(dllexport)
#define _ASSERT_(x) assert(x)
#define MAX_CONNECT_ATTEMPT 2// akshay added
#define MAX_GET_LD_TIMEOUT 5
#define MAX_GET_LN_TIMEOUT 5
#define MAX_GET_DS_TIMEOUT 5
#define MAX_GET_DATA_TIMEOUT 5
#define MAX_SET_DATA_TIMEOUT 5
#define MAX_ADD_CAUSE_TIMEOUT 30
#define MAX_SET_DATA_TIMEOUT 5

/* Macros to access each individual bit of any bitstring.               */
#define BSTR_BIT_SET_ON(ptr,bitnum) \
    ( ((ST_UINT8 *)(ptr))[(bitnum)/8] |= (0x80>>((bitnum)&7)) )
#define BSTR_BIT_SET_OFF(ptr,bitnum) \
    ( ((ST_UINT8 *)(ptr))[(bitnum)/8] &= ~(0x80>>((bitnum)&7)) )

/* BSTR_BIT_GET returns 0 if bit is clear, 1 if bit is set.     */
#define BSTR_BIT_GET(ptr,bitnum) \
    (( ((ST_UINT8 *)(ptr))[(bitnum)/8] &  (0x80>>((bitnum)&7)) ) ? 1:0)

/*extern "C" variables*/
extern "C" MVL_CFG_INFO mvlCfg;

/*typedef struct*/
typedef struct 
    {
    ST_UCHAR u8;
    ST_UINT16 u16;
    ST_UINT32 u32;
    ST_UINT64 u64;
    ST_INT8 i8;
    ST_INT16 i16;
    ST_INT32 i32;
    ST_INT64 i64;
    ST_DOUBLE d;
    ST_FLOAT f;
    MMS_UTC_TIME utc;
    char octStr[256];
    }VAL_STRUCT;

typedef struct leaf_ctrl
    {
    DBL_LNK l;

    ST_CHAR* pName;
    } STRUCT_LEAF;

typedef struct 
    {
    int count;
    STRUCT_LEAF* pListItem;
    unsigned char moreFollows;
    char continueAfter[MAX_IDENT_LEN+1];
    } STRUCT_GET_LIST;

typedef struct 
    {
    MVL_TYPE_ID typeId;
    RUNTIME_TYPE* rtPtrOut;
    ST_INT  numRtOut;
    } STRUCT_GET;

typedef struct 
    {
    ST_CHAR name [MVL61850_MAX_OBJREF_LEN+1];
    ST_CHAR DatSet [MVL61850_MAX_OBJREF_LEN+1];
    ST_CHAR GoID [MVL61850_MAX_RPTID_LEN+1];
    ST_CHAR ADDR [32]; /*DstAddress$Addr*/
    
    ST_CHAR MinTime [32];
    ST_CHAR MaxTime [32];
    ST_CHAR PRIORITY [32]; /*DstAddress$PRIORITY*/
    ST_CHAR VID [32]; /*DstAddress$VID*/
    ST_CHAR APPID [32]; /*DstAddress$APPID*/
    ST_CHAR ConfRev [32];
    } STRUCT_GCB;

typedef struct 
    {
    ST_CHAR name [MVL61850_MAX_OBJREF_LEN+1];
    ST_CHAR DatSet [MVL61850_MAX_OBJREF_LEN+1];
    ST_CHAR RptID [MVL61850_MAX_RPTID_LEN+1];

    ST_CHAR OptFlds [32];
    ST_CHAR TrgOps [32];
    ST_CHAR BufTm [32];
    ST_CHAR ConfRev [32];
    } STRUCT_RCB;

typedef struct
  {
  ST_INT cmd_term_num_va;       /* num of vars received in CommandTermination*/
  ST_CHAR oper_name [MAX_IDENT_LEN + 1];
  MVL61850_LAST_APPL_ERROR LastApplError; /* for decoded LastApplError  */
  } MY_CONTROL_INFO;

/*extern "C" functions*/
extern "C" MVL_NET_INFO *connectToServer (ST_CHAR *clientARName, ST_CHAR *serverARName);
extern "C" ST_RET disconnectFromServer (MVL_NET_INFO *clientNetInfo);
extern "C" int addToDibTable (char* name, char* ip);
extern "C" void removeFromDibTable (char* name);
extern "C" ST_RET waitReqDoneEx (MVL_REQ_PEND *req, ST_INT timeout);
extern "C" ST_RET named_var_read (MVL_NET_INFO *net_info, ST_CHAR *varName,
                                  ST_INT scope, ST_CHAR *domName,
                                  MVL_TYPE_ID type_id, ST_VOID *dataDest, ST_INT timeOut);
extern "C" ST_RET named_var_write (MVL_NET_INFO *netInfo, ST_CHAR *varName,
                                   ST_INT scope, ST_CHAR *domName,
                                   MVL_TYPE_ID type_id, ST_VOID *dataSrc, ST_INT timeOut);
extern "C" MVL_TYPE_ID  getVarRuntimeType  (int  el_tag, int  el_len);
extern "C" ST_VOID mvl_type_id_destroy (MVL_TYPE_ID TypeId);
extern "C" MVL_VMD_CTRL *config_iec_remote_vmd (
    ST_CHAR *scl_filename,
    ST_CHAR *ied_name,
    ST_CHAR *access_point_name,
    SCL_INFO *remote_scl_info);
extern "C" void utcToString (MMS_UTC_TIME *data_ptr, char* buf);
extern "C" void stringToUtc (MMS_UTC_TIME* data_ptr, char* buf);

extern "C" ST_RET namedVarReadAsync (MVL_NET_INFO *net_info, ST_CHAR *varName,
                                     ST_INT scope, ST_CHAR *domName,
                                     MVL_TYPE_ID type_id, ST_VOID *dataDest, MVL_REQ_PEND **reqCtrl, READ_REQ_INFO **req_info,
                                     MVL_READ_RESP_PARSE_INFO** parse_info);
extern "C" void pClientLog (char* text, char* caption, int code);
extern "C" ST_RET get_last_string (ST_CHAR* astr, char* dst);
extern "C" ST_VOID doCommService ();
extern "C" void string2hexString (char* input, char* output);

extern "C" void addStringToEnd (char* str1, char* str2);

extern "C" void removeStringFromEnd (char* str);

extern "C" ST_RET named_var_write_ex (MVL_NET_INFO* netInfo, ST_CHAR* varName,
                                      ST_INT scope, ST_CHAR* domName,
                                      MVL_TYPE_ID type_id, ST_VOID* dataSrc, ST_INT timeOut, int* error);
extern "C" void writeRespToString (int err, char* str);
extern "C" void addCauseToString (int err, char* str);

/*class*/
class mServer
    {
    public:
        mServer ();
        ~mServer ();
        int sGetServerDirectory (MVL_NET_INFO *pNetInfo, char* getafter, STRUCT_GET_LIST* list);
    private:
    };

class mAssociation
    {
    public:
        mAssociation ();
        ~mAssociation ();

        int addToDib (char* name, char* ip);
        int removeFromDib (char* name);
        int sAssociate (char* name, MVL_NET_INFO** pNetInfo);
        int sRelease (MVL_NET_INFO* pNetInfo);
    private:
        void sAbort (); /*TODO: Arun*/
    };

class mLogicalDevice
    {
    public:
        mLogicalDevice ();
        ~mLogicalDevice ();
        int sGetLogicalDeviceDirectory (MVL_NET_INFO *pNetInfo, char* domName, char* getafter, STRUCT_GET_LIST* list);
    private:
    };

class mLogicalNode
    {
    public:
        mLogicalNode ();
        ~mLogicalNode ();
        int deleteTypeId (STRUCT_GET* pLn);
        int sGetLogicalNodeDirectory (MVL_NET_INFO *pNetInfo, char* domName, char* lnName, STRUCT_GET* pLn);
    private:
        void sGetAllDataValues ();
    };

class mData
    {
    public:
        mData ();
        ~mData ();
        int sGetStructDataValues (MVL_NET_INFO *pNetInfo, char* domName, char* varName, char* pData, int allocSize, unsigned char* dataSrc);
        int sSetStructDataValues (MVL_NET_INFO *pNetInfo, char* domName, char* varName, char* pData);
        int sGetDataValuesEx (MVL_NET_INFO *pNetInfo, char* domName, char* varName, char* pData);
        int sGetDataValues (MVL_NET_INFO *pNetInfo, char* domName, char* varName, int type, int len, char* pData);
        int sSetDataValues (MVL_NET_INFO *pNetInfo, char* domName, char* varName, int type, int len, char* pData);
        int sSetDataValuesEx (MVL_NET_INFO* pNetInfo, char* domName, char* varName, char* pData, char* addCause);
        int sGetDataValuesAsync (MVL_NET_INFO *pNetInfo, 
            char* domName, 
            char* varName, 
            int type, 
            int len, 
            char* dataSrc, 
            MVL_REQ_PEND **reqCtrl, 
            READ_REQ_INFO **req_info,
            ST_DOUBLE* stopTime,
            MVL_READ_RESP_PARSE_INFO** parse_info);
    private:
        void sGetDataDirectory ();
        void sGetDataDefinition ();
    };

class mBRCB 
    {
    public: 
        mBRCB ();
        ~mBRCB ();
        int sGetBRCBValues (MVL_NET_INFO *pNetInfo, char* domName, char* varName, STRUCT_RCB* pGcb, unsigned char* dataSrc);
    private:
        void sSetBRCBValues ();
    };

class mURCB 
    {
    public: 
        mURCB ();
        ~mURCB ();
        int sGetURCBValues (MVL_NET_INFO *pNetInfo, char* domName, char* varName, STRUCT_RCB* pGcb, unsigned char* dataSrc);
    private:
        void sSetURCBValues ();
    };

class mGOOSE  
    {
    public: 
        mGOOSE ();
        ~mGOOSE ();
        int sGetGoCBValues (MVL_NET_INFO *pNetInfo, char* domName, char* varName, STRUCT_GCB* pGcb, unsigned char* dataSrc);
    private:
        void sSetGoCBValues ();
    };

class mDataSet 
    {
    public:
        mDataSet ();
        ~mDataSet ();
        int getDataSetNames (MVL_NET_INFO *pNetInfo, char* domName, char* getafter, STRUCT_GET_LIST* list);
        int sGetDataSetDirectory (MVL_NET_INFO *pNetInfo, char* domName, char* dsName, STRUCT_GET_LIST* list);
    private:
    };

class mLocal 
    {
    public:
        mLocal ();
        ~mLocal ();
        int getSclData (char* pSclPath, SCL_INFO* pSclInfo);
        int getVmdData (char* pSclPath, char* pIedName, char* pApName, MVL_VMD_CTRL **vmd_ctrl, SCL_INFO* pSclInfo);
    private:
    };

class mControl
    {
    public:
        mControl ();
        ~mControl ();
        int sSelect (MVL_NET_INFO *pNetInfo, char* domName, char* varName);
        int sSelectWithValue (MVL_NET_INFO *pNetInfo, 
                        char* domName, 
                        char* varNameOper, 
                        char* varNameSBOw, 
                        char* valueInStr, 
                        char* addCause,
                        char* orCat,
                        char* interlockCheck,
                        char* synchrocheck,
                        char* test,
                        int timeInSec);
        int sOperate (MVL_NET_INFO *pNetInfo, 
                        char* domName, 
                        char* varName, 
                        char* valueInStr, 
                        char* addCause,
                        char* orCat,
                        char* interlockCheck,
                        char* synchrocheck,
                        char* test,
                        int timeInSec,
                        int ctlModel);
        int sCancel ();
    private:
    };

/*functions*/
void valToString (void* dataSrc, char* pData, int type, int len);
void* stringToVal (int type, int len, char* pData, VAL_STRUCT* val);
#endif