/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*      (c) Copyright Systems Integration Specialists Company, Inc.,    */
/*      1997 - 2018, All Rights Reserved                                */
/*                                                                      */
/*                      PROPRIETARY AND CONFIDENTIAL                    */
/*                                                                      */
/* MODULE NAME : copp_log.h                                             */
/* PRODUCT(S)  : MOSI Stack (over TP4)                                  */
/*                                                                      */
/* MODULE DESCRIPTION :                                                 */
/*              This header file contains logging macros for COPP.      */
/*      Note:   To compile logging functions use the DEBUG_SISCO define */
/*              in the project make file.                               */
/*                                                                      */
/*                                                                      */
/* MODIFICATION LOG :                                                   */
/*  Date     Who   Rev                  Comments                        */
/* --------  ---  ------   -------------------------------------------  */
/* 05/31/18  EJV  Added COPP_LOG_ENCF, COPP_LOG_DECF filtering macros.  */
/* 03/10/14  EJV  Changed COPP_LOG_ENCH, COPP_LOG_DECH to *_HEX mask.   */
/* 11/01/07  EJV    05     Updated COPP_LOG_* macros to use SLOG_n.     */
/* 09/13/99  MDE    04     Added SD_CONST modifiers                     */
/* 10/08/98  MDE    03     Migrated to updated SLOG interface           */
/* 05/27/97  JRB  7.00     MMSEASE 7.0 release.                         */
/* 03/19/97  JRB    02     General cleanup.                             */
/* 02/20/97  EJV    01     Created                                      */
/************************************************************************/

#ifndef COPP_LOG_INCLUDED
#define COPP_LOG_INCLUDED

#include "slog.h"

#ifdef __cplusplus
extern "C" {
#endif

/* NOTE: COPP log masks and acse_debug_sel are defined in acse2usr.h */

#if defined(DEBUG_SISCO)

extern SD_CONST ST_CHAR *SD_CONST _copp_err_logstr;
extern SD_CONST ST_CHAR *SD_CONST _copp_dec_logstr;
extern SD_CONST ST_CHAR *SD_CONST _copp_dec_hex_logstr;
extern SD_CONST ST_CHAR *SD_CONST _copp_enc_logstr;
extern SD_CONST ST_CHAR *SD_CONST _copp_enc_hex_logstr;

  /*--------------------------------------------*/
  /* hard errors logging                        */
  /*--------------------------------------------*/

  #define COPP_LOG_ERR0(a) \
        SLOG_0 (acse_debug_sel & COPP_LOG_ERR,_copp_err_logstr,a)

  #define COPP_LOG_ERR1(a,b) \
        SLOG_1 (acse_debug_sel & COPP_LOG_ERR,_copp_err_logstr,a,b)

  #define COPP_LOG_ERR2(a,b,c) \
        SLOG_2 (acse_debug_sel & COPP_LOG_ERR,_copp_err_logstr,a,b, c)

  #define COPP_LOG_ERR3(a,b,c,d) \
        SLOG_3 (acse_debug_sel & COPP_LOG_ERR,_copp_err_logstr,a,b, c,d)

  /*--------------------------------------------*/
  /* PS-user req and rsp logging                */
  /*--------------------------------------------*/

  #define COPP_LOG_ENC0(a) \
        SLOG_0 (acse_debug_sel & COPP_LOG_ENC,_copp_enc_logstr,a)

  #define COPP_LOG_ENC1(a,b) \
        SLOG_1 (acse_debug_sel & COPP_LOG_ENC,_copp_enc_logstr,a,b)

  #define COPP_LOG_ENC2(a,b,c) \
        SLOG_2 (acse_debug_sel & COPP_LOG_ENC,_copp_enc_logstr,a,b,c)

  #define COPP_LOG_ENC3(a,b,c,d) \
        SLOG_3 (acse_debug_sel & COPP_LOG_ENC,_copp_enc_logstr,a,b,c,d)

  #define COPP_LOG_ENC4(a,b,c,d,e) \
        SLOG_4 (acse_debug_sel & COPP_LOG_ENC,_copp_enc_logstr,a,b,c,d,e)

  /* continuation macros */
  #define COPP_LOG_ENCC0(a) \
        SLOGC_0 (acse_debug_sel & COPP_LOG_ENC,a)

  #define COPP_LOG_ENCC1(a,b) \
        SLOGC_1 (acse_debug_sel & COPP_LOG_ENC,a,b)

  /* hex logging */
  #define COPP_LOG_ENCH(a,b) \
        SLOGH (acse_debug_sel & COPP_LOG_ENC_HEX,a,b)

/* filtering macros */

  #define COPP_LOG_ENCF0(log_enable,a) \
        SLOG_0 ((!sLogCtrl->filtered_logging_enable || log_enable) && (acse_debug_sel & COPP_LOG_ENC),_copp_enc_logstr,a)

  #define COPP_LOG_ENCF1(log_enable,a,b) \
        SLOG_1 ((!sLogCtrl->filtered_logging_enable || log_enable) && (acse_debug_sel & COPP_LOG_ENC),_copp_enc_logstr,a,b)

  #define COPP_LOG_ENCF2(log_enable,a,b,c) \
        SLOG_2 ((!sLogCtrl->filtered_logging_enable || log_enable) && (acse_debug_sel & COPP_LOG_ENC),_copp_enc_logstr,a,b,c)

  #define COPP_LOG_ENCF3(log_enable,a,b,c,d) \
        SLOG_3 ((!sLogCtrl->filtered_logging_enable || log_enable) && (acse_debug_sel & COPP_LOG_ENC),_copp_enc_logstr,a,b,c,d)

  #define COPP_LOG_ENCF4(log_enable,a,b,c,d,e) \
        SLOG_4 ((!sLogCtrl->filtered_logging_enable || log_enable) && (acse_debug_sel & COPP_LOG_ENC),_copp_enc_logstr,a,b,c,d,e)

  /* continuation macros */
  #define COPP_LOG_ENCFC0(log_enable,a) \
        SLOGC_0 ((!sLogCtrl->filtered_logging_enable || log_enable) && (acse_debug_sel & COPP_LOG_ENC),a)

  #define COPP_LOG_ENCFC1(log_enable,a,b) \
        SLOGC_1 ((!sLogCtrl->filtered_logging_enable || log_enable) && (acse_debug_sel & COPP_LOG_ENC),a,b)

  /* hex logging */
  #define COPP_LOG_ENCFH(log_enable,a,b) \
        SLOGH ((!sLogCtrl->filtered_logging_enable || log_enable) && (acse_debug_sel & COPP_LOG_ENC_HEX),a,b)


  /*--------------------------------------------*/
  /* PS-user ind and cnf logging                */
  /*--------------------------------------------*/

  #define COPP_LOG_DEC0(a) \
        SLOG_0 (acse_debug_sel & COPP_LOG_DEC,_copp_dec_logstr,a)

  #define COPP_LOG_DEC1(a,b) \
        SLOG_1 (acse_debug_sel & COPP_LOG_DEC,_copp_dec_logstr,a,b)

  #define COPP_LOG_DEC2(a,b,c) \
        SLOG_2 (acse_debug_sel & COPP_LOG_DEC,_copp_dec_logstr,a,b,c)

  #define COPP_LOG_DEC3(a,b,c,d) \
        SLOG_3 (acse_debug_sel & COPP_LOG_DEC,_copp_dec_logstr,a,b,c,d)

  #define COPP_LOG_DEC4(a,b,c,d,e) \
        SLOG_4 (acse_debug_sel & COPP_LOG_DEC,_copp_dec_logstr,a,b,c,d,e)

  /* continuation macros */
  #define COPP_LOG_DECC0(a) \
        SLOGC_0 (acse_debug_sel & COPP_LOG_DEC,a)

  #define COPP_LOG_DECC1(a,b) \
        SLOGC_1 (acse_debug_sel & COPP_LOG_DEC,a,b)

  /* hex logging */
  #define COPP_LOG_DECH(a,b) \
        SLOGH (acse_debug_sel & COPP_LOG_DEC_HEX,a,b)

/* filtering macros */

  #define COPP_LOG_DECF0(log_enable,a) \
        SLOG_0 ((!sLogCtrl->filtered_logging_enable || log_enable) && (acse_debug_sel & COPP_LOG_DEC),_copp_dec_logstr,a)

  #define COPP_LOG_DECF1(log_enable,a,b) \
        SLOG_1 ((!sLogCtrl->filtered_logging_enable || log_enable) && (acse_debug_sel & COPP_LOG_DEC),_copp_dec_logstr,a,b)

  #define COPP_LOG_DECF2(log_enable,a,b,c) \
        SLOG_2 ((!sLogCtrl->filtered_logging_enable || log_enable) && (acse_debug_sel & COPP_LOG_DEC),_copp_dec_logstr,a,b,c)

  #define COPP_LOG_DECF3(log_enable,a,b,c,d) \
        SLOG_3 ((!sLogCtrl->filtered_logging_enable || log_enable) && (acse_debug_sel & COPP_LOG_DEC),_copp_dec_logstr,a,b,c,d)

  #define COPP_LOG_DECF4(log_enable,a,b,c,d,e) \
        SLOG_4 ((!sLogCtrl->filtered_logging_enable || log_enable) && (acse_debug_sel & COPP_LOG_DEC),_copp_dec_logstr,a,b,c,d,e)

  /* continuation macros */
  #define COPP_LOG_DECFC0(log_enable,a) \
        SLOGC_0 ((!sLogCtrl->filtered_logging_enable || log_enable) && (acse_debug_sel & COPP_LOG_DEC),a)

  #define COPP_LOG_DECFC1(log_enable,a,b) \
        SLOGC_1 ((!sLogCtrl->filtered_logging_enable || log_enable) && (acse_debug_sel & COPP_LOG_DEC),a,b)

  /* hex logging */
  #define COPP_LOG_DECFH(log_enable,a,b) \
        SLOGH ((!sLogCtrl->filtered_logging_enable || log_enable) && (acse_debug_sel & COPP_LOG_DEC_HEX),a,b)


#else

  /*--------------------------------------------*/
  /* hard errors logging                        */
  /*--------------------------------------------*/

  #define COPP_LOG_ERR0(a)
  #define COPP_LOG_ERR1(a,b)
  #define COPP_LOG_ERR2(a,b,c)
  #define COPP_LOG_ERR3(a,b,c,d)

  /*--------------------------------------------*/
  /* PS-user req and rsp logging                */
  /*--------------------------------------------*/

  #define COPP_LOG_ENC0(a)
  #define COPP_LOG_ENC1(a,b)
  #define COPP_LOG_ENC2(a,b,c)
  #define COPP_LOG_ENC3(a,b,c,d)
  #define COPP_LOG_ENC4(a,b,c,d,e)
  #define COPP_LOG_ENCC0(a)
  #define COPP_LOG_ENCC1(a,b)
  #define COPP_LOG_ENCH(a,b)

  #define COPP_LOG_ENCF0(a)
  #define COPP_LOG_ENCF1(a,b)
  #define COPP_LOG_ENCF2(a,b,c)
  #define COPP_LOG_ENCF3(a,b,c,d)
  #define COPP_LOG_ENCF4(a,b,c,d,e)
  #define COPP_LOG_ENCFC0(a)
  #define COPP_LOG_ENCFC1(a,b)
  #define COPP_LOG_ENCFH(a,b)

  /*--------------------------------------------*/
  /* PS-user ind and cnf logging                */
  /*--------------------------------------------*/

  #define COPP_LOG_DEC0(a)
  #define COPP_LOG_DEC1(a,b)
  #define COPP_LOG_DEC2(a,b,c)
  #define COPP_LOG_DEC3(a,b,c,d)
  #define COPP_LOG_DEC4(a,b,c,d,e)
  #define COPP_LOG_DECC0(a)
  #define COPP_LOG_DECC1(a,b)
  #define COPP_LOG_DECH(a,b)

  #define COPP_LOG_DECF0(a)
  #define COPP_LOG_DECF1(a,b)
  #define COPP_LOG_DECF2(a,b,c)
  #define COPP_LOG_DECF3(a,b,c,d)
  #define COPP_LOG_DECF4(a,b,c,d,e)
  #define COPP_LOG_DECFC0(a)
  #define COPP_LOG_DECFC1(a,b)
  #define COPP_LOG_DECFH(a,b)

#endif


#ifdef __cplusplus
}
#endif

#endif  /* end of 'already included'    */
