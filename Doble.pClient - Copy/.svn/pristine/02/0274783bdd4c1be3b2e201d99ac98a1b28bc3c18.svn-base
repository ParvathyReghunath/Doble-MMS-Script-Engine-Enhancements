/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,       */
/*          2003, All Rights Reserved                                   */
/*                                                                      */
/* MODULE NAME : ssleCon.c                                              */
/* PRODUCT(S)  : Sisco MMS Security Toolkit                             */
/*                                                                      */
/* MODULE DESCRIPTION :                                                 */
/*                                                                      */
/* Handles SSL Connections and Connection Requests.                     */
/*                                                                      */
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :                            */
/*                                                                      */
/* MODIFICATION LOG :                                                   */
/*  Date     Who   Rev                     Comments                     */
/* --------  ---  ------   -------------------------------------------  */
/* 02/12/04  ASK    02     Fix missing closing comment                  */
/* 03/01/03  ASK    01     Created                                      */
/************************************************************************/

#ifndef SSLE_CONN_INCLUDED
#define SSLE_CONN_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "openssl/ssl.h"

#define SSLE_CONNECT_FAILURE -1
#define SSLE_CONNECT_PENDING  0
#define SSLE_CONNECT_SUCCESS  1

ST_INT _ssleConnect(SSL *ssl, ST_INT mode);

/************************************************************************/
#ifdef __cplusplus
  }
#endif

#endif /* SSLE_CONN_INCLUDED */
/************************************************************************/
