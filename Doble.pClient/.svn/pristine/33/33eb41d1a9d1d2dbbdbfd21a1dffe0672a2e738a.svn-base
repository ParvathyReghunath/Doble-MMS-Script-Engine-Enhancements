/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,       */
/*          2003 - 2014, All Rights Reserved                            */
/*                                                                      */
/* MODULE NAME : ssleInt.h                                              */
/* PRODUCT(S)  : Sisco MMS Security Toolkit                             */
/*                                                                      */
/* MODULE DESCRIPTION :                                                 */
/*                                                                      */
/* This is the Sisco SSL Toolkit internal header file.                  */
/* Contains prototypes of functions to be used internally by the SSL    */
/* Toolkit API.                                                         */
/*                                                                      */
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :                            */
/*                                                                      */
/* MODIFICATION LOG :                                                   */
/*  Date     Who   Rev                     Comments                     */
/* --------  ---  ------   -------------------------------------------  */
/* 12/11/14  EJV  SNAP_LITE_THR use event file on Win & Linux/UNIX.     */
/* 06/03/11  EJV           Added back MAP30_ACSE (secManLck* funs).     */
/* 10/28/10  EJV           Removed internal code.                       */
/* 06/08/10  EJV           Chg funct ptrs like normal.                  */
/* 01/28/09  EJV    07     UNIX,Linux _ssleInitEvLogging: added params  */
/* 01/05/09  EJV    06     Added _verifyMACECertificateCallback         */
/*                         Mv ssleLog proto from ssleMain.c here.       */
/*                         Added extern sslUsrLog fun ptr for certtool. */
/* 01/18/08  EJV    05     Mv secManLck* funcs here from ssec.h.        */
/* 12/05/07  EJV    04     Mv callback function pointers to ssleMain.c. */
/* 10/18/03  ASK    03     Add sslUsrCrlLoad fun ptr.                   */
/* 05/06/03  ASK    02     Add sslUsrLog fun ptr.                       */
/* 08/29/02  ASK    01     Created                                      */
/************************************************************************/

#ifndef SSLE_INT_INCLUDED
#define SSLE_INT_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

/* defined in ssleMain.c */
ST_VOID ssleLog (LOG_CTRL *lc,
                 SD_CONST ST_CHAR *timeStr,
                 SD_CONST ST_INT logType,
                 SD_CONST ST_CHAR *SD_CONST logTypeStr,
                 SD_CONST ST_CHAR *SD_CONST sourceFile,
                 SD_CONST ST_INT lineNum,
                 SD_CONST ST_INT bufLen,
                 SD_CONST ST_CHAR *buf);
extern ST_VOID (*sslUsrLog) (S_SEC_EVENT *evt); /* expose to use in sslecert_unix.c     */

/* Functions preceeded with '_' are to be used internally by the SSL BB */
/* API only.                                                            */

DH *_get_dh512(void);
ST_INT _passwdCallback(ST_CHAR *buf, ST_INT size, ST_INT flag, ST_VOID *userdata);
ST_INT _verifyCertificateCallback(ST_INT ok, X509_STORE_CTX *store);
ST_INT _verifyMACECertificateCallback(ST_INT ok, X509_STORE_CTX *store);
ST_VOID *_getCertificate(X509 *cert);
ST_VOID _getExtensions(ST_VOID *peerCert, X509 *cert);

#if defined(_WIN32) && !defined(SNAP_LITE_THR)
ST_RET _ssleInitEvLogging(ST_VOID);
#else
/* UNIX/Linux                                                           */
ST_RET _ssleInitEvLogging(ST_CHAR *logFileName, ST_ULONG logFileSize);
#endif
ST_RET _ssleEndEvLogging (ST_VOID);

#if defined(MAP30_ACSE)
/* Software Lock access */
ST_RET  secManLckStart (ST_VOID);
ST_VOID secManLckEnd (ST_VOID);
ST_RET  secManLckCheck (ST_VOID);
#endif /* defined(MAP30_ACSE) */


/************************************************************************/
#ifdef __cplusplus
  }
#endif

#endif /* SSLE_INT_INCLUDED */
/************************************************************************/




