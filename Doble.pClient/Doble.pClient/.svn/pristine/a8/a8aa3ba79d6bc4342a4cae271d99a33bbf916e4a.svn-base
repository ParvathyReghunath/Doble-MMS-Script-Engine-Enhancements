/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*      (c) Copyright Systems Integration Specialists Company, Inc.,    */
/*      2006-2018, All Rights Reserved.                                 */
/*                                                                      */
/*                  PROPRIETARY AND CONFIDENTIAL                        */
/*                                                                      */
/* MODULE NAME : smpval.h                                               */
/* PRODUCT(S)  :                                                        */
/*                                                                      */
/* MODULE DESCRIPTION :                                                 */
/*      Structures, functions for sending/receiving IEC 61850 Sampled   */
/*      Value messages (according to IEC 61850-9-2).                    */
/*                                                                      */
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :                            */
/*                                                                      */
/* MODIFICATION LOG :                                                   */
/*  Date     Who   Rev                  Comments                        */
/* --------  ---  ------   -------------------------------------------  */
/* 08/15/18  JRB  SMPVAL_SUBSCRIBER: add "MAC" and "isRsv".             */
/*                Add smpvalSubscriberList extern.                      */
/*                Tissue 1223: Use setSrcRef instead of setSrcCB in LSVS.*/
/*                Add LSVSToSubscriber.                                 */
/* 07/23/18  JRB  Add SMPVAL_PUBLISHER, smpvalPublisherCreateAll,       */
/*                smpvalPublisherDestroyAll, smpvalPublisherList.       */
/* 02/12/18  JRB  Add SMPVAL_SUBSCRIBER, smpvalSubscribeIedNameAll,     */
/*                smpvalUnsubscribeAll.                                 */
/*                SMPVAL_MSG: add isRsv, slipSendContext.               */
/*                Add prototype for existing smpval_msg_decode_asn1.    */
/* 12/12/11  JRB           Increase "svID" size to support Ed 1 & Ed 2. */
/*                         Fix OptFlds bit num definitions (SVOPT_*).   */
/*                         smpval_msg_decode: added "edition" arg.      */
/*                         Added smpval_msg_log proto.                  */
/* 02/15/08  JRB     03    Fix securityLen type (ST_UINT -> ST_INT).    */
/* 08/09/06  JRB     02    Fix svID len (Vstring65).                    */
/*                         Change DatSet to Vstring129 (ObjectReference)*/
/* 03/17/05  JRB     01    Created                                      */
/************************************************************************/
#ifndef SMPVAL_INCLUDED
#define SMPVAL_INCLUDED

#include "asn1r.h"                      /* need MMS_UTC_TIME    */
#include "ethertyp.h"                   /* need ETYPE_INFO      */
#include "scl.h"                        /* need SCL_INFO        */

#ifdef __cplusplus
extern "C" {
#endif

/* Use these to check max len of "svID" in ASDU.        */
#define MAX_SMPVAL_SVID_LEN_ED1 65      /* 61850 MsvID, UsvID len (ed 1)*/
#define MAX_SMPVAL_SVID_LEN_ED2 129     /* 61850 MsvID, UsvID len (ed 2)*/

#define MAX_SMPVAL_OBJREF_LEN   129     /* 61850 ObjectReference len    */
#define MAX_SMPVAL_SECURITY_LEN 20      /* max len of security info     */

/* Bit numbers in OptFlds bitstring (configured by SmvOpts in SCL file) */
#define SVOPT_BITNUM_REFRTM     0
#define SVOPT_BITNUM_SMPSYNCH   1       /* Ignored for Edition 2        */
#define SVOPT_BITNUM_SMPRATE    2
#define SVOPT_BITNUM_DATSET     3       /* Edition 2 only       */
#define SVOPT_BITNUM_SECURITY   4       /* Edition 2 only       */

/************************************************************************/
/* STRUCTURE DEFINITIONS                                                */
/************************************************************************/
typedef struct
  {
  ST_UINT8 *SamplePtr;          /* pointer to "Sample" data             */
  ST_INT SampleLen;             /* length of "Sample" data in bytes     */
  ST_CHAR svID [MAX_SMPVAL_SVID_LEN_ED2+1];     /* MsvID or UsvID       */
                                /* Big enough for Ed 1 or Ed 2          */
  ST_BOOLEAN DatSetPres;        /* is "DatSet" present in ASDU? */
  ST_CHAR DatSet [MAX_SMPVAL_OBJREF_LEN+1];     /* Vstring129 (ObjectReference)*/
                                                /* (Optional)            */
  ST_UINT16 SmpCnt;
  ST_UINT32 ConfRev;
  ST_BOOLEAN SmpSynch;
  ST_BOOLEAN RefrTmPres;        /* is "RefrTm" present in ASDU? */
  MMS_UTC_TIME RefrTm;          /* Optional                     */
  ST_BOOLEAN SmpRatePres;       /* is "SmpRate" present in ASDU?        */
  ST_UINT16 SmpRate;            /* Optional                     */
  /* NOTE: SmpSynch_ed2, SmpModPres, SmpMod used for Edition 2 only     */
  ST_UINT8 SmpSynch_ed2;        /* range 0 - 255                */
  ST_BOOLEAN SmpModPres;        /* is "SmpMod" present in ASDU?*/
  ST_UINT16 SmpMod;             /* Optional                     */
  } SMPVAL_ASDU;

typedef struct
  {
  ST_BOOLEAN isRsv;     /* SD_TRUE if this is RSV message               */
  ST_INT edition;       /* 61850 edition (1 or 2) used for this message */
  ST_UINT16 numASDU;    /* Num of ASDU concatenated into one APDU       */
  SMPVAL_ASDU *asduArray;       /* array of "numASDU" structs           */
                                /* allocated by smpval_msg_create OR    */
                                /* allocated during decode              */
  ST_UCHAR securityBuf [MAX_SMPVAL_SECURITY_LEN];       /* security info*/
  ST_INT securityLen;           /* len of security info                 */
  ST_VOID *slipSendContext;     /* for sending RSV message              */
  }SMPVAL_MSG;

typedef struct smpval_subscriber
  {
  /* CRITICAL: First 2 parameters used to add this struct to linked     */
  /* lists using list_add_last, etc.                                    */
  struct smpval_subscriber *next; /* CRITICAL: DON'T MOVE.*/
  struct smpval_subscriber *prev; /* CRITICAL: DON'T MOVE.*/

  ST_CHAR   svID    [MVL61850_MAX_OBJREF_LEN + 1];
  ST_CHAR   SvCBRef [MVL61850_MAX_OBJREF_LEN + 1];
  ST_CHAR   DatSet  [MVL61850_MAX_OBJREF_LEN + 1];
  MVL_NVLIST_CTRL *dsNvl;         /* NVL created for SV Dataset   */
  ST_CHAR setSrcRef [MVL61850_MAX_OBJREF_LEN + 1]; /* to match setSrcRef in LSVS*/
  ST_UCHAR MAC [CLNP_MAX_LEN_MAC];      /* Multicast MAC address        */
  ST_BOOLEAN isRsv;
  ST_VOID (*usr_callback)(struct smpval_subscriber *smpvalSubscriber,
                          SMPVAL_MSG *smpvalMsg); /* callback function ptr*/
  } SMPVAL_SUBSCRIBER;

typedef struct smpval_publisher
  {
  /* CRITICAL: First 2 parameters used to add this struct to linked     */
  /* lists using list_add_last, etc.                                    */
  struct smpval_publisher *next;        /* CRITICAL: DON'T MOVE.*/
  struct smpval_publisher *prev;        /* CRITICAL: DON'T MOVE.*/

  ST_BOOLEAN multicast;   /* SD_TRUE if this is MSVCB */
  ST_CHAR objRefAcsi [MVL61850_MAX_OBJREF_LEN+1]; /* for tracking     */

  /* Pointers to important attributes in SampledValue Control Block.  */
  ST_BOOLEAN *SvEna;
  ST_CHAR *DatSet;
  ST_CHAR *MsvID;    /* pointer to MsvID (UsvID if multicast=SD_TRUE) */
  ST_UINT32 *ConfRev;
  ST_UINT16 *SmpRate;
  MMS_BVSTRING *OptFlds;
  ST_INT8 *SmpMod;    /* added in Edition 2 (optional)  */
  /* NOTE: these point to data in the dstAddress sub-structure.*/
  ST_UINT8  *DstAddress_Addr;  /* Ostring6 */
  ST_UINT8  *DstAddress_PRIORITY;
  ST_UINT16 *DstAddress_VID;
  ST_UINT16 *DstAddress_APPID;
  ST_UINT16 *noASDU;  /* added in Edition 2 (mandatory) */
  } SMPVAL_PUBLISHER;

/************************************************************************/
/* Functions for sending SMPVAL messages.                               */
/************************************************************************/
SMPVAL_MSG *smpval_msg_create (
        ST_UINT numASDU);       /* Num of ASDU concatenated into one APDU*/

ST_VOID smpval_msg_destroy (SMPVAL_MSG *smpvalMsg);

/* Updates data stored for one ASDU to be sent in a SMPVAL message      */
ST_RET smpval_asdu_data_update (
        SMPVAL_MSG *smpvalMsg,  /* SMPVAL msg info structure            */
        ST_INT asduIdx,         /* index into array of ASDU for this SMPVAL msg*/
        ST_UINT8 *SamplePtr,    /* pointer to data                      */
        ST_INT SampleLen,       /* length of data in bytes              */
        ST_CHAR *svID,
        ST_INT SmpCnt,
        ST_BOOLEAN DatSetPres,  /* SD_TRUE if optional DatSet should be sent*/
        ST_CHAR *DatSet,        /* Optional (chk DatSetPres arg)        */
        ST_UINT32 ConfRev,
        ST_BOOLEAN SmpSynch,
        ST_BOOLEAN RefrTmPres,  /* SD_TRUE if optional RefrTm should be sent*/
        MMS_UTC_TIME *RefrTm,   /* Optional (chk_RefrTmPres arg)        */
        ST_BOOLEAN SmpRatePres, /* SD_TRUE if optional SmpRate should be sent*/
        ST_UINT16 SmpRate);     /* Optional (chk SmpRatePres arg)       */
/* Similar function for Edition 2.      */
ST_RET smpval_asdu_data_update_ed2 (
        SMPVAL_MSG *smpvalMsg,  /* SMPVAL msg info structure            */
        ST_INT asduIdx,         /* index into array of ASDU for this SMPVAL msg*/
        ST_UINT8 *SamplePtr,    /* pointer to data                      */
        ST_INT SampleLen,       /* length of data in bytes              */
        ST_CHAR *svID,
        ST_INT SmpCnt,
        ST_BOOLEAN DatSetPres,  /* SD_TRUE if optional DatSet should be sent*/
        ST_CHAR *DatSet,        /* Optional (chk DatSetPres arg)        */
        ST_UINT32 ConfRev,
        ST_UINT8 SmpSynch,      /* 0-255                                */
        ST_BOOLEAN RefrTmPres,  /* SD_TRUE if optional RefrTm should be sent*/
        MMS_UTC_TIME *RefrTm,   /* Optional (chk_RefrTmPres arg)        */
        ST_BOOLEAN SmpRatePres, /* SD_TRUE if optional SmpRate should be sent*/
        ST_UINT16 SmpRate,      /* Optional (chk SmpRatePres arg)       */
        /* These args are used for Ed 2 only    */
        ST_BOOLEAN SmpModPres,  /* SD_TRUE if optional SmpMod should be sent*/
        ST_UINT16 SmpMod);      /* Optional (chk SmpModPres arg)        */

/* Encodes complete SMPVAL message (APDU plus Ethertype header).                */
/* RETURNS: ptr to start of encoded message (or NULL on error)          */
ST_UCHAR *smpval_msg_encode (
        SMPVAL_MSG *smpvalMsg,  /* SMPVAL msg info structure            */
        ETYPE_INFO *etypeInfo,  /* Ethertype info       */
        ST_UCHAR *asn1DataBuf,  /* buffer to encode in  */
        ST_INT asn1DataBufLen,  /* len of buffer        */
        ST_INT *asn1DataLenOut);/* ptr to len encoded   */
ST_RET smpval_msg_send (
        SMPVAL_MSG *smpvalMsg,  /* SMPVAL msg info structure            */
        ETYPE_INFO *etypeInfo,  /* Ethertype info       */
        ST_UCHAR *dstMac);      /* Destination (Multicast) MAC address  */

/************************************************************************/
/* Functions for receiving SMPVAL messages.                             */
/************************************************************************/
/* Decode SMPVAL message and fill in SMPVAL_MSG struct.*/
SMPVAL_MSG *smpval_msg_decode (
        SN_UNITDATA *sn_req,    /* message to decode                    */
        ST_INT edition,         /* 61850 edition (1 or 2).              */
        ETYPE_INFO *etypeInfo); /* where to store decoded Ethertype info*/

ST_RET smpval_msg_decode_asn1 (SMPVAL_MSG *smpvalMsg, ST_UCHAR *apdu, ST_INT apdu_len);

/* Must be called to free structure returned from smpval_msg_decode.    */
ST_VOID smpval_msg_free (SMPVAL_MSG *smpvalMsg);
ST_VOID smpval_msg_log (SMPVAL_MSG *smpvalMsg,
        ST_CHAR *hdr);  /* text to log on first line    */

ST_RET smpvalSubscribeIedNameAll (SCL_INFO *scl_info,
        ST_CHAR *iedName,           /* IED name of Subscriber         */
        ST_CHAR *apName,            /* AccessPoint name of Subscriber */
        ST_VOID (*usr_callback)(struct smpval_subscriber *smpvalSubscriber,
                                SMPVAL_MSG *smpvalMsg),
        ST_VOID *slipReceiveContext); /* for receiving RSV message  */

ST_RET smpvalUnsubscribeAll (ST_VOID);

SMPVAL_SUBSCRIBER *smpvalSubscriberFind(ST_CHAR *setSrcRef);
SMPVAL_SUBSCRIBER *LSVSToSubscriber (MVL_VAR_ASSOC *base_va);

/************************************************************************/
/* Functions for storing SMPVAL Publisher information.                  */
/************************************************************************/
ST_RET smpvalPublisherCreateAll (SCL_INFO *scl_info,
        SCL_SERVER *scl_server,   /* Server to Publish SampledValue */
        MVL_VMD_CTRL *vmd_ctrl);  /* VMD containing Server objects  */
ST_VOID smpvalPublisherDestroyAll (ST_VOID);

/************************************************************************/
/* Utility functions                                                    */
/************************************************************************/
/* Copies data from source to destination but reverses the order of the */
/* bytes (i.e. converts Big-Endian to Little-Endian or vice versa).     */
ST_VOID reverse_bytes (
        ST_UINT8 *dst,          /* ptr to destination buffer    */
        ST_UINT8 *src,          /* ptr to source buffer         */
        ST_INT numbytes);       /* number of bytes to copy      */

/************************************************************************/
/* Global variables                                                     */
/************************************************************************/
extern SMPVAL_SUBSCRIBER *smpvalSubscriberList; /* linked list of SV subscriptions.*/
extern SMPVAL_PUBLISHER *smpvalPublisherList; /* linked list of SV publishers*/

#ifdef __cplusplus
}
#endif
#endif          /* included     */
