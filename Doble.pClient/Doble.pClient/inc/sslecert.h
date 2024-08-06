/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,       */
/*          2003 - 2010, All Rights Reserved                            */
/*                                                                      */
/* MODULE NAME : ssleCert.h                                             */
/* PRODUCT(S)  : Sisco MMS Security Toolkit                             */
/*                                                                      */
/* MODULE DESCRIPTION :                                                 */
/*                                                                      */
/* This is the Sisco SSL Toolkit certificate header file. This module   */
/* will be responsible for certificate verification and presentation    */
/* routines.                                                            */
/*                                                                      */
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :                            */
/*                                                                      */
/* MODIFICATION LOG :                                                   */
/*  Date     Who   Rev                     Comments                     */
/* --------  ---  ------   -------------------------------------------  */
/* 09/09/10  EJV           _sslesCertConvert: added certType param.     */
/* 04/28/10  JRB           Del ifdef SSL_ENGINE_DUMMY. Use (ST_VOID *)  */
/*                         so OpenSSL files not needed to compile.      */
/* 11/10/09  JRB    10     Add "#if !defined(SSL_ENGINE_DUMMY)".        */
/* 03/11/09  EJV    09     Added _ssleFreeMaceCert.                     */
/*                         Ren _ssleFreeMaceCerts to _ssleFreeMaceCertsCache. */
/* 11/13/08  EJV    08     Add _ssleSetCertListValidity, _ssleGetCertValidity */
/* 07/23/08  EJV    07     Added _ssleWriteCertXml. Rem duplicate proto.*/
/* 12/04/07  EJV    06     Mv ssleLoadCRLists to ssec.h (now in sh lib).*/
/* 08/05/05  ASK    05     Mv fun protos from ssleCert_w32.c            */
/* 11/07/03  ASK    04     Added proto for _ssleFreeMaceCerts           */
/* 10/18/03  ASK    03     Added arg to _ssleLoadCRLists                */
/* 04/10/03  ASK    02     Added _sslesCertConvert                      */
/* 02/19/03  ASK    01     Created                                      */
/************************************************************************/

#ifndef SSLE_CERT_INCLUDED
#define SSLE_CERT_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "glbtypes.h"
#include "sx_defs.h"
#include "ssec.h"

/* NOTE: "ssl_ctx" points to SSL_CTX struct. Cast to (SSL_CTX *) when needed.*/
ST_INT _ssleLoadCACerts(ST_VOID *ssl_ctx, S_CERT_CTRL *caList);
ST_RET _ssleLoadUserCert(ST_VOID *ssl_ctx, S_CERT_ID *localCert);

ST_RET  _ssleFreeMaceCert(SSLE_CERT_NAME_LIST *certEntry);
ST_VOID _ssleFreeMaceCertsCache(ST_VOID);
ST_RET  _ssleSetCertListValidity(S_CERT_CTRL *certList, ST_BOOLEAN bLocal);

/* exists in ssleCert_comn.c */
/* NOTE: "x509"    points to X509    struct. Cast to (X509 *)    when needed.*/
ST_RET       _ssleGetCertValidity (ST_VOID *x509, S_CERT_VALIDITY *validity);
S_CERT_INFO *_sslesCertConvert(ST_VOID *x509, ST_UINT certType);
ST_BOOLEAN _duplicateCert(ST_ULONG err);

ST_RET _ssleWriteCertXml (SX_ENC_CTRL *sxEncCtrl, S_CERT_INFO *sCert);

/************************************************************************/
#ifdef __cplusplus
  }
#endif

#endif /* SSLE_CERT_INCLUDED */
/************************************************************************/
