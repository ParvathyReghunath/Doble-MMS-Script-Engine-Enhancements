/************************************************************************/
/* SOFTWARE MODULE HEADER ***********************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,       */
/*              2003 - 2014, All Rights Reserved                        */
/*                                                                      */
/* MODULE NAME : snaplog.h                                              */
/* PRODUCT(S)  :                                                        */
/*                                                                      */
/* MODULE DESCRIPTION :                                                 */
/*   Secure Network Access Provider (SNAP) logging defines and macros.  */
/*                                                                      */
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :                            */
/*                                                                      */
/* MODIFICATION LOG :                                                   */
/*  Date     Who   Rev                     Comments                     */
/* --------  ---  ------   -------------------------------------------  */
/* 06/23/14  EJV  Added extern LOGCFG_VALUE_GROUP snapLogMaskMapCtrl.   */
/* 05/12/09  EJV    07     Mv logging related defs from snap_l.h.       */
/* 02/28/09  EJV    06     Added extern logCfgFile.                     */
/* 11/29/08  EJV    05     Added more slog macros. Add snalCfgDefLogging*/
/*                           and snapSetLogMasks protos.                */
/* 01/29/08  EJV    04     Added more slog macros.                      */
/* 07/02/03  EJV    03     Added FLOWC macros                           */
/* 04/11/03  EJV    02     Added __cplusplus.                           */
/* 01/07/03  EJV    01     Created                                      */
/************************************************************************/
#ifndef SNAPLOG_INCLUDED
#define SNAPLOG_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

/* logging masks */
#define SNAP_LOG_ERR    0x0001
#define SNAP_LOG_NERR   0x0002
#define SNAP_LOG_FLOW   0x0004
#define SNAP_LOG_DATA   0x0008
#define SNAP_LOG_DEBUG  0x0010

extern ST_UINT  snap_debug_sel;

/* Log type strings */
extern SD_CONST ST_CHAR *SD_CONST _snap_err_logstr;
extern SD_CONST ST_CHAR *SD_CONST _snap_nerr_logstr;
extern SD_CONST ST_CHAR *SD_CONST _snap_flow_logstr;
extern SD_CONST ST_CHAR *SD_CONST _snap_data_logstr;
extern SD_CONST ST_CHAR *SD_CONST _snap_debug_logstr;

/* default SNAP log and log configuration file names    */
#if defined(_WIN32)
#define SNAPL_PROCESS_LOG_NAME   "snap_l.log"           /* default log name     */
#define SNAPL_PROCESS_LOG_CFG    "snaplogcfg.xml"       /* default log cfg      */

#else  /* Linux, UNIX */
#define SNAPL_PROCESS_LOG_NAME   "snap_l.log"           /* default log name     */
#define SNAPL_PROCESS_LOG_CFG    "snaplogcfg.xml"       /* default log cfg      */

#define SNAPL_UTILITY_LOG_NAME   "snap_lu.log"          /* default log name     */
#define SNAPL_UTILITY_LOG_CFG    "snaplogcfgu.xml"      /* default log cfg      */
#endif /* Linux, UNIX */

extern ST_CHAR  logCfgFile[S_MAX_PATH]; /* set during init, use during cfg updates */

extern LOGCFG_VALUE_GROUP snapLogMaskMapCtrl;


ST_RET snapCfgDefLogging (ST_VOID);
ST_RET snapSetLogMasks   (ST_VOID);

#define SNAPLOG_ERR0(a) \
  SLOG_0 (snap_debug_sel & SNAP_LOG_ERR, _snap_err_logstr, a)
#define SNAPLOG_ERR1(a,b) \
  SLOG_1 (snap_debug_sel & SNAP_LOG_ERR, _snap_err_logstr, a,b)
#define SNAPLOG_ERR2(a,b,c) \
  SLOG_2 (snap_debug_sel & SNAP_LOG_ERR, _snap_err_logstr, a,b,c)
#define SNAPLOG_ERR3(a,b,c,d) \
  SLOG_3 (snap_debug_sel & SNAP_LOG_ERR, _snap_err_logstr, a,b,c,d)
#define SNAPLOG_ERR4(a,b,c,d,e) \
  SLOG_4 (snap_debug_sel & SNAP_LOG_ERR, _snap_err_logstr, a,b,c,d,e)

#define SNAPLOG_CERR0(a) \
  SLOGC_0 (snap_debug_sel & SNAP_LOG_ERR,a)
#define SNAPLOG_CERR1(a,b) \
  SLOGC_1 (snap_debug_sel & SNAP_LOG_ERR,a,b)
#define SNAPLOG_CERR2(a,b,c) \
  SLOGC_2 (snap_debug_sel & SNAP_LOG_ERR,a,b,c)

#define SNAPLOG_ERRH(a,b) \
  SLOGH (snap_debug_sel & SNAP_LOG_ERR,a,b)


#define SNAPLOG_NERR0(a) \
  SLOG_0 (snap_debug_sel & SNAP_LOG_NERR, _snap_nerr_logstr, a)
#define SNAPLOG_NERR1(a,b) \
  SLOG_1 (snap_debug_sel & SNAP_LOG_NERR, _snap_nerr_logstr, a,b)
#define SNAPLOG_NERR2(a,b,c) \
  SLOG_2 (snap_debug_sel & SNAP_LOG_NERR, _snap_nerr_logstr, a,b,c)
#define SNAPLOG_NERR3(a,b,c,d) \
  SLOG_3 (snap_debug_sel & SNAP_LOG_NERR, _snap_nerr_logstr, a,b,c,d)
#define SNAPLOG_NERR4(a,b,c,d,e) \
  SLOG_4 (snap_debug_sel & SNAP_LOG_NERR, _snap_nerr_logstr, a,b,c,d,e)
#define SNAPLOG_NERR5(a,b,c,d,e,f) \
  SLOG_5 (snap_debug_sel & SNAP_LOG_NERR, _snap_nerr_logstr, a,b,c,d,e,f)
#define SNAPLOG_NERR6(a,b,c,d,e,f,g) \
  SLOG_6 (snap_debug_sel & SNAP_LOG_NERR, _snap_nerr_logstr, a,b,c,d,e,f,g)

#define SNAPLOG_NERRH(a,b) \
  SLOGH (snap_debug_sel & SNAP_LOG_NERR,a,b)

#define SNAPLOG_FLOW0(a) \
  SLOG_0 (snap_debug_sel & SNAP_LOG_FLOW, _snap_flow_logstr, a)
#define SNAPLOG_FLOW1(a,b) \
  SLOG_1 (snap_debug_sel & SNAP_LOG_FLOW, _snap_flow_logstr, a,b)
#define SNAPLOG_FLOW2(a,b,c) \
  SLOG_2 (snap_debug_sel & SNAP_LOG_FLOW, _snap_flow_logstr, a,b,c)
#define SNAPLOG_FLOW3(a,b,c,d) \
  SLOG_3 (snap_debug_sel & SNAP_LOG_FLOW, _snap_flow_logstr, a,b,c,d)
#define SNAPLOG_FLOW4(a,b,c,d,e) \
  SLOG_4 (snap_debug_sel & SNAP_LOG_FLOW, _snap_flow_logstr, a,b,c,d,e)
#define SNAPLOG_FLOW5(a,b,c,d,e,f) \
  SLOG_5 (snap_debug_sel & SNAP_LOG_FLOW, _snap_flow_logstr, a,b,c,d,e,f)
#define SNAPLOG_FLOW6(a,b,c,d,e,f,g) \
  SLOG_6 (snap_debug_sel & SNAP_LOG_FLOW, _snap_flow_logstr, a,b,c,d,e,f,g)

#define SNAPLOG_FLOWC0(a) \
  SLOGC_0 (snap_debug_sel & SNAP_LOG_FLOW, a)
#define SNAPLOG_FLOWC1(a,b) \
  SLOGC_1 (snap_debug_sel & SNAP_LOG_FLOW, a,b)
#define SNAPLOG_FLOWC2(a,b,c) \
  SLOGC_2 (snap_debug_sel & SNAP_LOG_FLOW, a,b,c)
#define SNAPLOG_FLOWC3(a,b,c,d) \
  SLOGC_3 (snap_debug_sel & SNAP_LOG_FLOW, a,b,c,d)
#define SNAPLOG_FLOWC4(a,b,c,d,e) \
  SLOGC_4 (snap_debug_sel & SNAP_LOG_FLOW, a,b,c,d,e)
#define SNAPLOG_FLOWC5(a,b,c,d,e,f) \
  SLOGC_5 (snap_debug_sel & SNAP_LOG_FLOW, a,b,c,d,e,f)


#define SNAPLOG_FLOWH(a,b) \
  SLOGH (snap_debug_sel & SNAP_LOG_FLOW,a,b)

#define SNAPLOG_DATAH(a,b) \
  SLOGH (snap_debug_sel & SNAP_LOG_DATA,a,b)



#define SNAPLOG_DEBUG0(a) \
  SLOG_0 (snap_debug_sel & SNAP_LOG_DEBUG, _snap_debug_logstr, a)
#define SNAPLOG_DEBUG1(a,b) \
  SLOG_1 (snap_debug_sel & SNAP_LOG_DEBUG, _snap_debug_logstr, a,b)
#define SNAPLOG_DEBUG2(a,b,c) \
  SLOG_2 (snap_debug_sel & SNAP_LOG_DEBUG, _snap_debug_logstr, a,b,c)
#define SNAPLOG_DEBUG3(a,b,c,d) \
  SLOG_3 (snap_debug_sel & SNAP_LOG_DEBUG, _snap_debug_logstr, a,b,c,d)
#define SNAPLOG_DEBUG4(a,b,c,d,e) \
  SLOG_4 (snap_debug_sel & SNAP_LOG_DEBUG, _snap_debug_logstr, a,b,c,d,e)

#define SNAPLOG_DEBUGC0(a) \
  SLOGC_0 (snap_debug_sel & SNAP_LOG_DEBUG, a)
#define SNAPLOG_DEBUGC1(a,b) \
  SLOGC_1 (snap_debug_sel & SNAP_LOG_DEBUG, a,b)
#define SNAPLOG_DEBUGC2(a,b,c) \
  SLOGC_2 (snap_debug_sel & SNAP_LOG_DEBUG, a,b,c)
#define SNAPLOG_DEBUGC3(a,b,c,d) \
  SLOGC_3 (snap_debug_sel & SNAP_LOG_DEBUG, a,b,c,d)
#define SNAPLOG_DEBUGC4(a,b,c,d,e) \
  SLOGC_4 (snap_debug_sel & SNAP_LOG_DEBUG, a,b,c,d,e)

#define SNAPLOG_DEBUGH(a,b) \
  SLOGH (snap_debug_sel & SNAP_LOG_DEBUG,a,b)

#ifdef __cplusplus
  }
#endif

#endif  /* !SNAPLOG_INCLUDED    */
