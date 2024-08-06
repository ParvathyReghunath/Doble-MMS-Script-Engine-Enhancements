/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*      (c) Copyright Systems Integration Specialists Company, Inc.,    */
/*      2018-2018 All Rights Reserved                                   */
/*                                                                      */
/* MODULE NAME : userslip.h                                             */
/* PRODUCT(S)  : MMS Lite                                               */
/*                                                                      */
/* MODULE DESCRIPTION :                                                 */
/*      User functions to interface to "slip".                          */
/*                                                                      */
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :                            */
/*                                                                      */
/* MODIFICATION LOG :                                                   */
/*  Date     Who  Comments                                              */
/* --------  ---  ----------------------------------------------------  */
/* 02/12/18  JRB  Created.                                              */
/************************************************************************/
#ifndef USERSLIP_INCLUDED
#define USETSLIP_INCLUDED

#include "slip.h"
#include "scl.h"

#ifdef __cplusplus
extern "C" {
#endif

ST_VOID *userSlipSendContextCreate (ST_ULONG IP_IGMPv3Src, ST_ULONG IP_dest, ST_CHAR *networkDevice);
ST_VOID *userSlipReceiveContextCreate (SCL_INFO *scl_info,
  ST_CHAR *iedName,     /* IED name of Subscriber               */
  ST_CHAR *apName,      /* AccessPoint name of Subscriber       */
  ST_CHAR *networkDevice,
  SLIP_RECEIVE_MESSAGE_HANDLER handler); /* callback when msg received   */
ST_RET userSlipReceiveContextModifyMulticastFilter (ST_VOID *pReceiveContext, ST_ULONG IP_IGMPv3Src, ST_ULONG IP_dest);

ST_VOID userSlipReceiveMessage (SLIP_RECEIVE_MESSAGE *pReceiveMessage);

/* userSlipReceiveMessage calls this to process RSV */
ST_VOID rsvReceiveMessage (SLIP_RECEIVE_MESSAGE *pReceiveMessage);
/* userSlipReceiveMessage calls this to process RGOOSE */
ST_VOID rgooseReceiveMessage (SLIP_RECEIVE_MESSAGE *pReceiveMessage);

#ifdef __cplusplus
}
#endif
#endif          /* included     */

