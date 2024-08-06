/*******************************************************************************
File      : ied.hpp
By        : Arun Lal K M 
Date      : 18th Jun 2019 
Reference : iec61850-8-1{ed2.0}
6.2.1 Client/server services
Table 2 – Services requiring client/server Communication Profile
*******************************************************************************/
#ifndef ied_h
#define ied_h

/*include*/
#include "glbtypes.h"
#include "sysincs.h"
#include <mvl_defs.h>
#include <mvl_acse.h>
#include <iostream>

#include "acsi.hpp"
#include "xml.hpp"
#include "pipe.hpp"

/*#define*/
#define NODE_IED "IED"
#define NODE_LD "LD"
#define NODE_LN "LN"
#define NODE_FC "FC"
#define NODE_DO "DO"
#define NODE_DA "DA"
#define NODE_BDA "BDA"
#define NODE_DATASET "DATASET"
#define NODE_DS "DS"
#define NODE_LEAF "LEAF"
#define NODE_CB "CBs"
#define NODE_URCB "URCB"
#define NODE_BRCB "BRCB"
#define NODE_RGCB "RGCB"
#define NODE_GOCB "GOCB"
#define NODE_BR "BR"
#define NODE_RP "RP"
#define NODE_GO "GO"
#define NODE_RG "RG"

#define NODE_GOID "GOID"
#define NODE_ADDR "MAC_IP"
#define NODE_MINTIME "MINTIME"
#define NODE_MAXTIME "MAXTIME"
#define NODE_PRIORITY "PRIORITY"
#define NODE_VID "VID"
#define NODE_APPID "APPID"
#define NODE_CONFREV "CONFREV"
#define NODE_RPTID "RPTID"
#define NODE_TRGOPS "TRGOPS"
#define NODE_BUFTIME "BUFTIME"
#define NODE_OPTFLDS "OPTFLDS"

#define MAX_PERCENTAGE 80

#define DUMMY_IP "0.0.0.0"
#define DllExport __declspec(dllexport)
#define MIN_SLEEP_TIME_IN_MS 1
#define MIN_COM_SLEEP_TIME_IN_MS 10
#define MIN_PIPE_SLEEP_TIME_IN_MS 10 

#define MIN_POLL_REQ_SLEEP_TIME_IN_MS 10
#define MIN_POLL_RES_SLEEP_TIME_IN_MS 10

#define MAX_SLEEP_TIME_IN_MS 1000

#define _ASSERT_(x) assert(x)
#define MIN_SLEEP Sleep(MIN_SLEEP_TIME_IN_MS)
#define MIN_COM_SLEEP Sleep(MIN_COM_SLEEP_TIME_IN_MS)
#define MIN_PIPE_SLEEP Sleep(MIN_PIPE_SLEEP_TIME_IN_MS)

#define MIN_POLL_REQ_SLEEP Sleep(MIN_POLL_REQ_SLEEP_TIME_IN_MS)
#define MIN_POLL_RES_SLEEP Sleep(MIN_POLL_RES_SLEEP_TIME_IN_MS)

#define MAX_POLL_SLEEP Sleep(MAX_SLEEP_TIME_IN_MS)

/*extern*/
extern int pipe_start();

/*extern "C"*/
extern "C" ST_BOOLEAN doIt;
extern "C" ST_VOID doCommService (ST_VOID);

/*enum*/
enum
    {
    REQ_NOT_SEND = 0,
    REQ_SEND = 1,
    } POLL_STATE;

enum
    {
    RESERVED_LOCAL_SCL = -1,
    RESERVED_REMOTE_LITE = -3
    } RESERVED_UID;

enum
    {
    SERVER = 1,
    CLIENT = 2
    } IED_TYPE;

enum
    {
    STATUS_DISCONNECTED = 0,
    STATUS_CONNECTED = 1
    } ASSOC_STATUS;

enum
    {
    LAYER_DO = 1,
    LAYER_DA = 2
    }DATA_MODEL_LAYER;

/*typedef struct*/
typedef struct
    {
    int id;
    int keepConnected;
    char pXmlPath[MAX_FILE_NAME];
    char pSclPath[MAX_FILE_NAME];
    char pIedName[MAX_FILE_NAME];
    char pApName[MAX_FILE_NAME];
    }THREAD_DATA_STRUCT;

typedef struct poll_ctrl
    {
    DBL_LNK l;

    int state;
    int type;
    int len;
    ST_DOUBLE stopTime;
    char domName[MAX_FILE_NAME];
    char leaf[MAX_FILE_NAME];
    char result [MAX_FILE_NAME];
    MVL_REQ_PEND *reqCtrl;
    READ_REQ_INFO *req_info;
    MVL_READ_RESP_PARSE_INFO* parse_info;
    }POLL_ARG_STRUCT;

typedef struct 
    {
    int count;
    POLL_ARG_STRUCT* listItem;
    } POLL_LIST_STRUCT;

/*extern "C"*/
extern "C" int init();
extern "C" int post();
extern "C" ST_VOID scl_info_destroy (SCL_INFO *scl_info);

/*variables*/
nPipe* pipe;

/*class*/
class IED
    {
    public:
        IED (int uId, char* pIp);
        ~IED ();
        /*functions*/
        int getRemoteDataModel (int keepConnected, char* pXmlPath);
        int iedConnect ();
        int iedDisconnect ();
        int getEdition  (SCL_INFO* scl_info, SCL_SERVER* scl_server,  int* edition);
        int getMappedIeds  (SCL_INFO* scl_info, SCL_SERVER* pSclServer, STRUCT_GET_LIST* getList);
        int getLocalDataset (char* domName, MVL_DOM_CTRL* dom);
        int getSclInfo (char* pSclPath, SCL_INFO* pSclInfo);
        int getVmdInfo (char* pXmlPath, char* pSclPath, char* pIedName, char* pApName);
        int getRemoteDataModellLite ();
        void getIedName (STRUCT_GET_LIST* list, char* iedName);
        int getLD (STRUCT_GET_LIST* getList);
        int getLN (STRUCT_GET_LIST* getList, char* domName);
        int getDataset (char* domName);
        int getControlBlock (char* domName, STRUCT_GET_LIST* getCbList);
        int getLocalControlBlock (char* pIedName, char* pApName, char* domName, STRUCT_GET_LIST* getCbList, SCL_INFO* pSclInfo);
        int getDS (STRUCT_GET_LIST* getList, char* domName);
        int getDataStruct (STRUCT_GET* pLn, char* domName, char* lnName);
        int getDsStruct (STRUCT_GET_LIST* getList, char* domName, char* dsName);
        int deleteDataStruct (STRUCT_GET* pLn);
        int printlnStruct (RUNTIME_TYPE* rtPtrOut, ST_INT  numRtOut, char* pRef, STRUCT_GET_LIST* list);
        int printStruct (RUNTIME_TYPE* rtPtrOut, ST_INT numRtOut, int* cnt, char* pRef, int layer, STRUCT_GET_LIST* list);
        int getDataEx (char* domName, char* leaf, char* data);
        int getData (char* domName, char* leaf, char* data, int type, int len);
        int setData (char* domName, char* leaf, char* data, int type, int len);
        int setDataEx (char* domName, char* leaf, char* data, char* error);
        int getDataAsync (POLL_ARG_STRUCT* leafItem);
        int getStructData (char* domName, char* leaf, char* data, int allocSize);
        int setStructData (char* domName, char* leaf, char* data);
        int operate (char* domName, 
                        char* leaf, 
                        char* data, 
                        char* addcause,
                        char* orCat,
                        char* interlockCheck,
                        char* synchrocheck,
                        char* test,
                        int timeInSec,
                        int ctlModel);
        int selectWithValue (char* domName, 
                            char* leafOper, 
                            char* leafSBOw, 
                            char* data, 
                            char* addcause,
                            char* orCat,
                            char* interlockCheck,
                            char* synchrocheck,
                            char* test,
                            int timeInSec);
        int select (char* domName, char* leaf);
        int getAllGoCbValues (char* domName, STRUCT_GET_LIST* getCbList);
        int getAllRgCbValues (char* domName, STRUCT_GET_LIST* getCbList);
        int getAllBrCbValues (char* domName, STRUCT_GET_LIST* getCbList);
        int getAllUrCbValues (char* domName, STRUCT_GET_LIST* getCbList);
        int getAllLocalGoCbValues (char* pIedName, char* pApName, char* domName, STRUCT_GET_LIST* getCbList, SCL_INFO* pSclInfo);
        int getAllLocalRgCbValues (char* pIedName, char* pApName, char* domName, STRUCT_GET_LIST* getCbList, SCL_INFO* pSclInfo);
        int getAllLocalBrCbValues (char* pIedName, char* pApName, char* domName, STRUCT_GET_LIST* getCbList, SCL_INFO* pSclInfo);
        int getAllLocalUrCbValues (char* pIedName, char* pApName, char* domName, STRUCT_GET_LIST* getCbList, SCL_INFO* pSclInfo);
        int getGseComm (SCL_INFO* pSclInfo, char* pIedName, char* pApName, char* pCbName, SCL_GSE** pSclGse);

        /*variables*/
        POLL_LIST_STRUCT* leafList;
        FILE* pXmlFp;
        char sId[MAX_AR_LEN + 1];
        char ip[MAX_AR_LEN + 1];
        char iedName[MAX_AR_LEN + 1];

        mServer* acsiServer;
        mLogicalDevice* acsiLogicalDevice;
        mAssociation* acsiAssociation;
        mLogicalNode* acsiLogicalNode;
        mData* acsiData;
        mDataSet* acsiDataSet;
        mGOOSE* acsiGoCB;
        mBRCB* acsiBRCB;
        mURCB* acsiURCB;
        mControl* acsiControl;
        mLocal* local;
        XML* xml;
        int associationStatus; 
        int iId;

    private:
        /*variables*/
        MVL_NET_INFO* netInfo;
    };

/*functions*/
int getIedListLocal (char* pSclPath, char* result, int size);
int getIedLocal (char* pSclPath, char* pIedName, char* pApName);
void listClear (STRUCT_LEAF* pListItem);
void pipeWrite (char* type, char* subType, char* status, int id, int ret, int percentage);

/*Exposed functions*/
DllExport
int 
xGetIedLite (char* ip, char* iedName);

DllExport
int 
xGetIedAsync (int id, int keepConnected, char* pXmlPath);

DllExport
int 
xGetIed (int id, int keepConnected, char* pXmlPath);

DllExport
int
xAddIed (int id, char* ip);

DllExport
int
xRemoveIed (int id);

DllExport
int
xDllInit ();

DllExport
int
xDllPost ();

DllExport
int
xGetIedListLocal (char* pSclPath, char* result, int size);

DllExport
int
xGetIedLocalAsync (char* pXmlPath, char* pSclPath, char* pIedName, char* pApName, int uId);

DllExport
int
xGetIedLocal (char* pXmlPath, char* pSclPath, char* pIedName, char* pApName, int uId);

DllExport
int
xGetDataEx (int id, char* domName, char* leaf, char* data);

DllExport
int
xGetData (int id, char* domName, char* leaf, char* data, int type, int len);

DllExport
int
xSetData (int id, char* domName, char* leaf, char* data, int type, int len);

DllExport
int
xSetDataEx (int id, char* domName, char* leaf, char* data, char* error);

DllExport
int
xConnect (int id);

DllExport
int
xDisconnect (int id);

DllExport
int 
xChangeIp (int id, char* newIp);

DllExport
int
xAddToPollingList (int id, char* domName, char* leaf, int type, int len);

DllExport
int
xRemoveFromPollingList (int id, char* domName, char* leaf);

DllExport
int
xGetStructData (int id, char* domName, char* leaf, char* data, int allocSize);

DllExport
int
xSelect (int id, char* domName, char* leaf);

DllExport
int
xSelectWithValue (int id, 
                    char* domName, 
                    char* leafOper, 
                    char* leafSBOw, 
                    char* data, 
                    char* addcause,
                    char* orCat,
                    char* interlockCheck,
                    char* synchrocheck,
                    char* test,
                    int timeInSec);

DllExport
int
xOperate (int id, 
            char* domName, 
            char* leaf, 
            char* data, 
            char* addcause,
            char* orCat,
            char* interlockCheck,
            char* synchrocheck,
            char* test,
            int timeInSec,
            int ctlModel);

#endif