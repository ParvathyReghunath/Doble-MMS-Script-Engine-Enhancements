/************************************************************************/
/* SOFTWARE MODULE HEADER ***********************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,       */
/*              2003 - 2016, All Rights Reserved                        */
/*                                                                      */
/* MODULE NAME : snap_l.h                                               */
/* PRODUCT(S)  :                                                        */
/*                                                                      */
/* MODULE DESCRIPTION :                                                 */
/*   Secure Network Access Provider Lite (SNAP-Lite) structs.           */
/*                                                                      */
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :                            */
/*                                                                      */
/* MODIFICATION LOG :                                                   */
/*                                                                      */
/*  Date     Who   Rev                     Comments                     */
/*----------------------------------------------------------------------*/
/* 08/15/16  EJV  SNAPL_VERSION_STR: changed to V3.8100 for             */
/*                  MMS-SECURE-142-764 V11.8100 Linux 6&7 32/64-bit     */
/* 07/13/16  EJV  SNAPL_VERSION_STR: change to V3.8003 to align with    */
/*                S_SEC_VERSION_STR.                                    */
/* 01/20/16  EJV  Changed copyright year to 2016.                       */
/* 10/29/15  EJV  SNAPL_VERSION_STR: changed to V3.8002 for             */
/*                AXS4-61850-142-095 V7.0000,MMS-SECURE-142-664 V11.8000*/
/* 12/04/14  EJV  SNAPL_VERSION_STR: changed to V3.8001 for Windows rel */
/*                MMS-SECURE-142-064 V11.8000 (impl SNAPLITE-64).       */
/* 12/04/14  EJV  SNAPL_VERSION_STR: changed to V3.8000 for             */
/*                 AXS4-ICCP-142-095 V6.0000.                           */
/* 06/19/14  JRB  Add snapThrStart, snapThrStop.                        */
/* 04/24/14  EJV  Ported for MMS-LITE-SECURE (defining SNAP_LITE_THR).  */
/* 11/26/13  EJV   SNAPL_VERSION_STR: changed to V3.6000 for AXS4-ICCP. */
/* 01/02/13  EJV   SNAPL_VERSION_STR: changed to V3.3100 for Windows rel*/
/*                 MMS-SECURE-14x-064 V11.3100. Chg copyright year.     */
/* 11/05/12  EJV           SNAPL_VERSION_STR: changed to V3.3000 for    */
/*                           MMS-SECURE-14x-664 V11.3000.               */
/* 06/20/11  EJV           SNAPL_VERSION_STR: changed to V3.2000 for    */
/*                           MMS-SECURE-14x-764 V11.2000.               */
/* 01/11/12  EJV           Changed copyright year to 2011 for           */
/*                           MMS-SECURE-142-064 V11.1000 rel on Windows.*/
/* 03/22/11  EJV           SNAPL_VERSION_STR: changed to V3.1000 for    */
/*                           MMS-SECURE-14x-415 V3.0000.                */
/* 01/14/11  EJV           Changed copyright year to 2011.              */
/* 11/08/10  EJV           Changed to V3.0200 (ICCP-LITE V5.4000 B2 &up)*/
/*                         Implemented Win service.                     */
/* 10/06/10  EJV           SNAP_CTX: added sockCtxInit.                 */
/*           EJV           Changed to V3.0100 (ICCP-LITE V5.4000).      */
/*           EJV           Prev chg   V3.0000 (ICCP-LITE V5.3000).      */
/* 08/02/10  JRB           Chg sockCtx from (GEN_SOCK_CTXT *) to        */
/*                         (GEN_SOCK_CTXT) so don't need to alloc/free. */
/* 06/21/10  JRB           Update version. Add SNAPL_COPYRIGHT define.  */
/* 10/07/09  EJV    07     Changed to V2.0101 (hash-type link option).  */
/* 08/13/09  EJV    06     Changed to V2.0100 (final Linux release).    */
/* 05/12/09  EJV    05     Mv logging related defs to snaplog.h.        */
/* 12/29/08  EJV    04     Changed to V2.0050 (Linux release).          */
/* 02/18/08  EJV    03     Changed to V2.00 (Sun Solaris release).      */
/*                         Chg SNAPL_UTILITY_LOG_CFG to "logcfgu.xml".  */
/*                         Chg SNAPL_PROCESS_LOG_CFG to "logcfg.xml"    */
/* 01/29/08  EJV    02     SNAP_CTX: added max_pipeline and max_con_time*/
/*                         Changed define for logcfg.xml.               */
/* 03/14/07  EJV    01     Moved from snap_l_main.c                     */
/************************************************************************/

#ifndef SNAP_L_INCLUDED
#define SNAP_L_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "snap_l_inf.h"

/*------------------------------------------------------*/
/*            defines                                   */
/*------------------------------------------------------*/

#define SNAPL_VERSION_STR       "3.8100"        /* SNAP-Lite has its own version   */
/* although it could be sync with the S_SEC_VERSION_STR for major releases, also */
/* it would be good to keep the minor version aligned with the MMS-EASE version  */
#define SNAPL_COPYRIGHT "Copyright (c) 1986-2016 SISCO, Inc. All Rights Reserved."

#if defined(_WIN32)
#define SNAPL_SERVICE_NAME       "SISCO SNAP-Lite"      /* Win service name     */
#define SNAPL_PROCESS_NAME       "snap_l"               /* SNAP-Lite task name  */

#else  /* Linux, UNIX */
#define SNAPL_UTILITY            "snap_l"               /* Utility to start SNAP-Lite daemon    */

#define SNAPL_PROCESS_NAME       "snap_ld"              /* SNAP-Lite daemon     */
#endif /* Linux, UNIX */



  /* timeout defines */
#define SNAP_WAIT_THREAD_TIMEOUT        5000    /* milliseconds */
#define SNAP_WAIT_TIMEOUT               1000    /* milliseconds */
#define SNAP_WAIT_MAIN_TIMEOUT          1000    /* milliseconds */
#define SNAP_WAIT_SMALL_TIMEOUT         50      /* milliseconds, used in rekeying and thread termination*/
#define SNAP_REKEY_TIME                 1000    /* milliseconds, how often to send rekey heartbeat      */
/* (used also to drive the rekeying process)                    */
#define SNAP_LOG_QUEUES_TIME            1000    /* milliseconds */
/* SNAP_WAIT_TIMEOUT should be larger than SNAP_WAIT_MAIN_TIMEOUT       */

/* SNAP states (secure connections) */
#define SNAP_IDLE               0
#define SNAP_CONNECTING         1
#define SNAP_CONNECTED          2
#define SNAP_CLOSING            3
#define SNAP_DISCONNECTED       4
#define SNAP_FAILED            10

/* types for an operation */
#define SNAP_LOCAL_SSL          1
#define SNAP_REMOTE_SSL         2

/*------------------------------------------------------*/
/*            structures                                */
/*------------------------------------------------------*/

/* arguments passed to service */
typedef struct snap_args_tag
  {
  ST_BOOLEAN     bService;
  ST_CHAR        cmdLine[1024];
  } SNAP_ARGS;

/* This structure is used to manage connection between local app and a remote   */
typedef struct snap_ctrl_tag
  {
  DBL_LNK        link;                  /* link list                            */

  ST_DOUBLE      statLogTime;           /* last time we logged statistics       */
  ST_DOUBLE      rekeyTime;             /* next time to rekey if we are idle    */

  /* Local app connection info */
  GEN_SOCK      *locSock;               /* pointer to socket info               */
  ST_UINT        locState;              /* connection state                     */
  GEN_SOCK_DATA *locSendQue;            /* Pending send queue to the local app  */
  ST_ULONG       locQueCnt;
  /* Local App's IP Address and Port are configured in globals */

  /* Remote connection info */
  GEN_SOCK      *remSock;               /* pointer to socket info               */
  ST_UINT        remState;              /* connection state                     */
  GEN_SOCK_DATA *remSendQue;            /* Pending send queue to the Remote     */
  ST_ULONG       remQueCnt;
  ST_UINT16      remIpPort;             /* remote IP port                       */
  ST_UINT32      remIpAddr;             /* remote IP Addr                       */

  ST_DOUBLE      startTime;             /* timer for some operations            */
  ST_EVENT_SEM   event;                 /* event to wake up _snapXXXComThread   */
  } SNAP_CTRL;

/* This structure holds SNAP's core parameters */
typedef struct snap_ctx_tag
  {
  ST_UINT       snapState;              /* SNAPL_STATE_..., process state                       */
  ST_EVENT_SEM  snapExitEvent;          /* named event to direct SNAP to exit, checked by SNAP  */

  S_SEC_CONFIG  *secCfg;                /* ptr to global security configuration                 */

  SNAP_CTRL     *snapCtrlListLoc;       /* list of SNAP_CTRL for conns initiated from local app */
  SNAP_CTRL     *snapCtrlListRem;       /* list of SNAP_CTRL for conns initiated from remotes   */
  ST_EVENT_SEM   eventLoc;              /* event semaphore for _snapLocComThread                */
  ST_EVENT_SEM   eventRem;              /* event semaphore for _snapRemComThread                */

  ST_THREAD_HANDLE thLoc;               /* _snapLocComThread handle                             */
  ST_THREAD_ID     tidLoc;              /* _snapLocComThread ID                                 */
  ST_THREAD_HANDLE thRem;               /* _snapRemComThread handle                             */
  ST_THREAD_ID     tidRem;              /* _snapRemComThread ID                                 */

  ST_THREAD_HANDLE thIpcThread;         /* snapIpcThread handle                                 */
  ST_THREAD_ID     tIdIpcThread;        /* snapIpcThread ID                                     */


  GEN_SOCK *locListenSock;              /* listen socket for connections from local app         */
  GEN_SOCK *remListenSockSSL[S_SSL_MAX_LISTEN_PORTS];   /* tbl of secure listen sockets for     */
  /* connections from remotes               */
  ST_BOOLEAN    sockCtxInit;            /* SD_TRUE if sockCtx initialized                       */
  GEN_SOCK_CTXT sockCtx;                /* this is gensock2 context for SNAP                    */

  /* additional SNAP-Lite config parameters, defauts may be changed with program args           */
  ST_UINT     tcp_control_cnt;          /* max num of simultaneous TCP conns                    */
  ST_UINT     max_pipeline;             /* max num of TCP connections queued to user application*/
  ST_UINT     max_con_time;             /* ms, how long we will wait for TPDU-CR and P-CONNECT  */
  } SNAP_CTX;


/*------------------------------------------------------*/
/*            functions                                 */
/*------------------------------------------------------*/

#if defined(SNAP_LITE_THR) /* MMS-LITE with SNAP-Lite threads  */
ST_THREAD_RET ST_THREAD_CALL_CONV snapThr (ST_THREAD_ARG pArg);
ST_BOOLEAN snapThrRunningState (ST_VOID);
ST_RET snapThrEnd (ST_VOID);
ST_RET snapThrStart (
  ST_THREAD_HANDLE *th,
  ST_THREAD_ID     *tid,
  ST_LONG waitTimeMs);  /* time (ms) to wait for thread to start  */
ST_RET snapThrStop (
  ST_THREAD_HANDLE th,
  ST_THREAD_ID     tid,
  ST_LONG waitTimeMs);  /* time (ms) to wait for thread to stop   */

#else /* !defined(SNAP_LITE_THR) */
/* SNAP-Lite main thread */
ST_THREAD_RET ST_THREAD_CALL_CONV snapMain (ST_THREAD_ARG pArg);
#endif /* !defined(SNAP_LITE_THR) */

/* SNAP-Lite logging functions */
ST_RET snapLogMasks (ST_VOID);

/* funtions in snap_l_inf.c */
ST_THREAD_RET ST_THREAD_CALL_CONV snapIpcThread (ST_THREAD_ARG pArg);

/* funtions in snap_l_sec.c */
ST_RET snapStartSecurityThreads (SNAP_CTX *snapCtx);
ST_RET snapCleanSecurityThreads (SNAP_CTX *snapCtx);


/*------------------------------------------------------*/
/*            variables                                 */
/*------------------------------------------------------*/

/* SNAP-Lite threads check the _snapExit flag and exit when the flag is set */
extern ST_BOOLEAN _snapExit;            /* global exit flag     */


#ifdef __cplusplus
  }
#endif

#endif /* SNAP_L_INCLUDED */

