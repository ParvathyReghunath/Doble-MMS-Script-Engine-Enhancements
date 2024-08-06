/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*      (c) Copyright Systems Integration Specialists Company, Inc.,    */
/*      2003 - 2018, All Rights Reserved                                */
/*                                                                      */
/* MODULE NAME : client.h                                               */
/* PRODUCT(S)  : MMSEASE-LITE                                           */
/*                                                                      */
/* MODULE DESCRIPTION :                                                 */
/*      Defines for sample "client" application.                        */
/*                                                                      */
/* MODIFICATION LOG :                                                   */
/*  Date     Who   Rev                  Comments                        */
/* --------  ---  ------   -------------------------------------------  */
/* 01/05/18  JRB  Add nvl_write.                                        */
/* 04/27/17  JRB  Add nvl_read.                                         */
/* 07/15/16  JRB  Add USE_TYPE_PTR option.                              */
/* 05/11/16  JRB  Chg to handle Alternate Access in report dataset.     */
/*                RCB_INFO: replace varNameArray with varInfoArray.     */
/* 01/07/15  JRB           Added named_var_*_async functions.           */
/* 02/07/14  JRB           Increase RptID to 129 to support Edition 2.  */
/* 03/07/08  JRB     06    Allow 129 char DatSet (MVL61850_MAX_OBJREF_LEN)*/
/*                         Add rcb_disable.                             */
/* 07/22/05  JRB     05    Add rcb_type, RptID, varNameArray to RCB_INFO.*/
/*                         Add ALL_RCB_INFO struct.                     */
/*                         Chg u_iec_rpt_ind_data 4th arg to (RCB_INFO *).*/
/* 07/15/05  JRB     04    Add mvl_defs.h                               */
/* 05/13/04  JRB     03    Chg SqNum to INT16U to match 61850-7-2.      */
/* 12/17/03  JRB     02    61850-8-1 FDIS changes:                      */
/*                         Add ConfRev to RCB_DATA & RCB_VAR.           */
/*                         Chg bvstring9 to bvstring10 for OptFlds.     */
/* 10/09/03  JRB     01    New                                          */
/************************************************************************/

#include "mvl_defs.h"   /* need base MVL types.         */

/* Defines required by Foundry generated code.                          */
/* Default values defined in "mvl_defs.h" but we don't like the defaults*/
/* so we undefine and redefine them here.                               */

#undef MVL_NUM_DYN_DOMS
#undef MVL_NUM_DYN_VMD_VARS
#undef MVL_NUM_DYN_VMD_NVLS
#undef MVL_NUM_DYN_JOUS
#undef MVL_NUM_DYN_DOM_VARS
#undef MVL_NUM_DYN_DOM_NVLS
#undef MVL_NUM_DYN_AA_VARS
#undef MVL_NUM_DYN_AA_NVLS
#undef MVLU_NUM_DYN_TYPES

#define MAX_CNT 100000 /*TODO: Arun*/

#define MVL_NUM_DYN_DOMS        MAX_CNT /*10*/ /*TODO: Arun*/
#define MVL_NUM_DYN_VMD_VARS    MAX_CNT /*100*/ /*TODO: Arun*/
#define MVL_NUM_DYN_VMD_NVLS    MAX_CNT /*10*/ /*TODO: Arun*/
#define MVL_NUM_DYN_JOUS        MAX_CNT /*10*/ /*TODO: Arun*/
#define MVL_NUM_DYN_DOM_VARS    MAX_CNT /*100*/ /*TODO: Arun*/
#define MVL_NUM_DYN_DOM_NVLS    MAX_CNT /*10*/ /*TODO: Arun*/
#define MVL_NUM_DYN_AA_VARS     MAX_CNT /*10*/ /*TODO: Arun*/
#define MVL_NUM_DYN_AA_NVLS     MAX_CNT /*10*/ /*TODO: Arun*/
#define MVLU_NUM_DYN_TYPES      MAX_CNT /*100*/     /* important for UCA    */ /*TODO: Arun*/

/* Struct to store common type ids needed for IEC/UCA Reports.  */
typedef struct
  {
  MVL_TYPE_ID mmsbool;       /* named so not confused with C++ 'bool' type   */
  MVL_TYPE_ID int8u;
  MVL_TYPE_ID int16u;        /* for SqNum.                   */
                        /* NOTE: UCA server sends int8u for SqNum       */
                        /* but it's safe to decode it as int16u         */
  MVL_TYPE_ID int32u;        /* for IntgPd                   */
  MVL_TYPE_ID vstring65;     /* for RptID                    */
  MVL_TYPE_ID btime6;        /* for EntryTime                */
  MVL_TYPE_ID ostring8;      /* for EntryID                  */
  MVL_TYPE_ID vstring32;     /* for RptId                    */
  MVL_TYPE_ID bvstring6;     /* for TrgOps (IEC)             */
  MVL_TYPE_ID bvstring8;     /* for OptFlds,TrgOps (UCA)     */
  MVL_TYPE_ID bvstring10;    /* for OptFlds (IEC)            */
  MVL_TYPE_ID bstr6;         /* for Reason (IEC)             */
  MVL_TYPE_ID bstr8;         /* for Reason (UCA)             */
  MVL_TYPE_ID bstr9;         /* for Writing OptFlds (IEC). Can't write all 10 bits.*/
  MVL_TYPE_ID objref;        /* for ObjectReference (IEC)    */
  } RPT_TYPEIDS;

typedef struct
  {
  ST_CHAR    RptID[MVL61850_MAX_OBJREF_LEN+1];
  struct     /* BVstring */
    {
    ST_INT16 len_1;
    ST_UCHAR data_1[2];         /* BVstring9 - need 2 bytes     */
    } OptFlds;
  ST_UINT16  SqNum;
  MMS_BTIME6 TimeOfEntry;
  ST_CHAR    DatSetNa[MVL61850_MAX_OBJREF_LEN+1];
  ST_BOOLEAN BufOvfl;
  ST_UINT16 SubSeqNum;
  ST_BOOLEAN MoreSegmentsFollow;
  ST_UINT8 EntryID [8];         /* ostring8                     */
  ST_UINT32 ConfRev;
  ST_UINT8 *Inclusion;          /* ptr to bitstring             */
                                /* Must alloc appropriate size  */
                                /* buffer for bitstring.        */
  ST_CHAR *dataRefName;         /* ptr to array of dataRefName  */
  MMS_BVSTRING *Reason;         /* ptr to array of Reason bitstrings    */
                                /* WARNING: this only works for variable*/
                                /* len bitstring up to 8 bits.          */
  } RCB_DATA;

typedef struct
  {
  MVL_VAR_ASSOC *  RptID;
  MVL_VAR_ASSOC *  OptFlds;
  MVL_VAR_ASSOC *  SqNum;
  MVL_VAR_ASSOC *  TimeOfEntry;
  MVL_VAR_ASSOC *  DatSetNa;
  MVL_VAR_ASSOC *  BufOvfl;
  MVL_VAR_ASSOC *  SubSeqNum;
  MVL_VAR_ASSOC *  MoreSegmentsFollow;
  MVL_VAR_ASSOC *  EntryID;
  MVL_VAR_ASSOC *  ConfRev;
  MVL_VAR_ASSOC *  Inclusion;
  MVL_VAR_ASSOC ** dataRefName; /* array of ptrs        */
  MVL_VAR_ASSOC ** dataValue;   /* array of ptrs        */
  MVL_VAR_ASSOC ** Reason;      /* array of ptrs        */
  } RCB_VAR;

/* Variable specification information for one variable in NVL.  */
typedef struct
  {
  OBJECT_NAME *objectName;      /* variable object name         */
  ALT_ACCESS *altAcc;           /* variable alt access info     */
                                /* NULL if alt access not present*/
  } NVL_VAR_INFO;

typedef struct
  {
  DBL_LNK link;         /* allows linked list of this struct    */
  ST_INT rcb_type;      /* RCB_TYPE_UCA, RCB_TYPE_IEC_BRCB, etc.        */
                        /* see RCB_TYPE.. defines in 'mvl_uca.h'.       */
  ST_CHAR RptID [MVL61850_MAX_OBJREF_LEN+1];
  NVL_VAR_INFO *varInfoArray;   /* array of variables in rpt dataset    */
  ST_INT numDsVar;      /* num variables in rpt dataset */
  MVL_TYPE_ID *typeIdArr;    /* array of type ids    */
  MVL_TYPE_ID InclusionTypeid;       /*Type ID for Inclusion Bitstring*/
  RCB_DATA rcb_data;
  RCB_VAR rcb_var;
  } RCB_INFO;

typedef struct
  {
  RPT_TYPEIDS *rpt_typeids;
  RCB_INFO *rcb_info_list;      /* linked list of RCB_INFO structs      */
  } ALL_RCB_INFO;       /* used to track multiple RCBs. */

/************************************************************************/
/* Global variables.                                                    */
/************************************************************************/
/* NONE */

/************************************************************************/
/* Function prototypes.                                                 */
/************************************************************************/
ST_RET waitReqDone (MVL_REQ_PEND *req, ST_INT timeout);
ST_VOID doCommService (ST_VOID);

ST_RET rpt_typeids_find (RPT_TYPEIDS *rpt_typeids);

RCB_INFO *rcb_info_create (MVL_NET_INFO *net_info, ST_CHAR *domName,
        ST_CHAR *rcbName, RPT_TYPEIDS *rpt_typeids, ST_INT timeOut);
ST_VOID rcb_info_destroy (RCB_INFO *rcb_info);

ST_RET rcb_enable (MVL_NET_INFO *netInfo, ST_CHAR *domName,
        ST_CHAR *rcbName, ST_UCHAR *OptFlds, ST_UCHAR *TrgOps,
        ST_UINT32 IntgPd, RPT_TYPEIDS *rpt_typeids, ST_INT timeOut);
ST_RET rcb_disable (MVL_NET_INFO *netInfo, ST_CHAR *domName,
        ST_CHAR *rcbName, RPT_TYPEIDS *rpt_typeids, ST_INT timeOut);

/* Simple functions to read/write a single variable.    */
ST_RET named_var_read (MVL_NET_INFO *net_info, ST_CHAR *varName,
                ST_INT scope, ST_CHAR *domName,
                MVL_TYPE_ID type_id, ST_VOID *dataDest, ST_INT timeOut);
ST_RET named_var_write (MVL_NET_INFO *netInfo, ST_CHAR *varName,
                ST_INT scope, ST_CHAR *domName,
                MVL_TYPE_ID type_id, ST_VOID *dataSrc, ST_INT timeOut);

ST_RET u_iec_rpt_ind (MVL_COMM_EVENT *event);
ST_VOID u_iec_rpt_ind_data (MVL_VAR_ASSOC **info_va,
        ST_UINT8 *OptFldsData,  /* ptr to data part of OptFlds bvstring */
        ST_UINT8 *InclusionData,        /* ptr to Inclusion bstring     */
        RCB_INFO *rcb_info,
        ST_INT va_total);

ST_RET named_var_read_async (MVL_NET_INFO *net_info,
  ST_CHAR *varName,
  ST_INT scope,
  ST_CHAR *domName,
  MVL_TYPE_ID type_id,
  ST_VOID *dataDest,  /* must point to valid memory until resp rcvd  */
  ST_VOID (*req_done)(MVL_REQ_PEND *reqCtrl));  /* called when resp rcvd*/

ST_VOID named_var_read_async_done (MVL_REQ_PEND *reqCtrl);

ST_RET named_var_write_async (MVL_NET_INFO *netInfo,
  ST_CHAR *varName,
  ST_INT scope,
  ST_CHAR *domName,
  MVL_TYPE_ID type_id,
  ST_VOID *dataSrc,
  ST_VOID (*req_done)(MVL_REQ_PEND *reqCtrl));  /* called when resp rcvd*/

ST_VOID named_var_write_async_done (MVL_REQ_PEND *reqCtrl);

ST_RET nvl_read (MVL_NET_INFO *net_info, MVL_VMD_CTRL *remote_vmd_ctrl, ST_CHAR *nvlName,
  ST_INT scope, ST_CHAR *domName, ST_INT timeOut);

ST_RET nvl_write (MVL_NET_INFO *net_info, MVL_VMD_CTRL *remote_vmd_ctrl, ST_CHAR *nvlName,
  ST_INT scope, ST_CHAR *domName,
  ST_VOID **data,     /* array of pointers to data  */
  ST_INT *data_size,  /* array of sizes of data    */
  ST_INT num_data,    /* number of pointers in data array */
                      /* This must match the number of variables in the NVL.  */
  ST_INT timeOut);

