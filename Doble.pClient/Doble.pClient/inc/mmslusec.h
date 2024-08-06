/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,       */
/*                    2003 - 2013, All Rights Reserved.                 */
/*                                                                      */
/* MODULE NAME : mmslusec.h                                             */
/* PRODUCT(S)  : MMS-EASE Lite with LITESECURE                          */
/*                                                                      */
/* MODULE DESCRIPTION :                                                 */
/*              Prototypes for Security related user functions.         */
/*                                                                      */
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :                            */
/*                                                                      */
/* MODIFICATION LOG :                                                   */
/*  Date     Who   Rev                  Comments                        */
/* --------  ---  ------   -------------------------------------------  */
/* 10/08/13  EJV  Added ulCheckCertExpirationAll proto.                 */
/* 09/12/13  EJV  Added Ex functions.                                   */
/* 06/12/13  EJV  Moved S_SEC_CERT_STATUS to ssec.h.                    */
/* 07/12/10  EJV           Merged with mms_usec.h:                      */
/*                           added S_SEC_CERT_STATUS struct;            */
/*                           added ulUpdateCertRevocationList           */
/*                           added ulCheckCertExpiration                */
/*                           added ulPrintSecConnInfo                   */
/*                           ulSetSecurityCalling: rpl S_SEC_LOC_AR and */
/*                             S_SEC_REM_AR with S_SEC_LOC_REM_AR.      */
/* 01/20/04  EJV     03    ulFreeAssocSecurity: del cc param.           */
/* 01/06/04  EJV     02    Merged with  MMS-EASE mms_usec.h.            */
/* 07/08/03  EJV     01    Adopted from MMS-EASE mms_usec.h.            */
/************************************************************************/
#ifndef MMS_USEC_H
#define MMS_USEC_H

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------*/
/* NOTE: The MMS-EASE Lite Secured applications (Client, Server)        */
/*       are designed to work with Security Toolkit for MMS-EASE Lite   */
/*       (LITESECURE-000-001).                                          */
/*----------------------------------------------------------------------*/

#if defined(S_SEC_ENABLED)

#include "acseauth.h"
#include "ssec.h"


ST_RET ulInitSecurity (ST_VOID);
ST_RET ulEndSecurity (ST_VOID);

ST_RET ulSetSecurityCalling (ST_CHAR            *locArName,
                            ST_CHAR            *remArName,
                            S_SEC_LOC_REM_AR   **locArSecOut,
                            S_SEC_LOC_REM_AR   **remArSecOut,
                            ACSE_AUTH_INFO     *locAuthInfo,
                            S_CERT_CTRL       **maceCertOut,
                            S_SEC_ENCRYPT_CTRL *encryptCtrl);
ST_RET ulSetSecurityCallingEx (ST_CHAR          *locArName,
                             ST_CHAR            *remArName,
                             S_SEC_LOC_REM_AR  **locArSecOut,
                             S_SEC_LOC_REM_AR  **remArSecOut,
                             ACSE_AUTH_INFO     *locAuthInfo,
                             S_CERT_CTRL       **maceCertOut,
                             S_SEC_ENCRYPT_CTRL *encryptCtrl,
                             S_SEC_CERT_STATUS  *certStatus);

ST_RET ulCheckSecureAssocConf (MVL_NET_INFO *cc, S_SEC_AUTHCHK_RSLT *secChk);
ST_RET ulCheckSecureAssocConfEx (MVL_NET_INFO *cc, S_SEC_AUTHCHK_RSLT *secChk, S_SEC_CERT_STATUS  *certStatus);

ST_RET ulCheckSecureAssocInd (MVL_NET_INFO *cc, ACSE_AUTH_INFO *rspAuthInfo, S_SEC_AUTHCHK_RSLT *secChk);
ST_RET ulCheckSecureAssocIndEx (MVL_NET_INFO *cc, ACSE_AUTH_INFO *rspAuthInfo, S_SEC_AUTHCHK_RSLT *secChk,
                                ST_CHAR *locArName, ST_CHAR *remArName, S_SEC_CERT_STATUS  *certStatus);

ST_RET ulFreeAssocSecurity (ACSE_AUTH_INFO *authInfo);
ST_RET ulCheckSecurityConfiguration (ST_VOID);
ST_RET ulUpdateCertRevocationList (ST_VOID);
ST_RET ulCheckCertExpirationAll (ST_VOID);
ST_RET ulCheckCertExpiration (MVL_NET_INFO *cc, ST_BOOLEAN  bCheckNow, S_SEC_CERT_STATUS *certStatus);
ST_RET ulPrintSecConnInfo (MVL_NET_INFO *cc, S_SEC_CERT_STATUS *certStatus);


#endif /* defined(S_SEC_ENABLED) */

#ifdef __cplusplus
}
#endif

#endif  /* MMS_USEC_H */
