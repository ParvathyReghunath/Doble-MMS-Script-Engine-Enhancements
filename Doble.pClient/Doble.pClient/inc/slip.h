/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*      (c) Copyright Systems Integration Specialists Company, Inc.,    */
/*      2016, All Rights Reserved                                       */
/*                                                                      */
/* MODULE NAME : slip.h                                                 */
/* PRODUCT(S)  : MMS-EASE Lite                                          */
/*                                                                      */
/* MODULE DESCRIPTION :                                                 */
/*      Structures and functions for the SLIP API.                      */
/*                                                                      */
/* MODIFICATION LOG :                                                   */
/*  Date     Who                        Comments                        */
/* --------  ---  ----------------------------------------------------  */
/* 04/19/18  DSF  Resolved SLIP_SOCKADDR_IN port issue                  */
/* 02/07/18  DSF  No C++ comments                                       */
/* 02/07/18  DSF  Eliminated anonymous unions - some older compilers    */
/*                can't deal with them                                  */
/* 11/29/17  DSF  Added slipDecodeGooseFrame()                          */
/* 08/07/17  DSF  Added SLIP_RECEIVE_MESSAGE.elapsed                    */
/* 03/15/17  DSF  Added Int and String Context options                  */
/* 01/25/16  DSF  Initial Release                                       */
/************************************************************************/

#ifndef SLIP_INCLUDED
#define SLIP_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "glbtypes.h"
#include "sysincs.h"
#include "gen_list.h"
#include "mms_def2.h"           /* need MAX_IDENT_LEN   */
#include "clnp_sne.h"
#include "stats.h"

#include "glbsem.h"
#include "gensock2.h"
#if defined (SEV_SUPPORT)
#include "sev.h"
#else
#define SEV_TRACE_ENABLED 0
#define SEV_CHECKED_TRACE_ENABLED 0

#define SEV_TRACE(__evId,__evInfo,__evText)
#define SEV_TRACE_EX(__evId,__evInfo,__evText, __srcFile, __srcLine)
#define SEV_TRACE_EXX(__threadId,__evId,__evInfo,__evText, __srcFile, __srcLine)
#define SEV_TRACE_CHECKED(__mask,__evId,__evInfo,__evText)
#define SEV_TRACE_EX_CHECKED(__mask,__evId,__evInfo,__evText, __srcFile, __srcLine)
#define SEV_TRACE_EXX_CHECKED(__mask,__threadId,__evId,__evInfo,__evText, __srcFile, __srcLine)

#define SEV_TRACE_FMT0(__evId,__evInfo,__fmt)
#define SEV_TRACE_FMT1(__evId,__evInfo,__fmt,a)
#define SEV_TRACE_FMT2(__evId,__evInfo,__fmt,a,b)
#define SEV_TRACE_FMT3(__evId,__evInfo,__fmt,a,b,c)
#define SEV_TRACE_FMT4(__evId,__evInfo,__fmt,a,b,c,d)
#define SEV_TRACE_FMT5(__evId,__evInfo,__fmt,a,b,c,d,e)
#define SEV_TRACE_FMT6(__evId,__evInfo,__fmt,a,b,c,d,e,f)

#define SEV_TRACE_CHECKED_FMT0(__mask,__evId,__evInfo,__fmt)
#define SEV_TRACE_CHECKED_FMT1(__mask,__evId,__evInfo,__fmt,a)
#define SEV_TRACE_CHECKED_FMT2(__mask,__evId,__evInfo,__fmt,a,b)
#define SEV_TRACE_CHECKED_FMT3(__mask,__evId,__evInfo,__fmt,a,b,c)
#define SEV_TRACE_CHECKED_FMT4(__mask,__evId,__evInfo,__fmt,a,b,c,d)
#define SEV_TRACE_CHECKED_FMT5(__mask,__evId,__evInfo,__fmt,a,b,c,d,e)
#define SEV_TRACE_CHECKED_FMT6(__mask,__evId,__evInfo,__fmt,a,b,c,d,e,f)

#define _sevGetDoubleEt() (0.0)
#endif
#include "str_util.h"

#undef ASSERT

#if defined (_WIN32)
#include <ws2def.h>
#include <ws2ipdef.h>
#include <ws2tcpip.h>
#include <mstcpip.h>
#include <mswsock.h>
#endif

#if !defined (_WIN32)
#define SOCKADDR        struct sockaddr
#define SOCKADDR_IN     struct sockaddr_in
#define IN_ADDR         struct in_addr
#define IP_MREQ         struct ip_mreq
#define IP_MREQ_SOURCE  struct ip_mreq_source
#define closesocket     close
#define SOCKADDR_IN6    struct sockaddr_in6
#define IN6_ADDR        struct in6_addr
#define IPV6_MREQ       struct ipv6_mreq
#endif

/************************************************************************/
/*                      Log Macros                                      */
/************************************************************************/

#define SLIPLOG_ERR          0x00000001
#define SLIPLOG_NERR         0x00000002
#define SLIPLOG_FLOW         0x00000004
#define SLIPLOG_DETAIL       0x00000008
#define SLIPLOG_TIMING       0x00000010

extern ST_ULONG slipLogMask;

extern ST_CHAR *_slipLogErrString;
extern ST_CHAR *_slipLogNerrString;
extern ST_CHAR *_slipLogFlowString;
extern ST_CHAR *_slipLogDetailString;
extern ST_CHAR *_slipLogTimingString;


#define SLIP_LOG_ERR0(a) \
  SLOG_0 (slipLogMask & SLIPLOG_ERR, _slipLogErrString, a);
#define SLIP_LOG_ERR1(a,b) \
  SLOG_1 (slipLogMask & SLIPLOG_ERR, _slipLogErrString, a, b);
#define SLIP_LOG_ERR2(a,b,c) \
  SLOG_2 (slipLogMask & SLIPLOG_ERR, _slipLogErrString, a, b, c);
#define SLIP_LOG_ERR3(a,b,c,d) \
  SLOG_3 (slipLogMask & SLIPLOG_ERR, _slipLogErrString, a, b, c, d);
#define SLIP_LOG_ERR4(a,b,c,d,e) \
  SLOG_4 (slipLogMask & SLIPLOG_ERR, _slipLogErrString, a, b, c, d, e);

#define SLIP_LOG_ERRH(a,b) \
  SLOGH (slipLogMask & SLIPLOG_ERR, a, b);

#define SLIP_LOG_CERR0(a) \
  SLOGC_0 (slipLogMask & SLIPLOG_ERR, a);
#define SLIP_LOG_CERR1(a,b) \
  SLOGC_1 (slipLogMask & SLIPLOG_ERR, a, b);
#define SLIP_LOG_CERR2(a,b,c) \
  SLOGC_2 (slipLogMask & SLIPLOG_ERR, a, b, c);
#define SLIP_LOG_CERR3(a,b,c,d) \
  SLOGC_3 (slipLogMask & SLIPLOG_ERR, a, b, c, d);
#define SLIP_LOG_CERR4(a,b,c,d,e) \
  SLOGC_4 (slipLogMask & SLIPLOG_ERR, a, b, c, d, e);

/************************************************************************/
/************************************************************************/

#define SLIP_LOG_NERR0(a) \
  SLOG_0 (slipLogMask & SLIPLOG_NERR, _slipLogNerrString, a)
#define SLIP_LOG_NERR1(a,b) \
  SLOG_1 (slipLogMask & SLIPLOG_NERR, _slipLogNerrString, a, b)
#define SLIP_LOG_NERR2(a,b,c) \
  SLOG_2 (slipLogMask & SLIPLOG_NERR, _slipLogNerrString, a, b, c)
#define SLIP_LOG_NERR3(a,b,c,d) \
  SLOG_3 (slipLogMask & SLIPLOG_NERR, _slipLogNerrString, a, b, c, d)
#define SLIP_LOG_NERR4(a,b,c,d,e) \
  SLOG_4 (slipLogMask & SLIPLOG_NERR, _slipLogNerrString, a, b, c, d, e)
#define SLIP_LOG_NERR5(a,b,c,d,e,f) \
  SLOG_5 (slipLogMask & SLIPLOG_NERR, _slipLogNerrString, a, b, c, d, e, f)

#define SLIP_LOG_NERRH(a,b) \
  SLOGH (slipLogMask & SLIPLOG_NERR, a, b)

#define SLIP_LOG_CNERR0(a) \
  SLOGC_0 (slipLogMask & SLIPLOG_NERR, a)
#define SLIP_LOG_CNERR1(a,b) \
  SLOGC_1 (slipLogMask & SLIPLOG_NERR, a, b)
#define SLIP_LOG_CNERR2(a,b,c) \
  SLOGC_2 (slipLogMask & SLIPLOG_NERR, a, b, c)
#define SLIP_LOG_CNERR3(a,b,c,d) \
  SLOGC_3 (slipLogMask & SLIPLOG_NERR, a, b, c, d)
#define SLIP_LOG_CNERR4(a,b,c,d,e) \
  SLOGC_4 (slipLogMask & SLIPLOG_NERR, a, b, c, d, e)
#define SLIP_LOG_CNERR5(a,b,c,d,e,f) \
  SLOGC_5 (slipLogMask & SLIPLOG_NERR, a, b, c, d, e, f)

/************************************************************************/
/************************************************************************/

#define SLIP_LOG_FLOW0(a) \
  SLOG_0 (slipLogMask & SLIPLOG_FLOW, _slipLogFlowString, a)
#define SLIP_LOG_FLOW1(a,b) \
  SLOG_1 (slipLogMask & SLIPLOG_FLOW, _slipLogFlowString, a, b)
#define SLIP_LOG_FLOW2(a,b,c) \
  SLOG_2 (slipLogMask & SLIPLOG_FLOW, _slipLogFlowString, a, b, c)
#define SLIP_LOG_FLOW3(a,b,c,d) \
  SLOG_3 (slipLogMask & SLIPLOG_FLOW, _slipLogFlowString, a, b, c, d)
#define SLIP_LOG_FLOW4(a,b,c,d,e) \
  SLOG_4 (slipLogMask & SLIPLOG_FLOW, _slipLogFlowString, a, b, c, d, e)
#define SLIP_LOG_FLOW5(a,b,c,d,e,f) \
  SLOG_5 (slipLogMask & SLIPLOG_FLOW, _slipLogFlowString, a, b, c, d, e, f)

#define SLIP_LOG_FLOWH(a,b) \
  SLOGH (slipLogMask & SLIPLOG_FLOW, a, b)

#define SLIP_LOG_CFLOW0(a) \
  SLOGC_0 (slipLogMask & SLIPLOG_FLOW, a)
#define SLIP_LOG_CFLOW1(a,b) \
  SLOGC_1 (slipLogMask & SLIPLOG_FLOW, a, b)
#define SLIP_LOG_CFLOW2(a,b,c) \
  SLOGC_2 (slipLogMask & SLIPLOG_FLOW, a, b, c)
#define SLIP_LOG_CFLOW3(a,b,c,d) \
  SLOGC_3 (slipLogMask & SLIPLOG_FLOW, a, b, c, d)
#define SLIP_LOG_CFLOW4(a,b,c,d,e) \
  SLOGC_4 (slipLogMask & SLIPLOG_FLOW, a, b, c, d, e)
#define SLIP_LOG_CFLOW5(a,b,c,d,e,f) \
  SLOGC_5 (slipLogMask & SLIPLOG_FLOW, a, b, c, d, e, f)

/************************************************************************/
/************************************************************************/

#define SLIP_LOG_DETAIL0(a) \
  SLOG_0 (slipLogMask & SLIPLOG_DETAIL, _slipLogDetailString, a)
#define SLIP_LOG_DETAIL1(a,b) \
  SLOG_1 (slipLogMask & SLIPLOG_DETAIL, _slipLogDetailString, a, b)
#define SLIP_LOG_DETAIL2(a,b,c) \
  SLOG_2 (slipLogMask & SLIPLOG_DETAIL, _slipLogDetailString, a, b, c)
#define SLIP_LOG_DETAIL3(a,b,c,d) \
  SLOG_3 (slipLogMask & SLIPLOG_DETAIL, _slipLogDetailString, a, b, c, d)
#define SLIP_LOG_DETAIL4(a,b,c,d,e) \
  SLOG_4 (slipLogMask & SLIPLOG_DETAIL, _slipLogDetailString, a, b, c, d, e)
#define SLIP_LOG_DETAIL5(a,b,c,d,e,f) \
  SLOG_5 (slipLogMask & SLIPLOG_DETAIL, _slipLogDetailString, a, b, c, d, e, f)

#define SLIP_LOG_DETAILH(a,b) \
  SLOGH (slipLogMask & SLIPLOG_DETAIL, a, b)

#define SLIP_LOG_CDETAIL0(a) \
  SLOGC_0 (slipLogMask & SLIPLOG_DETAIL, a)
#define SLIP_LOG_CDETAIL1(a,b) \
  SLOGC_1 (slipLogMask & SLIPLOG_DETAIL, a, b)
#define SLIP_LOG_CDETAIL2(a,b,c) \
  SLOGC_2 (slipLogMask & SLIPLOG_DETAIL, a, b, c)
#define SLIP_LOG_CDETAIL3(a,b,c,d) \
  SLOGC_3 (slipLogMask & SLIPLOG_DETAIL, a, b, c, d)
#define SLIP_LOG_CDETAIL4(a,b,c,d,e) \
  SLOGC_4 (slipLogMask & SLIPLOG_DETAIL, a, b, c, d, e)
#define SLIP_LOG_CDETAIL5(a,b,c,d,e,f) \
  SLOGC_5 (slipLogMask & SLIPLOG_DETAIL, a, b, c, d, e, f)

/************************************************************************/
/************************************************************************/

#define SLIP_LOG_TIMING0(a) \
  SLOG_0 (slipLogMask & SLIPLOG_TIMING, _slipLogTimingString, a)
#define SLIP_LOG_TIMING1(a,b) \
  SLOG_1 (slipLogMask & SLIPLOG_TIMING, _slipLogTimingString, a, b)
#define SLIP_LOG_TIMING2(a,b,c) \
  SLOG_2 (slipLogMask & SLIPLOG_TIMING, _slipLogTimingString, a, b, c)
#define SLIP_LOG_TIMING3(a,b,c,d) \
  SLOG_3 (slipLogMask & SLIPLOG_TIMING, _slipLogTimingString, a, b, c, d)
#define SLIP_LOG_TIMING4(a,b,c,d,e) \
  SLOG_4 (slipLogMask & SLIPLOG_TIMING, _slipLogTimingString, a, b, c, d, e)
#define SLIP_LOG_TIMING5(a,b,c,d,e,f) \
  SLOG_5 (slipLogMask & SLIPLOG_TIMING, _slipLogTimingString, a, b, c, d, e, f)

#define SLIP_LOG_TIMINGH(a,b) \
  SLOGH (slipLogMask & SLIPLOG_TIMING, a, b)

#define SLIP_LOG_CTIMING0(a) \
  SLOGC_0 (slipLogMask & SLIPLOG_TIMING, a)
#define SLIP_LOG_CTIMING1(a,b) \
  SLOGC_1 (slipLogMask & SLIPLOG_TIMING, a, b)
#define SLIP_LOG_CTIMING2(a,b,c) \
  SLOGC_2 (slipLogMask & SLIPLOG_TIMING, a, b, c)
#define SLIP_LOG_CTIMING3(a,b,c,d) \
  SLOGC_3 (slipLogMask & SLIPLOG_TIMING, a, b, c, d)
#define SLIP_LOG_CTIMING4(a,b,c,d,e) \
  SLOGC_4 (slipLogMask & SLIPLOG_TIMING, a, b, c, d, e)
#define SLIP_LOG_CTIMING5(a,b,c,d,e,f) \
  SLOGC_5 (slipLogMask & SLIPLOG_TIMING, a, b, c, d, e, f)


/************************************************************************/
/*      SLIP Interface                                                  */
/************************************************************************/

/************************************************************************/
#define SLIP_ERROR_SUCCESS                     0
#define SLIP_ERROR_SYSTEM                      1
#define SLIP_ERROR_INTERNAL                    2
#define SLIP_ERROR_EXTERNAL                    3
#define SLIP_ERROR_INVALID_PARAM               4
#define SLIP_ERROR_BAD_STATE                   5
#define SLIP_ERROR_TOO_BIG                     6
#define SLIP_ERROR_TOO_MANY_ITEMS              7

#define SLIP_ERROR_OTHER                       100

/************************************************************************/
#define SLIP_CONTEXT_STATE_UNINITIALIZED       0
#define SLIP_CONTEXT_STATE_INACTIVE            1
#define SLIP_CONTEXT_STATE_ACTIVE              2
#define SLIP_CONTEXT_STATE_ACTIVATING          3
#define SLIP_CONTEXT_STATE_DEACTIVATING        4
#define SLIP_CONTEXT_STATE_FAULT               5

/************************************************************************/
#define SLIP_MESSAGE_PRIORITY_NORMAL           0
#define SLIP_MESSAGE_PRIORITY_HIGH             1

/************************************************************************/
#define SLIP_THREAD_PRIORITY_NORMAL            0
#define SLIP_THREAD_PRIORITY_HIGH              2
#define SLIP_THREAD_PRIORITY_CRITICAL          15

/************************************************************************/
#define SLIP_PROVIDER_SOCKETS                  0    /* internal use only */
#define SLIP_PROVIDER_PCAP                     1

/************************************************************************/
#define SLIP_PROTOCOL_RAW                      0    /* internal use, testing only */
#define SLIP_PROTOCOL_RAW_UDP                  1    /* internal use, testing only */
#define SLIP_PROTOCOL_GOOSE                    2
#define SLIP_PROTOCOL_RGOOSE                   3

/************************************************************************/
#define SLIP_SEND_MODE_BASIC                   0
#define SLIP_SEND_MODE_ADVANCED_1              1
#define SLIP_SEND_MODE_ADVANCED_2              2
#define SLIP_SEND_MODE_ADVANCED_3              3

/************************************************************************/
#define ETHERTYPE_IPv4                        0x0800
#define ETHERTYPE_ARP                         0x0806
#define ETHERTYPE_IPv6                        0x86DD
#define ETHERTYPE_VLAN                        0x8100
#define ETHERTYPE_GOOSE                       0x88B8
#define ETHERTYPE_GSE                         0x88B9
#define ETHERTYPE_SV                          0x88BA
#define ETHERTYPE_TEST                        0x8900

/************************************************************************/
#define RGOOSE_UDP_PORT                       102

/************************************************************************/
#define GOOSE_TYPE_TUNNELLED                  0
#define GOOSE_TYPE_GOOSE                      1
#define GOOSE_TYPE_SV                         2
#define GOOSE_TYPE_GSE                        3

/************************************************************************/
#define GOOSE_APPID_TYPE1                     0
#define GOOSE_APPID_GSE                       0
#define GOOSE_APPID_SV                        1
#define GOOSE_APPID_TYPE1A                    2

/************************************************************************/
#define ETHER_FRAME_HEADER_LENGTH             18    /* Dest MAC(6) + Source MAC(6) + VLAN Tag(4) + EtherType(2) */
#define IP_MIN_FRAME_HEADER_LENGTH            20
#define IPV6_MIN_FRAME_HEADER_LENGTH          40
#define IP_OPTIONS_LENGTH                     4
#define UDP_FRAME_HEADER_LENGTH               8
#define GOOSE_FRAME_HEADER_LENGTH             8     /* App ID(2) + Length(2) + Reserved 1(2) + Reserved 2(2) */
#define RGOOSE_FRAME_HEADER_LENGTH            37

/************************************************************************/
#define IP_SECURITY_TYPE                      130

#define IP_SECURITY_CLASS_LEVEL_RESERVED4     ((ST_UCHAR) 0x01)
#define IP_SECURITY_CLASS_LEVEL_TOP_SECRET    ((ST_UCHAR) 0x3D)
#define IP_SECURITY_CLASS_LEVEL_SECRET        ((ST_UCHAR) 0x5A)
#define IP_SECURITY_CLASS_LEVEL_CONFIDENTIAL  ((ST_UCHAR) 0x96)
#define IP_SECURITY_CLASS_LEVEL_RESERVED3     ((ST_UCHAR) 0x66)
#define IP_SECURITY_CLASS_LEVEL_RESERVED2     ((ST_UCHAR) 0xCC)
#define IP_SECURITY_CLASS_LEVEL_UNCLASSIFIED  ((ST_UCHAR) 0xAB)
#define IP_SECURITY_CLASS_LEVEL_RESERVED1     ((ST_UCHAR) 0xF1)


/************************************************************************/
/* IPv4/IPv6 Unions/Structs                                             */
/************************************************************************/

#define SLIP_IP_UNDEFINED           0
#define SLIP_IP_V4                  AF_INET
#define SLIP_IP_V6                  AF_INET6

typedef union
  {
  SOCKADDR                    sockaddr;
  SOCKADDR_IN                 ipv4;
  SOCKADDR_IN6                ipv6;
  } SLIP_SOCKADDR_IN;

typedef union slip_in_addr
  {
#ifdef __cplusplus
  slip_in_addr ()
    {
    memset (this, 0, sizeof (*this));
    }
#endif
  IN_ADDR         ipv4;
  IN6_ADDR        ipv6;
  } SLIP_IN_ADDR;

typedef struct slip_4_6_addr
  {
#ifdef __cplusplus
  slip_4_6_addr ()
    {
    memset (this, 0, sizeof (*this));
    }
#endif
  ST_INT          ipVersion;
  SLIP_IN_ADDR    inAddr;
  } SLIP_IN_4_6_ADDR;

/************************************************************************/
/* IP_SOURCE_MULTICAST_GROUP                                            */
/************************************************************************/

#define SLIP_GROUP_STATE_NONE                 0
#define SLIP_GROUP_STATE_ADDED                1
#define SLIP_GROUP_STATE_REMOVED              2

typedef struct
  {
  ST_CHAR       sourceIpAddr[MAX_IDENT_LEN + 1];
  ST_CHAR       multicastGroupIpAddr[MAX_IDENT_LEN + 1];

  /* Internal fields */
  ST_BOOLEAN          used;
  ST_INT              groupState;
  SLIP_IN_ADDR        sourceSockAddr;
  SLIP_IN_ADDR        multicastGroupSockAddr;
  } IP_SOURCE_MULTICAST_GROUP;

/************************************************************************/
/* GOOSE_TUNNEL_INFO                                                    */
/************************************************************************/

typedef struct
  {
  ST_UCHAR      destMac[ETHE_MAC_LEN];
  ST_UINT8      pcp;
  ST_UINT16     vlanId;
  } SLIP_GOOSE_TUNNEL_INFO;

/************************************************************************/
/* GOOSE_INFO                                                           */
/************************************************************************/

typedef struct
  {
  ST_BOOLEAN                  gooseInfoSpecified;
  ST_INT                      version;
  ST_INT                      type;
  ST_BOOLEAN                  simulated;
  ST_UINT16                   appId;
  SLIP_GOOSE_TUNNEL_INFO      tunnelInfo;
  } SLIP_GOOSE_INFO;

/************************************************************************/
/* SLIP_MESSAGE_HOP                                                     */
/************************************************************************/

#define SLIP_MESSAGE_HOP_TYPE_RECEIVED         1         /* message received on socket */
#define SLIP_MESSAGE_HOP_TYPE_DELIVERED        2         /* message delivered to application */
#define SLIP_MESSAGE_HOP_TYPE_QUEUED           3         /* message queued by application to send */

typedef struct
  {
  ST_INT        hopType;
  ST_DOUBLE     timestamp;
  ST_VOID       *pUserInfo;
  } SLIP_MESSAGE_HOP;

/************************************************************************/
/* SLIP_MESSAGE_TRANSIT_RECORD                                          */
/************************************************************************/

#define SLIP_MESSAGE_MAX_HOPS                  8

typedef struct
  {
  ST_UINT64           transitId;
  ST_INT              hopCount;
  SLIP_MESSAGE_HOP    hops[SLIP_MESSAGE_MAX_HOPS];
  ST_VOID             *pUserInfo;
  } SLIP_MESSAGE_TRANSIT_RECORD;

/************************************************************************/
/* SLIP_RECEIVE_MESSAGE                                                 */
/************************************************************************/

#define SLIP_MESSAGE_TYPE_UNICAST      0x0001
#define SLIP_MESSAGE_TYPE_MULTICAST    0x0002


#define SLIP_MESSAGE_TRUNCATED         0x0100
#define SLIP_MESSAGE_INTERNAL          0x0200
#define SLIP_MESSAGE_BROADCAST         0x0400
#define SLIP_MESSAGE_MULTICAST         0x0800
#define SLIP_MESSAGE_LOCAL             0x1000

typedef struct
  {
  DBL_LNK                     link;

  ST_VOID                     *pReceiveContext;
  ST_UCHAR                    destMac[ETHE_MAC_LEN];
  ST_UCHAR                    sourceMac[ETHE_MAC_LEN];
  SLIP_IN_ADDR                destSockAddr;
  SLIP_SOCKADDR_IN            sourceSockAddr;

  ST_INT32                    length;
  ST_UCHAR                    *pBuffer;

  ST_INT32                    frameLength;
  ST_UCHAR                    *pFrame;

  ST_UINT32                   flags;

  ST_UINT16                   etherType;

  ST_BOOLEAN                  pcpSpecified;
  ST_UINT8                    pcp;

  ST_BOOLEAN                  deiSpecified;
  ST_BOOLEAN                  dei;

  ST_BOOLEAN                  vlanIdSpecified;
  ST_UINT16                   vlanId;

  ST_BOOLEAN                  dscpSpecified;
  ST_UINT8                    dscp;

  ST_BOOLEAN                  ecnSpecified;
  ST_UINT8                    ecn;

  SLIP_GOOSE_INFO             gooseInfo;

#if defined (_WIN32)
  struct timeval              timestamp;
#endif
  ST_DOUBLE                   elapsed;

  SLIP_MESSAGE_TRANSIT_RECORD  transitRecord;
  ST_VOID                     *pUserInfo;
  } SLIP_RECEIVE_MESSAGE;

/************************************************************************/
/* SLIP_SEND_MESSAGE                                                    */
/************************************************************************/

typedef struct
  {
  DBL_LNK                     link;

  ST_VOID                     *pSendContext;
  ST_INT                      messagePriority;

  ST_BOOLEAN                  destMacSpecified;
  ST_UCHAR                    destMac[ETHE_MAC_LEN];

  ST_BOOLEAN                  destSockAddrSpecified;
  SLIP_SOCKADDR_IN            destSockAddr;

  ST_BOOLEAN                  sourceMacSpecified;
  ST_UCHAR                    sourceMac[ETHE_MAC_LEN];

  ST_BOOLEAN                  sourceSockAddrSpecified;
  SLIP_IN_ADDR                sourceSockAddr;

  ST_INT32                    length;
  ST_UCHAR                    *pBuffer;

  ST_BOOLEAN                  etherTypeSpecified;
  ST_UINT16                   etherType;

  ST_BOOLEAN                  pcpSpecified;
  ST_UINT8                    pcp;

  ST_BOOLEAN                  deiSpecified;
  ST_BOOLEAN                  dei;

  ST_BOOLEAN                  vlanIdSpecified;
  ST_UINT16                   vlanId;

  ST_BOOLEAN                  dscpSpecified;
  ST_UINT8                    dscp;

  ST_BOOLEAN                  ecnSpecified;
  ST_UINT8                    ecn;

  SLIP_GOOSE_INFO             gooseInfo;

  SLIP_MESSAGE_TRANSIT_RECORD  transitRecord;
  ST_VOID                     *pUserInfo;
  } SLIP_SEND_MESSAGE;

/************************************************************************/
/* SLIP_ERROR                                                           */
/************************************************************************/

#define MAX_REASON_LENGTH         256

typedef struct
  {
  ST_VOID       *pReceiveContext;
  ST_VOID       *pSendContext;

  ST_INT        slipError;
  ST_CHAR       slipReason[MAX_REASON_LENGTH + 1];

  ST_INT        systemError;
  ST_CHAR       systemReason[MAX_REASON_LENGTH + 1];
  } SLIP_ERROR;

/************************************************************************/
/* SLIP_ALERT                                                           */
/************************************************************************/

#define SLIP_ALERT_TYPE_SEND_ERROR                 1
#define SLIP_ALERT_TYPE_RECEIVE_ERROR              2
#define SLIP_ALERT_TYPE_RECEIVE_MESSAGE_DROPPED    3
#define SLIP_ALERT_TYPE_PROVIDER_FAULT             4
#define SLIP_ALERT_TYPE_RECEIVE_QUEUE_CAPACITY     5

#define SLIP_ALERT_STATE_NONE                      0
#define SLIP_ALERT_STATE_CONDITION_ENTERED         1
#define SLIP_ALERT_STATE_CONDITION_EXITED          2

typedef struct
  {
  ST_VOID       *pReceiveContext;
  ST_VOID       *pSendContext;
  ST_INT        type;
  ST_INT        state;
  ST_CHAR       slipReason[MAX_REASON_LENGTH + 1];

  ST_INT        systemError;
  ST_CHAR       systemReason[MAX_REASON_LENGTH + 1];
  } SLIP_ALERT;

/************************************************************************/
/* Callback Handler Prototypes                                          */
/************************************************************************/

typedef ST_INT (*SLIP_PEEK_RECEIVE_MESSAGE_HANDLER) (SLIP_RECEIVE_MESSAGE *pReceiveMessage);
typedef ST_VOID (*SLIP_RECEIVE_MESSAGE_HANDLER) (SLIP_RECEIVE_MESSAGE *pReceiveMessage);
typedef ST_VOID (*SLIP_SEND_MESSAGE_HANDLER) (SLIP_SEND_MESSAGE *pSendMessage);
typedef ST_VOID (*SLIP_ALERT_HANDLER) (SLIP_ALERT *pAlert);

/************************************************************************/
/************************************************************************/
/*      SLIP Interface functions                                        */
/************************************************************************/
/************************************************************************/

/************************************************************************/
/*      General functions                                               */
/************************************************************************/

ST_VOID slipAddLogMaskGroups ();

SLIP_ERROR *slipStart ();
SLIP_ERROR *slipCleanup ();

#define slipStop() slipCleanup()

ST_VOID *slipGetReceiveContextList ();
ST_VOID *slipGetSendContextList ();

SLIP_ERROR *slipAllocateReceiveContext (ST_INT provider, ST_INT protocol, ST_VOID **ppReceiveContext);
SLIP_ERROR *slipReleaseReceiveContext (ST_VOID *pReceiveContext);

SLIP_ERROR *slipAllocateSendContext (ST_INT provider, ST_INT protocol, ST_VOID **ppSendContext);
SLIP_ERROR *slipReleaseSendContext (ST_VOID *pSendContext);

SLIP_ERROR *slipAddMessageHop (SLIP_MESSAGE_TRANSIT_RECORD *pTransitRecord, ST_INT hopType, ST_VOID *pUserInfo);
SLIP_ERROR *slipCopyMessageTransitRecord (SLIP_MESSAGE_TRANSIT_RECORD *pDest, SLIP_MESSAGE_TRANSIT_RECORD *pSource);

ST_VOID slipSetPeekReceiveMessageHandler (SLIP_PEEK_RECEIVE_MESSAGE_HANDLER handler);
ST_VOID slipSetReceiveMessageHandler (SLIP_RECEIVE_MESSAGE_HANDLER handler);
ST_VOID slipReleaseReceiveMessage (SLIP_RECEIVE_MESSAGE *pMessage);

ST_VOID slipSetSendMessageHandler (SLIP_SEND_MESSAGE_HANDLER handler);
SLIP_ERROR *slipSendMessage (SLIP_SEND_MESSAGE *pMessage);

ST_VOID slipReleaseError (SLIP_ERROR *pError);

ST_VOID slipSetAlertHandler (SLIP_ALERT_HANDLER handler);
ST_VOID slipReleaseAlert (SLIP_ALERT *pAlert);

ST_VOID slipGetProviderString (ST_INT provider, ST_CHAR *providerString);
ST_VOID slipGetProtocolString (ST_INT protocol, ST_CHAR *protocolString);
ST_VOID slipGetReceiveMessageTypeString (ST_UINT receiveMessageType, ST_CHAR *typeString);
ST_VOID slipGetContextStateString (ST_INT contextState, ST_CHAR *contextStateString);
ST_VOID slipGetErrorString (ST_INT slipError, ST_CHAR *errorString);

ST_VOID slipGetAlertTypeString (ST_INT alertType, ST_CHAR *typeString);
ST_VOID slipGetAlertStateString (ST_INT alertState, ST_CHAR *stateString);

ST_BOOLEAN slipGetMacString (ST_UCHAR *mac, ST_CHAR *macString, ST_CHAR delim);
ST_BOOLEAN slipGetMac (ST_CHAR *macString, ST_UCHAR *mac);

ST_BOOLEAN slipGetIpAddrStringFromInAddr (IN_ADDR *ipAddr, ST_CHAR *ipAddrString, ST_INT maxLen);
ST_BOOLEAN slipGetIpAddrStringFromSockaddrIn (SOCKADDR_IN *ipAddr, ST_CHAR *ipAddrString, ST_INT maxLen);
ST_BOOLEAN slipGetIpAddrWithPort (ST_CHAR *ipAddrString, ST_INT port, SOCKADDR_IN *ipAddr);
ST_BOOLEAN slipGetIpAddr (ST_CHAR *ipAddrString, SOCKADDR_IN *ipAddr);
ST_BOOLEAN slipGetInAddr (ST_CHAR *ipAddrString, IN_ADDR *inAddr);

ST_BOOLEAN slipGetIpAddrStringFromIn6Addr (IN6_ADDR *ipAddr, ST_CHAR *ipAddrString, ST_INT maxLen);
ST_BOOLEAN slipGetIpAddrStringFromSockaddrIn6 (SOCKADDR_IN6 *ipAddr, ST_CHAR *ipAddrString, ST_INT maxLen);
ST_BOOLEAN slipGetIpv6AddrWithPort (ST_CHAR *ipAddrString, ST_INT port, SOCKADDR_IN6 *ipAddr);
ST_BOOLEAN slipGetIpv6Addr (ST_CHAR *ipAddrString, SOCKADDR_IN6 *ipAddr);
ST_BOOLEAN slipGetIn6Addr (ST_CHAR *ipAddrString, IN6_ADDR *inAddr);

ST_BOOLEAN slipGetIpAddrStringFromSlipInAddr (ST_INT ipVersion, SLIP_IN_ADDR *ipAddr, ST_CHAR *ipAddrString, ST_INT maxLen);
ST_BOOLEAN slipGetIpAddrStringFromSlipSockaddrIn (SLIP_SOCKADDR_IN *ipAddr, ST_CHAR *ipAddrString, ST_INT maxLen);

ST_BOOLEAN slipGetSlipSockAddrInWithPort (ST_CHAR *ipAddrString, ST_INT port, SLIP_SOCKADDR_IN *ipAddr);
ST_BOOLEAN slipGetSlipSockAddrIn (ST_CHAR *ipAddrString, SLIP_SOCKADDR_IN *ipAddr);
ST_BOOLEAN slipGetSlipInAddr (ST_CHAR *ipAddrString, SLIP_IN_ADDR *inAddr, ST_INT *ipVersion);

ST_BOOLEAN slipGetInAddrFromSockAddrIn (SLIP_IN_ADDR *inAddr, SLIP_SOCKADDR_IN *sockAddrIn);

ST_BOOLEAN slipSetSockAddrInAddr (SLIP_SOCKADDR_IN *sockAddrIn, SLIP_IN_ADDR *inAddr, ST_INT ipVersion);

ST_BOOLEAN slipSetSockAddrInPort (SLIP_SOCKADDR_IN *sockAddrIn, ST_INT port);

ST_BOOLEAN slipSockAddrInEqualsInAddr (SLIP_SOCKADDR_IN *sockAddrIn, SLIP_IN_ADDR *inAddr);

SLIP_RECEIVE_MESSAGE *slipDecodeGooseFrame (ST_UCHAR *pFrame, ST_INT frameLength);
SLIP_RECEIVE_MESSAGE *slipDecodeRgooseFrame (ST_UCHAR *pFrame, ST_INT frameLength);
SLIP_RECEIVE_MESSAGE *slipDecodeUdpFrame (ST_UCHAR *pFrame, ST_INT frameLength);
SLIP_RECEIVE_MESSAGE *slipDecodeIpFrame (ST_UCHAR *pFrame, ST_INT frameLength);

/************************************************************************/
/*      SLIP Context functions                                          */
/************************************************************************/

#define SLIP_MAX_ETHE_MESSAGE_SIZE                       ETHE_MAX_LEN_UDATA
#define SLIP_MAX_IP_MESSAGE_SIZE                         (65535 - IP_FRAME_HEADER_LENGTH)
#define SLIP_MAX_UDP_MESSAGE_SIZE                        (SLIP_MAX_IP_MESSAGE_SIZE - UDP_FRAME_HEADER_LENGTH)

#define SLIP_DEFAULT_SOCK_RECEIVE_BUFFER_SIZE            8192

#define SLIP_DEFAULT_RECEIVE_MESSAGE_TYPE                (SLIP_MESSAGE_TYPE_UNICAST | SLIP_MESSAGE_TYPE_MULTICAST)

#define SLIP_DEFAULT_RECEIVE_MESSAGE_QUEUE_SIZE          1000
#define SLIP_DEFAULT_RECEIVE_MESSAGE_SIZE                1600
#define SLIP_DEFAULT_MAX_PENDING_RECEIVES                10
#define SLIP_DEFAULT_MAX_MULTICAST_GROUPS                32
#define SLIP_DEFAULT_MAX_MULTICAST_MACS                  32
#define SLIP_DEFAULT_IGMP_VERSION                        3

#define SLIP_DEFAULT_SOCK_SEND_BUFFER_SIZE               8192
#define SLIP_DEFAULT_MAX_PENDING_SENDS                   10
#define SLIP_DEFAULT_SEND_TTL                            64
#define SLIP_DEFAULT_SEND_IP_SECURITY_OPTION             SD_FALSE
#define SLIP_DEFAULT_SEND_UDP_CHECKSUM                   SD_TRUE
#define SLIP_DEFAULT_SEND_VLAN_TAG                       SD_TRUE
#define SLIP_DEFAULT_SEND_PCP                            0
#define SLIP_DEFAULT_SEND_DEI                            SD_FALSE
#define SLIP_DEFAULT_SEND_VLANID                         0
#define SLIP_DEFAULT_SEND_DSCP                           0
#define SLIP_DEFAULT_SEND_ECN                            0
#define SLIP_DEFAULT_SEND_GOOSE_TYPE                     GOOSE_TYPE_GOOSE
#define SLIP_DEFAULT_SEND_RGOOSE_VERSION                 2
#define SLIP_DEFAULT_SEND_GOOSE_APPID                    1
#define SLIP_DEFAULT_SEND_GOOSE_SIMULATED                SD_FALSE
#define SLIP_DEFAULT_SEND_MODE                           SLIP_SEND_MODE_BASIC

#define SLIP_DEFAULT_THREAD_PRIORITY                     SLIP_THREAD_PRIORITY_NORMAL

#define SLIP_DEFAULT_MAX_TIMING_STATS_COUNT              100000

#define SLIP_INT_OPTION_ALLOC_SEND_BUFFER                1

#define SLIP_INT_OPTION_DEFAULT_ALLOC_SEND_BUFFER        0

#define SLIP_INT_OPTION_MAX                              1
#define SLIP_STRING_OPTION_MAX                           0

ST_INT slipContextGetSockReceiveBufferSize ();
SLIP_ERROR *slipContextSetSockReceiveBufferSize (ST_INT value);

ST_INT32 slipContextGetMaxReceiveMessageSize ();
SLIP_ERROR *slipContextSetMaxReceiveMessageSize (ST_INT32 value);

ST_INT slipContextGetMaxPendingReceives ();
SLIP_ERROR *slipContextSetMaxPendingReceives (ST_INT value);

ST_INT slipContextGetReceiveMessageQueueSize ();
SLIP_ERROR *slipContextSetReceiveMessageQueueSize (ST_INT value);

ST_INT slipContextGetReceiveBufferSize ();
SLIP_ERROR *slipContextSetReceiveBufferSize (ST_INT value);

ST_INT slipContextGetMaxMulticastGroups ();
SLIP_ERROR *slipContextSetMaxMulticastGroups (ST_INT value);

ST_INT slipContextGetMaxMulticastMacs ();
SLIP_ERROR *slipContextSetMaxMulticastMacs (ST_INT value);

ST_INT slipContextGetIgmpVersion ();
SLIP_ERROR *slipContextSetIgmpVersion (ST_INT value);

ST_INT slipContextGetSockSendBufferSize ();
SLIP_ERROR *slipContextSetSockSendBufferSize (ST_INT value);

ST_BOOLEAN slipContextGetSendUdpChecksum ();
ST_VOID slipContextSetSendUdpChecksum (ST_BOOLEAN value);

ST_BOOLEAN slipContextGetSendIpSecurityOption ();
ST_VOID slipContextSetSendIpSecurityOption (ST_BOOLEAN value);

ST_BOOLEAN slipContextGetSendVlanTag ();
ST_VOID slipContextSetSendVlanTag (ST_BOOLEAN value);

ST_INT slipContextGetSendTtl ();
SLIP_ERROR *slipContextSetSendTtl (ST_INT value);

ST_INT slipContextGetSendMode ();
SLIP_ERROR *slipContextSetSendMode (ST_INT value);

ST_INT slipContextGetMaxPendingSends ();
SLIP_ERROR *slipContextSetMaxPendingSends (ST_INT value);

ST_INT slipContextGetThreadPriority ();
SLIP_ERROR *slipContextSetThreadPriority (ST_INT value);

ST_BOOLEAN slipContextGetEnableStats ();
ST_VOID slipContextSetEnableStats (ST_BOOLEAN value);

ST_INT32 slipContextGetMaxTimingStatsCount ();
SLIP_ERROR *slipContextSetMaxTimingStatsCount (ST_INT32 value);

ST_DOUBLE slipContextGetInternalReactionTimeThreshold ();
SLIP_ERROR *slipContextSetInternalReactionTimeThreshold (ST_DOUBLE value);

ST_INT slipContextGetIntOption (ST_INT option);
SLIP_ERROR *slipContextSetIntOption (ST_INT option, ST_INT value);

ST_CHAR *slipContextGetStringOption (ST_INT option);
SLIP_ERROR *slipContextSetStringOption (ST_INT option, ST_CHAR *value);

/************************************************************************/
/*      SLIP Receive Context functions                                  */
/************************************************************************/

ST_INT slipReceiveContextGetProvider (ST_VOID *pReceiveContext);
ST_INT slipReceiveContextGetProtocol (ST_VOID *pReceiveContext);

ST_INT slipReceiveContextGetState (ST_VOID *pReceiveContext);

ST_INT slipReceiveContextGetIpVersion (ST_VOID *pReceiveContext);

ST_UINT slipReceiveContextGetReceiveMessageType (ST_VOID *pReceiveContext);
SLIP_ERROR *slipReceiveContextSetReceiveMessageType (ST_VOID *pReceiveContext, ST_UINT value);

ST_CHAR *slipReceiveContextGetEthernetAdapterName (ST_VOID *pReceiveContext);
SLIP_ERROR *slipReceiveContextSetEthernetAdapterName (ST_VOID *pReceiveContext, ST_CHAR *value);

ST_UCHAR *slipReceiveContextGetLocalMac (ST_VOID *pReceiveContext);

ST_INT slipReceiveContextGetEtherType (ST_VOID *pReceiveContext);
SLIP_ERROR *slipReceiveContextSetEtherType (ST_VOID *pReceiveContext, ST_INT value);

ST_INT *slipReceiveContextGetEtherTypes (ST_VOID *pReceiveContext, ST_INT *pCount);
SLIP_ERROR *slipReceiveContextSetEtherTypes (ST_VOID *pReceiveContext, ST_INT count, ST_INT *values);

ST_CHAR *slipReceiveContextGetLocalIpAddr (ST_VOID *pReceiveContext);
SLIP_ERROR *slipReceiveContextSetLocalIpAddr (ST_VOID *pReceiveContext, ST_CHAR *value);

ST_INT slipReceiveContextGetLocalPort (ST_VOID *pReceiveContext);
SLIP_ERROR *slipReceiveContextSetLocalPort (ST_VOID *pReceiveContext, ST_INT value);

SLIP_ERROR *slipReceiveContextAddMulticastMac (ST_VOID *pReceiveContext, ST_CHAR *value);
SLIP_ERROR *slipReceiveContextRemoveMulticastMac (ST_VOID *pReceiveContext, ST_CHAR *value);
SLIP_ERROR *slipReceiveContextClearMulticastMacs (ST_VOID *pReceiveContext);

SLIP_ERROR *slipReceiveContextAddMulticastGroup (ST_VOID *pReceiveContext, ST_CHAR *value);
SLIP_ERROR *slipReceiveContextRemoveMulticastGroup (ST_VOID *pReceiveContext, ST_CHAR *value);
SLIP_ERROR *slipReceiveContextClearMulticastGroups (ST_VOID *pReceiveContext);

SLIP_ERROR *slipReceiveContextAddSourceMulticastGroup (ST_VOID *pReceiveContext, IP_SOURCE_MULTICAST_GROUP *value);
SLIP_ERROR *slipReceiveContextRemoveSourceMulticastGroup (ST_VOID *pReceiveContext, IP_SOURCE_MULTICAST_GROUP *value);
SLIP_ERROR *slipReceiveContextClearSourceMulticastGroups (ST_VOID *pReceiveContext);

SLIP_ERROR *slipReceiveContextModifyMulticastFilter (ST_VOID *pReceiveContext);

SLIP_ERROR *slipReceiveContextSetFrameHeaderSize (ST_VOID *pReceiveContext, ST_INT32 headerSize);
SLIP_ERROR *slipReceiveContextSetFrameFooterSize (ST_VOID *pReceiveContext, ST_INT32 footerSize);

ST_VOID *slipReceiveContextGetUserInfo (ST_VOID *pReceiveContext);
SLIP_ERROR *slipReceiveContextSetUserInfo (ST_VOID *pReceiveContext, ST_VOID *value);

SLIP_ERROR *slipReceiveContextInitialize (ST_VOID *pReceiveContext);
SLIP_ERROR *slipReceiveContextTerminate (ST_VOID *pReceiveContext);

SLIP_ERROR *slipReceiveContextActivate (ST_VOID *pReceiveContext);
SLIP_ERROR *slipReceiveContextDeactivate (ST_VOID *pReceiveContext);

AD_CTX *slipReceiveContextGetAdapterContext (ST_VOID *pReceiveContext);

/************************************************************************/
/*      SLIP Send Context functions                                     */
/************************************************************************/

ST_INT slipSendContextGetProvider (ST_VOID *pSendContext);
ST_INT slipSendContextGetProtocol (ST_VOID *pSendContext);

ST_INT slipSendContextGetState (ST_VOID *pSendContext);

ST_INT slipSendContextGetIpVersion (ST_VOID *pSendContext);

ST_CHAR *slipSendContextGetEthernetAdapterName (ST_VOID *pSendContext);
SLIP_ERROR *slipSendContextSetEthernetAdapterName (ST_VOID *pSendContext, ST_CHAR *value);

ST_UCHAR *slipSendContextGetLocalMac (ST_VOID *pSendContext);

ST_CHAR *slipSendContextGetLocalIpAddr (ST_VOID *pSendContext);
SLIP_ERROR *slipSendContextSetLocalIpAddr (ST_VOID *pSendContext, ST_CHAR *value);

ST_CHAR *slipSendContextGetSourceMac (ST_VOID *pSendContext);
SLIP_ERROR *slipSendContextSetSourceMac (ST_VOID *pSendContext, ST_CHAR *value);

ST_CHAR *slipSendContextGetSourceIpAddr (ST_VOID *pSendContext);
SLIP_ERROR *slipSendContextSetSourceIpAddr (ST_VOID *pSendContext, ST_CHAR *value);

ST_CHAR *slipSendContextGetDefaultDestMac (ST_VOID *pSendContext);
SLIP_ERROR *slipSendContextSetDefaultDestMac (ST_VOID *pSendContext, ST_CHAR *value);

ST_INT slipSendContextGetDefaultEtherType (ST_VOID *pSendContext);
SLIP_ERROR *slipSendContextSetDefaultEtherType (ST_VOID *pSendContext, ST_INT value);

ST_CHAR *slipSendContextGetDefaultDestIpAddr (ST_VOID *pSendContext);
SLIP_ERROR *slipSendContextSetDefaultDestIpAddr (ST_VOID *pSendContext, ST_CHAR *value);

ST_INT slipSendContextGetDefaultDestPort (ST_VOID *pSendContext);
SLIP_ERROR *slipSendContextSetDefaultDestPort (ST_VOID *pSendContext, ST_INT value);

ST_UINT8 slipSendContextGetDefaultPcp (ST_VOID *pSendContext);
SLIP_ERROR *slipSendContextSetDefaultPcp (ST_VOID *pSendContext, ST_UINT8 value);

ST_BOOLEAN slipSendContextGetDefaultDei (ST_VOID *pSendContext);
SLIP_ERROR *slipSendContextSetDefaultDei (ST_VOID *pSendContext, ST_BOOLEAN value);

ST_UINT16 slipSendContextGetDefaultVlanId (ST_VOID *pSendContext);
SLIP_ERROR *slipSendContextSetDefaultVlanId (ST_VOID *pSendContext, ST_UINT16 value);

ST_UINT8 slipSendContextGetDefaultDscp (ST_VOID *pSendContext);
SLIP_ERROR *slipSendContextSetDefaultDscp (ST_VOID *pSendContext, ST_UINT8 value);

ST_UINT8 slipSendContextGetDefaultEcn (ST_VOID *pSendContext);
SLIP_ERROR *slipSendContextSetDefaultEcn (ST_VOID *pSendContext, ST_UINT8 value);

SLIP_GOOSE_INFO *slipSendContextGetDefaultGooseInfo (ST_VOID *pSendContext);
SLIP_ERROR *slipSendContextSetDefaultGooseInfo (ST_VOID *pSendContext, SLIP_GOOSE_INFO *value);

ST_VOID *slipSendContextGetUserInfo (ST_VOID *pSendContext);
SLIP_ERROR *slipSendContextSetUserInfo (ST_VOID *pSendContext, ST_VOID *value);

SLIP_ERROR *slipSendContextInitialize (ST_VOID *pSendContext);
SLIP_ERROR *slipSendContextTerminate (ST_VOID *pSendContext);

SLIP_ERROR *slipSendContextActivate (ST_VOID *pSendContext);
SLIP_ERROR *slipSendContextDeactivate (ST_VOID *pSendContext);

ST_INT32 slipSendContextGetSendMessageHeaderSize (ST_VOID *pSendContext);
ST_INT32 slipSendContextGetSendMessageFooterSize (ST_VOID *pSendContext);

AD_CTX *slipSendContextGetAdapterContext (ST_VOID *pSendContext);

SLIP_ERROR *slipSendContextAllocateSendMessage (ST_VOID *pSendContext, ST_INT32 messageLength, ST_UCHAR *pBuffer, SLIP_SEND_MESSAGE **ppMessage);
ST_VOID slipReleaseSendMessage (SLIP_SEND_MESSAGE *pSendMessage);

/************************************************************************/
/*      Stats functions                                                 */
/************************************************************************/

#define SLIP_STATS_TYPE_UNUSED         0
#define SLIP_STATS_TYPE_TIMING         1
#define SLIP_STATS_TYPE_COUNTER        2
#define SLIP_STATS_TYPE_LAST_TIMING    4
#define SLIP_STATS_TYPE_LAST_COUNTER   5
#define SLIP_STATS_TYPE_LAST           6

#define SLIP_STATS_TYPE_ANY            100

#define SLIP_STATS_MAX_NAME_LENGTH     128

#define SLIP_STATS_ITEM_UNDEFINED                                    0
#define SLIP_STATS_ITEM_MESSAGES_SENT                                1       /* Counter */
#define SLIP_STATS_ITEM_MESSAGES_RECEIVED                            2       /* Counter */
#define SLIP_STATS_ITEM_SEND_ERRORS                                  3       /* Counter */
#define SLIP_STATS_ITEM_RECEIVE_ERRORS                               4       /* Counter */
#define SLIP_STATS_ITEM_RECEIVE_MESSAGES_DROPPED                     5       /* Counter */
#define SLIP_STATS_ITEM_RECEIVE_QUEUE_SIZE                           6       /* Timing */
#define SLIP_STATS_ITEM_SEND_MESSAGE_SIZE                            7       /* Timing */
#define SLIP_STATS_ITEM_RECEIVE_MESSAGE_SIZE                         8       /* Timing */
#define SLIP_STATS_ITEM_TIME_ON_SEND_QUEUE                           9       /* Timing */
#define SLIP_STATS_ITEM_TIME_ON_RECEIVE_QUEUE                        10      /* Timing */
#define SLIP_STATS_ITEM_TIME_TO_SEND                                 11      /* Timing */
#define SLIP_STATS_ITEM_TOTAL_SEND_TIME                              12      /* Timing */
#define SLIP_STATS_ITEM_INTERNAL_REACTION_TIME                       13      /* Timing */


typedef struct
  {
  ST_INT            statsType;
  ST_INT            id;
  ST_CHAR           name[SLIP_STATS_MAX_NAME_LENGTH + 1];
  STATS_TIMING      timing;
  STATS_COUNTER     counter;
  } SLIP_STATS_ITEM;

SLIP_STATS_ITEM *slipStatsGetItems (ST_INT statsType);
SLIP_STATS_ITEM *slipStatsGetItemById (ST_INT id);
SLIP_STATS_ITEM *slipStatsGetItemByName (ST_CHAR *pName);

ST_VOID slipStatsReset ();
ST_VOID slipStatsResetPeriod ();

/************************************************************************/
/*      Log functions                                                   */
/************************************************************************/

ST_VOID slipLogAll ();
ST_VOID slipLogSlipContext ();
ST_VOID slipLogReceiveContext (ST_VOID *pReceiveContext);
ST_VOID slipLogSendContext (ST_VOID *pSendContext);
ST_VOID slipLogReceiveMessage (SLIP_RECEIVE_MESSAGE *pReceiveMessage);
ST_VOID slipLogSendMessage (SLIP_SEND_MESSAGE *pSendMessage, ST_UCHAR *pFrame);

ST_VOID slipTraceTransitRecord (SLIP_MESSAGE_TRANSIT_RECORD *pTransitRecord);

ST_VOID slipLogError (SLIP_ERROR *pError);
ST_VOID slipLogAlert (SLIP_ALERT *pAlert);

#ifdef __cplusplus
}
#endif

#endif