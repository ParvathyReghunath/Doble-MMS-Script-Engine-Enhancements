/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*      (c) Copyright Systems Integration Specialists Company, Inc.,    */
/*      2005 - 2014 All Rights Reserved                                 */
/*                                                                      */
/* MODULE NAME : tp0_ipc.h                                              */
/* PRODUCT(S)  : MMS-EASE-142-xxx, MMS-LITE                             */
/*                                                                      */
/* MODULE DESCRIPTION :                                                 */
/*      Interface to SNAP-Lite task.                                    */
/*                                                                      */
/* MODIFICATION LOG :                                                   */
/*  Date     Who   Rev     Comments                                     */
/* --------  ---  ------   -------------------------------------------  */
/* 05/13/14  EJV  Del BIND_ID_UNION, use PTR_U64_UNION in glbtypes.h    */
/*                snapl_ipc_bind_cnf snapl_ipc_unbind_cnf: changed      */
/*                   ST_VOID *user_bind_id to ST_UINT64 parameter.      */
/* 04/03/14  EJV  Added snapl_ipc_check_running_status fun.             */
/* 03/13/14  EJV   Added snapl_ipc_turn_listen_off(on) protos.          */
/* 11/17/09  JRB    04     Add snapl_ipc_bind_cnf, snapl_ipc_unbind_cnf.*/
/*                         Add BIND_ID_UNION.                           */
/* 01/05/09  EJV    03     snapl_ipc_bind: chg name to user_bind_id_u64.*/
/*                         snapl_ipc_unbind: chg name to user_bind_id_u64.*/
/* 01/10/08  EJV    02     Chg to event driven IPC.                     */
/* 02/15/07  EJV    01     Moved from tp0_sock.h                        */
/************************************************************************/
#ifndef TP0_IPC_INCLUDED
#define TP0_IPC_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "snap_l_ipc.h"

/************************************************************************/
/* Defines, etc. for interfacing to SNAP-Lite process.                  */
/************************************************************************/

/* states for ipcUsrState */
#define IPC_USTATE_CLOSED       0       /* not connected to SNAP-Lite           */
#define IPC_USTATE_CONNECTED    1       /* connected to SNAP-Lite               */
#define IPC_USTATE_INITIALIZING 2       /* sending INIT info to SNAP-Lite       */
#define IPC_USTATE_ACTIVE       3       /* ready for IPC  with SNAP-Lite        */

/* IPC msg receive states (ipc_rx_state) */
#define IPC_RX_MSG              0       /* receiving IPC msg from SNAP-Lite     */
#define IPC_RX_FD               1       /* receiving sock handle from SNAP-Lite */
#define IPC_RX_TPDU_CR          2       /* receiving TPDU-CR from SNAP-Lite     */
#define IPC_RX_SPDU_PCON        3       /* receiving PCON SPDU from SNAP-Lite   */

/* queue element for IPC msg to send out */
typedef struct IPC_MSG_TX_tag
  {
  DBL_LNK          l;
  ST_DOUBLE        start_time;
  SNAPL_IPC_MSG    ipc_msg;
  ST_UINT          ipc_msg_done_cnt;
  } IPC_MSG_TX;

/* queue element for IPC msg to send out */
typedef struct IPC_MSG_RX_tag
  {
  ST_DOUBLE        state;
  ST_DOUBLE        start_time;
  SNAPL_IPC_MSG    ipc_msg;
  ST_UINT          ipc_msg_done_cnt;

  SOCKET           fd;                          /* TCP socket handle passed from SNAP-Lite      */
#if defined(_WIN32)
  WSAPROTOCOL_INFO protocol_info;               /* for receiving handle                         */
  ST_UINT          protocol_info_done_cnt;      /* num of bytes received                        */
#endif
  ST_UINT          tpdu_len_done;               /* # of bytes already received from SNAP-Lite   */
  GEN_SOCK_DATA   *tpdu;                        /* calloc buffer for TPDU-CR                    */
  ST_UINT          spdu_len_done;               /* # of bytes already received from SNAP-Lite   */
  GEN_SOCK_DATA   *spdu;                        /* calloc buffer for P-CONNECT SPDU             */
  } IPC_MSG_RX;

/* info related to the TP0 IPC with the SNAP-Lite                       */
typedef struct IPC_SOCK_CTX_tag
  {
  SOCKET           hIpcSock;            /* socket handle for IPC with the SNAP-Lite     */
                                        /*   Linux, UNIX: UNIX domain socket used       */
                                        /*   _WIN32: normal socket used                 */
  ST_INT           ipcUsrState;         /* one of IPC_USTATE_*                          */

  ST_BOOLEAN       chkWritable;         /* flag to check for write event in select      */
  IPC_MSG_TX      *ipc_tx_que;          /* multiple messages may be queued              */

  IPC_MSG_RX       ipc_rx;              /* only one IPC msg can be received             */

  ST_INT           ipcTimeout;          /* ms, max time to wait for IPC msg             */
  /* Windows only - start */
  ST_EVENT_SEM     hIpcEvent;           /* app waits for this event to be signaled      */
  ST_THREAD_HANDLE thIpcEvent;
  ST_THREAD_ID     tIdIpcEvent;
  /* Windows only - end */

  /* Linux, UNIX only - start */
  ST_CHAR          ipcDir[S_MAX_PATH];  /* IPC directory (where to find domsock file),  */
                                        /* set by calling setListenerIPCDir fun. If it  */
                                        /* is not set by user appl the default dir      */
                                        /* SNAPL_IPC_PATH will be used.                 */
  /* Linux, UNIX only - end   */

  } IPC_SOCK_CTX;



/************************************************************************/
/* external variables                                                   */
/************************************************************************/

extern   IPC_SOCK_CTX            ipcSockCtx;
#define  IPC_SOCK_HANDLE        (ipcSockCtx.hIpcSock)
#define  IPC_SOCK_CHECK_WRITE   (ipcSockCtx.chkWritable)

/************************************************************************/
/* IPC functions called by tp0*.c to interface with the SNAP-Lite.      */
/************************************************************************/

ST_RET     snapl_ipc_start  (ST_EVENT_SEM hIpcEvent);
ST_RET     snapl_ipc_end    (ST_VOID);
ST_RET     snapl_ipc_init   (ST_UINT32 max_spdu_len, ST_UINT32 max_tpdu_len);
ST_RET     snapl_ipc_bind   (ST_UCHAR *sels, ST_UINT sels_len, ST_UINT64 user_bind_id_u64);
ST_RET     snapl_ipc_unbind (ST_UINT64 user_bind_id_u64);
ST_RET     snapl_ipc_turn_listen_off (ST_RET (*callbackFun) (ST_INT));
ST_RET     snapl_ipc_turn_listen_on  (ST_RET (*callbackFun) (ST_INT));
#if defined(_WIN32)
ST_RET     snapl_ipc_check_running_status (ST_VOID);
#endif
ST_BOOLEAN snapl_ipc_event  (ST_VOID);

ST_RET     snapl_ipc_processRxMsg (GEN_SOCK *pSock, GEN_SOCK_DATA *sockData);
ST_RET     snapl_ipc_verifyTPDU   (GEN_SOCK *pSock, GEN_SOCK_DATA *sockData);
ST_RET     sendRemSecurityInfo    (GEN_SOCK *pSock, SNAPL_CON_PARAM *con_info);
ST_RET     getRemSecurityInfo     (GEN_SOCK *pSock, SNAPL_CON_PARAM *con_info);

/* NOTE: same args as "u_cosp_bind_cnf" but totally different.  */
ST_VOID snapl_ipc_bind_cnf (ST_UINT64 user_bind_id_u64, ST_VOID *cosp_bind_id, ST_RET result);
/*NOTE: same args as "u_cosp_unbind_cnf" but totally different. */
ST_VOID snapl_ipc_unbind_cnf (ST_UINT64 user_bind_id_u64);


#ifdef __cplusplus
}
#endif

#endif /* !TP0_IPC_INCLUDED */

