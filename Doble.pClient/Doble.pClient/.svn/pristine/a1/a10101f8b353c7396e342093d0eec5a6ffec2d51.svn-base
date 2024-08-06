/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,       */
/*          2003 - 2015, All Rights Reserved                            */
/*                                                                      */
/* MODULE NAME : sslEngine.h                                            */
/* PRODUCT(S)  : Sisco MMS Security Toolkit                             */
/*                                                                      */
/* MODULE DESCRIPTION :                                                 */
/*                                                                      */
/* This is the Sisco SSL Engine DLL main header.                        */
/* Contains main structures and functions for interfacing with the      */
/* Sisco SSL API as well as various miscellaneous prototypes.           */
/*                                                                      */
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :                            */
/*                                                                      */
/* MODIFICATION LOG :                                                   */
/*  Date     Who   Rev                     Comments                     */
/* --------  ---  ------   -------------------------------------------  */
/* 08/28/15  EJV  UNIX/Linux:add ssleWriteCertsXml2, ssleWriteCertsXmlExx.*/
/* 03/07/15  EJV  Added code to handle certificate extraction for       */
/*                MMS-LITE-SECURE (defined SNAP_LITE_THR).              */
/* 05/16/14  EJV  Added if !defined(SNAP_LITE_THR) for DLL functions.   */
/* 05/16/13  EJV  Added ssleWriteCertsXml2 and ssleWriteCertsXmlExx.    */
/* 12/28/12  EJV  Added extern _sSecMajorVer, _sSecMinorVer, _bUserApp. */
/* 03/15/12  EJV           Add ssleSetLogFun proto.                     */
/* 02/18/12  EJV           _WIN32: added ssleInstall.                   */
/* 05/04/11  EJV           SSLE_CONN_CTRL: added alertLevel, alertType  */
/*                           to detect rekey problems.                  */
/* 12/17/10  EJV           Rem !defined(MMS_LITE) from DLL export funcs.*/
/* 10/25/10  EJV           SSLE_CFG: added allowUnsafeRenegotiations.   */
/* 04/28/10  JRB           Del ifdef SSL_ENGINE_DUMMY. Use (ST_VOID *)  */
/*                         so OpenSSL files not needed to compile.      */
/* 11/17/09  JRB    29     Chg funct ptrs like normal.                  */
/* 04/15/09  EJV    28     Add support for multiple SSL certs (& reload)*/
/*                         ssleGetCipherInUse: added params, chg return.*/
/*                         ssleGetCipherInUseStr added new fun.         */
/*                         SSLE_CONN_CTRL: chg cfgId to remCfgId,       */
/*                                                added locCfgId.       */
/*                         SSLE_CERT_NAME_LIST: don't alloc certId.     */
/*                         ssleDecodeMaceBlob: added certType param.    */
/* 03/30/09  EJV           Added ssleDuplicateCertId, ssleFreeCertId.   */
/*                         SSLE_CERT_NAME_LIST: chg certId to struct.   */
/* 03/04/09  EJV    27     Removed ssleCheckPeerCert.                   */
/*                         Added ssleCheckForRevokedSslCerts.           */
/*                         Added ssleVerifySslCert,ssleVerifySslConCert.*/
/*                         Added ssleFreeCertInfo.                      */
/*                         ssleDecodeMaceClob: added certType param.    */
/*                         ssleNewConnCtrl: chg return, added param.    */
/* 09/10/08  EJV    26     Add ssleWriteCertsXmlEx to log from SSLE DLL.*/
/*                         ssleReloadCfg: chg *caCert to SSLE_CFG *cfg  */
/*                         SSLE_CFG: rem crlCheckTime (not needed there)*/
/*                         Added ssleShm* functions.                    */
/*                         ssleEventLog: chg S_CERT_INFO* to S_CERT_ID* */
/* 08/21/08  EJV    25     Rem SSLE_THREAD_ARGS. Cleanup.               */
/*                         SSLE_CFG: del obsolete certificateCB, crlCB  */
/*                         SSLE_HANDLE: del obsolete crlEvent.          */
/* 05/28/08  EJV    24     Merged with non-Marben code.                 */
/* 01/15/07  EJV    23     SSLE_CONN_CTRL: chg mutex to obsolete2_mutex.*/
/* 01/04/06  EJV    22     Added DLLs incompatibility comments.         */
/* 12/19/05  EJV    21     Chg ssleCheckPeerCert param to (ST_VOID *)   */
/*                         SSLE_CONN_CTRL: due to changed ST_MUTEX_SEM  */
/*                          for compatibility with older apps renamed   */
/*                          mutex struct to obsolete_mutex; added *mutex*/
/* 10/13/05  EJV    20     Chg ssleCheckPeerCert param to (GEN_SOCK *)  */
/*                         Moved SEC_CTRL and defs from genssl.c        */
/* 08/24/04  EJV    19     Chg from WIN32 to _WIN32.                    */
/* 02/12/04  ASK    18     Added event semaphores to control CRL thread */
/*                         Added MACE cert list struct when caching     */
/*                         Changes to compile on AIX                    */
/* 10/23/03  ASK    17     Remove SSLE_CFG from SSLE_HANDLE             */
/* 10/20/03  ASK    16     Remove logmasks from SSLE_CFG                */
/* 10/16/03  ASK    15     Added ssleUserEnd proto.                     */
/* 10/15/03  ASK    14     Added #ifdef in SSLE_THREAD_ARGS.            */
/* 10/14/03  ASK    13     Additions for CRL checking at runtime        */
/* 09/04/03  EJV    12     Moved SSLE_LOG_* masks to ssleLog.h          */
/* 08/21/03  ASK    11     Remove failed rekey statistic. Chg Verify    */
/*                         time proto.                                  */
/* 07/15/03  ASK    10     Added ssleReloadCfg proto.                   */
/* 06/18/03  ASK    09     Add some statistics to SSLE_CONN_CTRL        */
/* 06/13/03  ASK    08     Added ssleDoRekey proto. Added rekey states. */
/*                         Add mutex in SSLE_CONN_CTRL, inc glbsem.h    */
/* 06/13/03  MDE    07     Added MMS_LITE                               */
/* 05/23/03  ASK    06     Added ssleWriteCertsXml proto. Added         */
/*                         usrNetAddr to SSLE_CONN_CTRL to log Ip addr. */
/* 05/07/03  ASK    05     Added SSL_ENGINE_DUMMY for NUL DLL.          */
/* 05/06/03  ASK    04     Added ssleCbLog function ptr                 */
/* 05/01/03  ASK    03     Added MACE and user init function protos     */
/* 04/11/03  ASK    02     Added ssleGetPeerCert and ssleGetCipherInUse */
/*                         protos. Misc cleanup.                        */
/* 08/21/02 MDE/ASK 01     Created                                      */
/************************************************************************/

#ifndef SSLE_ENGINE_INCLUDED
#define SSLE_ENGINE_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

/* References to page numbers refer to the book, "Network Security with */
/* OpenSSL", first edition, 2002.                                       */
/* NOTE: This file does not require any "OpenSSL" definitions.          */
/*       Some paremeters may be cast to "OpenSSL" types in secure code. */
#include "ssec.h"
#include "glbsem.h"

/* Configuration information */
#define SSLE_MODE_CALLING 1     /* same as GS_ROLE_CALLING      */
#define SSLE_MODE_CALLED  3     /* same as GS_ROLE_CALLED       */

/* Context state */
#define SSLE_STATE_IDLE         1 /* When the context has just been created                       */
#define SSLE_STATE_STARTING     2 /* When the context is doing the handshake                      */
#define SSLE_STATE_ACTIVE       3 /* Handshake is complete, now servicing communications normally */
#define SSLE_STATE_REKEYING     4 /* Renegotiating connection (like another handshake)            */
#define SSLE_STATE_ERROR        5 /* For an unrecoverable errors                                  */
/* update genssl.c when adding/changing STATE */

/* State Change Reasons */
#define SSLE_STATE_REASON_CONTEXT_CREATED                0 /* Set to idle        */
#define SSLE_STATE_REASON_INITIALIZED_AS_CLIENT          1 /* idle -> starting   */
#define SSLE_STATE_REASON_INITIALIZED_AS_SERVER          2 /* idle -> starting   */
#define SSLE_STATE_REASON_USER_DID_NOT_RETURN_SUCCESS    3 /* Set to error       */
#define SSLE_STATE_REASON_IN_ERROR_STATE                 4 /* Keep in error      */
#define SSLE_STATE_REASON_CONNECTION_ACTIVE              5 /* starting -> active */
#define SSLE_STATE_REASON_INTERNAL_ERROR                 6 /* Set to error       */
#define SSLE_STATE_REASON_END_CONTEXT                    7 /* active -> idle     */
#define SSLE_STATE_REASON_REKEYING                       8
#define SSLE_STATE_REASON_REKEY_COMPLETE                 9
/* update genssl.c when adding/changing state change reason */

/* Security control states (moved from genssl.c here) */
#define SEC_STATE_START_CALLING 1
#define SEC_STATE_START_CALLED  2
#define SEC_STATE_ACTIVE        3
#define SEC_STATE_REKEYING      4

typedef struct
  {
  ST_INT currState;
  ST_BOOLEAN partnerCertOk;

  } SSLE_STATE;


/* Connection Control Structure */
typedef struct
  {
  /* NOTE: "ssl" points to SSL struct. Cast to (SSL *) when needed.     */
  ST_VOID *ssl;         /* Internal use */

  /* some statistics */
  ST_ULONG statBytesEncrypted;
  ST_ULONG statBytesDecrypted;
  ST_ULONG statRekeySuccess;


  ST_INT connectRole;
  SSLE_STATE  state;

  ST_VOID *usrTpCtxt;   /* User transport context */
  ST_CHAR  usrNetAddr[64];     /* User peer address (used in logging from ssle*) */

  ST_UINT32  locCfgId;  /* save the local  SSL certificate cfgId (computed 32-bit CRC on certId) */
  ST_UINT32  remCfgId;  /* save the remote SSL certificate cfgId (computed 32-bit CRC on certId) */

  ST_UINT   alertLevel; /* OpenSSL Alert level: (1) Warning or (2) Fatal        */
  ST_UINT   alertType;  /* OpenSSL Alert type: see ssl.h                        */
  } SSLE_CONN_CTRL;

typedef struct
  {
  /* User callbacks */
  /* NOTE: stateCB & handshakeCB used for SNAP only     */
  ST_VOID (*stateCB) (SSLE_CONN_CTRL *sslBbConnCtrl, ST_INT reason);
  ST_RET  (*handshakeCB) (SSLE_CONN_CTRL *sslBbConnCtrl, ST_INT dataLen, ST_CHAR *data);
  ST_VOID (*logCB)(S_SEC_EVENT *evt);

  /* Trusted CA List */
  S_CERT_CTRL *caList;

  S_CERT_CTRL *locCertList;             /* added to set the validity during init and reload     */
  S_CERT_CTRL *remCertList;             /* added to set the validity during init and reload     */

  /* Local Certificate used for SSL */
  S_CERT_CTRL  *sslLocalCertCtrlArr[S_MAX_LOCAL_SSL_CERTS];     /* array of SSL certs - one will be loaded to OpenSSL   */
  ST_UINT       sslLocalCertCtrlArrCnt;
  S_CERT_CTRL **sslLocalCertCtrl;       /* return ptr to SSL cert loaded into OpenSSL           */

  /* Cipher Suite List */
  ST_INT numCiphers;                    /* SNAP only            */
  ST_INT *cipherList;                   /* SNAP only            */

  ST_CHAR  *instPath;                   /* UNIX: ptr to buff holding InstallPath*/
                                        /* needed by SSL Engine to find license */
  ST_UINT  allowUnsafeRenegotiations;   /* applies to OpenSSL 0.9.8m and above  */

  /* UNIX, Linux: Security Event logging, if not config then app log file is used */
  ST_INT   secEventLogFileSize;
  ST_CHAR *secEventLogFileName;
  } SSLE_CFG;

/* Handle used to create new connection controls */
typedef struct
  {
  /* NOTE: "ssl_ctx" points to SSL_CTX struct. Cast to (SSL_CTX *) when needed.*/
  ST_VOID *ssl_ctx;     /* Internal use */
  ST_VOID *usr;         /* For user's use - curr not used */

  } SSLE_HANDLE;

/* Added to a list of MACE Certificates */
typedef struct
  {
  DBL_LNK l;

  S_CERT_ID    certId;
  S_CERT_BLOB *certBlob;
  /* NOTE: "pkey" points to EVP_PKEY struct. Cast to (EVP_PKEY *) when needed.*/
  ST_VOID *pkey;

  } SSLE_CERT_NAME_LIST;



/* Buffer sizes */
#define SEC_RX_BUF_SIZE 2048 /* 2k buf size */
#define SEC_TX_BUF_SIZE 2048 /* 2k buf size */

/* Security control attached to a GEN_SOCK struct */
typedef struct tag_SEC_CTRL
  {
  ST_INT          secState;

  ST_CHAR         secRxBuf[SEC_RX_BUF_SIZE];    /* Rx buf */
  ST_CHAR         secTxBuf[SEC_TX_BUF_SIZE];    /* Tx buf */

  ST_CHAR        *secTxPtr;                     /* ptr to data to be sent */
  ST_INT          secTxLen;                     /* len of data to be sent */

  ST_DOUBLE       lastRekeyTime;
  SSLE_CONN_CTRL *sslConn;
  } SEC_CTRL;


#if !defined(_WIN32)
/* SSL Engine stores Installation Path (passed from SNAP-Lite or from application) */
ST_RET ssleSetSiscoInstallPath (ST_CHAR *pathBuf);
ST_RET ssleGetSiscoInstallPath (ST_CHAR *pathBuf, ST_INT pathBufSize);
#endif

/* variables allowing selective access to new SSLEngine features for application*/
/* compiled with newer versions of this software                                */
extern ST_BOOLEAN  _bUserApp;           /* 0 if SNAP-Lite, 1 if user application*/
extern ST_UINT32   _sSecMajorVer;       /* set by user app from ssleUserInit    */
extern ST_UINT32   _sSecMinorVer;       /* set by user app from ssleUserInit    */


/* Initialization function */
SSLE_HANDLE* ssleInit (SSLE_CFG *ssleCfg);

/* User Initialization function */
ST_RET ssleUserInit (S_SEC_CONFIG *pSecCfg, ST_VOID (*logCB)(S_SEC_EVENT *evt));

/* Conclusion function */
ST_VOID ssleEnd(SSLE_HANDLE *hSsl);

/* User Conclusion function */
ST_VOID ssleUserEnd (ST_VOID);

/* Allocate & free connection control */
ST_RET ssleNewConnCtrl (SSLE_HANDLE *hSslTk, ST_INT role, ST_VOID *usrSock, ST_CHAR *netAddr, SSLE_CONN_CTRL **sslConnOut);
ST_RET ssleFreeConnCtrl(SSLE_CONN_CTRL *conn);

/* When we're ready to start the handshaking process, we'll make a call
 * to ssleDoHandshake.
 */
ST_RET ssleDoHandshake (SSLE_CONN_CTRL *sslConn,
                        ST_INT dataLen,
                        ST_CHAR *data,
                        ST_BOOLEAN *moreData);

/* User function to encrypt data.
 * clearLen and clearData are the input parameters, the length of the
 * data to be encrypted and the data buffer itself.
 *
 * outData is the output parameter. outLen is the maximum size of this
 * data buffer. The number of bytes encrypted will be returned.
 */
ST_INT ssleEncrypt (SSLE_CONN_CTRL *conn,
                    ST_INT clearLen,
                    ST_UCHAR *clearData,
                    ST_INT outLen,
                    ST_CHAR *outData);

/* User function to decrypt data.
 * requestedLen is the number of bytes to be read and should be less than
 * or equal to the size of the data buffer.
 * The actual number of bytes read will be returned.
 */
ST_INT ssleDecrypt (SSLE_CONN_CTRL *sslConn,
                    ST_INT encryptedLen,
                    ST_UCHAR *encryptedData,
                    ST_INT requestedLen,
                    ST_UCHAR *outData,
                    ST_BOOLEAN *moreData);

/* Renegotiates the SSL */
ST_VOID ssleDoRekey(SSLE_CONN_CTRL *sslConn, ST_INT dataLen, ST_UCHAR *data);

/* Get the current cipher suite. Returns a constant */
ST_RET ssleGetCipherInUse(SSLE_CONN_CTRL *sslConn, ST_INT *cipherSuiteOut, ST_INT *cipherSuiteIdxOut);
ST_RET ssleGetCipherInUseStr(ST_INT cipherSuiteIdx, ST_INT *cipherSuiteOut, ST_CHAR *cipherSuiteStrOut, ST_INT cipherSuiteStrMaxLen);

/* Used to reload the Trusted CAs / Log masks (and reload SSL cert in OpenSSL) */
ST_RET ssleReloadCfg(SSLE_HANDLE *hSsl, SSLE_CFG *ssleCfg);

/* S_CERT_ID operations */
ST_RET ssleDuplicateCertId (S_CERT_ID *certId, S_CERT_ID *certIdNew);
ST_RET ssleFreeCertId      (S_CERT_ID *certId);

/* Get the peer's certificates as an S_CERT_INFO struct */
S_CERT_INFO *ssleGetPeerCert(SSLE_CONN_CTRL *sslConn);
ST_VOID ssleFreeCertInfo(S_CERT_INFO *certInfo);
ST_VOID ssleFreePeerCert(S_CERT_INFO *certInfo);
/* NOTE: "ssl_ctx" points to SSL_CTX struct. Cast to (SSL_CTX *) when needed.*/
/* NOTE: "x509"    points to X509    struct. Cast to (X509 *)    when needed.*/
ST_RET ssleVerifySslCert (ST_VOID *ssl_ctx, ST_VOID *x509, ST_UINT certType, ST_CHAR *ipAddr);
ST_RET ssleVerifySslConCert (SSLE_CONN_CTRL *sslConn, ST_UINT certType);
ST_RET ssleCheckForRevokedSslCerts (ST_VOID *secCtrl);

/* Get the local MACE Blob */
ST_RET ssleGetMaceBlob(S_CERT_ID *localCert, S_CERT_BLOB *certBlob);
/* Decode a MACE Blob */
ST_RET ssleDecodeMaceBlob (S_CERT_BLOB *certBlob, ST_UINT certType, S_CERT_INFO **certInfoOut);
/* Free MACE Blob data buffer */
ST_VOID ssleFreeMaceBlobData(S_CERT_BLOB *certBlob);

/* Sign Time */
ST_RET ssleSignTimeT (S_CERT_ID *localCert, S_SIGNED_TIME *signedTime, time_t timeToSign);
/* Verify signature of signed time */
ST_RET ssleVerifyTimeT (S_CERT_INFO *cert, S_SIGNED_TIME *signedTime, ST_UCHAR *clearTime);

/* Free Signed Time data buffer */
ST_VOID ssleFreeSignedTimeData (S_SIGNED_TIME *signedTime);

/* Logging to Event Viewer via ssleEngine.dll */
ST_VOID ssleEventLog (S_SEC_EVENT *secEvent, S_CERT_ID *certId);

/**** Output the serial number as a string ****/
ST_VOID sslePrintSrlNum (S_CERT_ID *certId, ST_CHAR *outBuf, ST_INT outBufLen);

/* Returns string for the certificate Issuer */
ST_VOID sslePrintIssuer (S_CERT_ID *certId, ST_CHAR *outBuf, ST_INT outBufLen);

/* Returns string for the certificate Subject.                          */
ST_VOID sslePrintSubject (S_CERT_ID *certId, ST_CHAR *outBuf, ST_INT outBufLen);

#if defined(SNAP_LITE_THR)
  ST_RET ssleInstall (ST_BOOLEAN bUser);
  ST_RET ssleWriteCertsXml2 (ST_CHAR *xmlFileName, ST_UINT version);
  ST_RET ssleWriteCertsXmlExx (ST_CHAR *xmlFileName, ST_UINT ssle_log_masks, ST_UINT version);
#else /* !defined(SNAP_LITE_THR) */
  #if defined(_WIN32)
    __declspec(dllexport) ST_RET WINAPI ssleInstall (ST_BOOLEAN bUser);

    /* Export Windows Certificate stores into XML (this is a Visual Basic export )*/
    __declspec(dllexport) ST_RET WINAPI ssleWriteCertsXml (ST_CHAR *xmlFileName);
    __declspec(dllexport) ST_RET WINAPI ssleWriteCertsXml2 (ST_CHAR *xmlFileName, ST_UINT version);
    __declspec(dllexport) ST_RET WINAPI ssleWriteCertsXmlEx (ST_CHAR *xmlFileName, ST_UINT ssle_log_masks);
    __declspec(dllexport) ST_RET WINAPI ssleWriteCertsXmlExx (ST_CHAR *xmlFileName, ST_UINT ssle_log_masks, ST_UINT version);
  #else  /* !defined(_WIN32) */
    /* UINX, Linux,... */
    ST_RET ssleWriteCertsXml (ST_CHAR *xmlFileName);
    ST_RET ssleWriteCertsXml2 (ST_CHAR *xmlFileName, ST_UINT version);
    ST_RET ssleWriteCertsXmlEx (ST_CHAR *xmlFileName, ST_UINT ssle_log_masks);
    ST_RET ssleWriteCertsXmlExx (ST_CHAR *xmlFileName, ST_UINT ssle_log_masks, ST_UINT version);
  #endif /* !defined(_WIN32) */
#endif /* !defined(SNAP_LITE_THR) */

/* Fun to set logging in SSL Engine DLL modules */
ST_RET ssleSetLogFun (ST_VOID (*secLog)(S_SEC_EVENT *evt));

/* Shared Memory functions */
ST_RET ssleShmStart (ST_VOID);
ST_RET ssleShmEnd (ST_VOID);
ST_INT64 ssleGetsecCfgChangedTs (ST_VOID);

/************************************************************************/
#ifdef __cplusplus
}
#endif

#endif /* SSLE_ENGINE_INCLUDED */
/************************************************************************/
