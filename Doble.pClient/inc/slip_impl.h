/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*      (c) Copyright Systems Integration Specialists Company, Inc.,    */
/*      2017, All Rights Reserved                                       */
/*                                                                      */
/* MODULE NAME : slip_impl.h                                            */
/* PRODUCT(S)  : SLIP API                                               */
/*                                                                      */
/* MODULE DESCRIPTION :                                                 */
/*      Structures and functions for SLIP API.                          */
/*                                                                      */
/* MODIFICATION LOG :                                                   */
/*  Date     Who                        Comments                        */
/* --------  ---  ----------------------------------------------------  */
/* 03/15/17  DSF  Added Int and String Context options                  */
/* 01/25/16  DSF  Initial Release                                       */
/************************************************************************/

#ifndef SLIP_IMPL_INCLUDED
#define SLIP_IMPL_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "slip.h"

/************************************************************************/
/* Implementation structs                                               */
/************************************************************************/

#define MAX_ADAPTER_LEN           255

/************************************************************************/
/* DBL_LNK_PTR                                                          */
/************************************************************************/

typedef struct
  {
  DBL_LNK     link;
  ST_VOID     *ptr;
  } DBL_LNK_PTR;


/************************************************************************/
/* MULTICAST_MAC                                                        */
/************************************************************************/

typedef struct
  {
  ST_BOOLEAN    used;
  ST_CHAR       multicastMac[MAX_IDENT_LEN + 1];
  ST_UCHAR      multicastMacHex[ETHE_MAC_LEN];
  } MULTICAST_MAC;

/************************************************************************/
/* IP_MULTICAST_GROUP                                                   */
/************************************************************************/

typedef struct
  {
  ST_BOOLEAN    used;
  ST_INT        groupState;
  ST_CHAR       multicastGroupIpAddr[MAX_IDENT_LEN + 1];
  SLIP_IN_ADDR  multicastGroupSockAddr;
  } IP_MULTICAST_GROUP;

/************************************************************************/
/* SLIP_ETHERNET_ADAPTER_CONTEXT                                        */
/************************************************************************/

typedef struct
  {
  ST_BOOLEAN            active;
  ST_CHAR               adapterName[MAX_ADAPTER_LEN + 1];
  SLIP_IN_ADDR          ipAddr;
  CLNP_PARAM            clnpParam;
  AD_CTX                *pAdCtx;
  GS_THREAD_CONTEXT     *pReceiveThreadContext;
  ST_VOID               *pUserContext;
  } SLIP_ETHERNET_ADAPTER_CONTEXT;

#define MAX_ETHERNET_ADAPTERS               8
#define MAX_ETHERNET_ADAPTER_CONTEXTS       (MAX_ETHERNET_ADAPTERS * 2)   /* GOOSE + RGOOSE context per adapter */

/************************************************************************/
/* SLIP_INT_OPTION                                                      */
/************************************************************************/

typedef struct
  {
  ST_BOOLEAN            used;
  ST_INT                option;
  ST_INT                value;
  } SLIP_INT_OPTION;

/************************************************************************/
/* SLIP_STRING_OPTION                                                   */
/************************************************************************/

#define MAX_STRING_OPTION_SIZE        128

typedef struct
  {
  ST_BOOLEAN            used;
  ST_INT                option;
  ST_CHAR               value[MAX_STRING_OPTION_SIZE + 1];
  } SLIP_STRING_OPTION;

#define MAX_CONTEXT_OPTIONS           32

/************************************************************************/
/* SLIP_RECEIVE_CONTEXT                                                 */
/************************************************************************/

#define MAX_ETHER_TYPES             10

typedef struct slip_receive_context
  {
  DBL_LNK                         link;

  ST_INT                          provider;
  ST_INT                          protocol;

  ST_INT                          contextState;

  ST_UINT                         receiveMessageType;

  ST_CHAR                         adapterName[MAX_ADAPTER_LEN + 1];

  ST_UCHAR                        localMac[ETHE_MAC_LEN];

  ST_INT                          etherTypes[MAX_ETHER_TYPES];

  ST_INT                          ipVersion;

  ST_CHAR                         localIpAddr[MAX_IDENT_LEN + 1];
  ST_INT                          localPort;
  SLIP_IN_ADDR                    localSockAddr;

  ST_INT                          maxMulticastMacs;
  ST_INT                          multicastMacCount;
  MULTICAST_MAC                   *multicastMacs;

  ST_INT                          maxMulticastGroups;
  ST_INT                          multicastGroupCount;
  IP_MULTICAST_GROUP              *multicastGroups;

  ST_INT                          maxSourceMulticastGroups;
  ST_INT                          sourceMulticastGroupCount;
  IP_SOURCE_MULTICAST_GROUP       *sourceMulticastGroups;

  ST_INT32                        frameHeaderSize;
  ST_INT32                        frameFooterSize;

  ST_VOID                         *pUserInfo;

  union
    {
    struct
      {
      SOCKET                          socket;
#if defined (_WIN32)
      LPFN_WSARECVMSG                 pWSARecvMsg;

      GS_THREAD_CONTEXT               *pReceiveContextThreadContext;

      ST_INT                          pendingReceiveMessageCount;
      SLIP_RECEIVE_MESSAGE            *pendingReceiveMessages;
#endif
      } sock;

    struct
      {
      SLIP_ETHERNET_ADAPTER_CONTEXT   *pAdapterContext;
      struct slip_receive_context     *pSockReceiveContext;
      } pcap;
    } u;
  } SLIP_RECEIVE_CONTEXT;

/************************************************************************/
/* SLIP_RECEIVE_MESSAGE_CTRL                                            */
/************************************************************************/

typedef struct
  {
  SLIP_RECEIVE_CONTEXT              *pReceiveContext;
  ST_BOOLEAN                        decodeFailed;
  union
    {
#if defined (_WIN32)
    struct
      {
      WSAMSG                          msg;
      WSABUF                          buf;
      SLIP_SOCKADDR_IN                sourceSockAddr;
      DWORD                           bytesTransferred;
      DWORD                           flags;
      WSAOVERLAPPED                   overlapped;
      } sock;
#endif

    struct
      {
      ST_UCHAR                        *pFrame;
      size_t                          frameLength;

      ST_UCHAR                        *pCurrentByte;
      ST_INT32                        dataLength;
      ST_INT32                        frameLengthRemainder;
      } pcap;
    } u;
  } SLIP_RECEIVE_MESSAGE_CTRL;

/************************************************************************/
/* SLIP_SEND_CONTEXT                                                    */
/************************************************************************/

typedef struct
  {
  DBL_LNK           link;

  ST_INT            provider;
  ST_INT            protocol;

  ST_INT            contextState;

  ST_CHAR           adapterName[MAX_ADAPTER_LEN + 1];

  ST_UCHAR          localMac[ETHE_MAC_LEN];

  ST_INT            ipVersion;

  ST_CHAR           localIpAddr[MAX_IDENT_LEN + 1];
  ST_INT            localPort;
  SLIP_IN_ADDR      localSockAddr;

  ST_CHAR           sourceMac[MAX_IDENT_LEN + 1];
  ST_UCHAR          sourceMacHex[ETHE_MAC_LEN];

  ST_CHAR           sourceIpAddr[MAX_IDENT_LEN + 1];
  SLIP_IN_ADDR      sourceSockAddr;

  ST_CHAR           sourceIpMask[MAX_IDENT_LEN + 1];

  ST_CHAR           defaultDestMac[MAX_IDENT_LEN + 1];
  ST_UCHAR          defaultDestMacHex[ETHE_MAC_LEN];

  ST_INT            defaultEtherType;

  ST_CHAR           defaultDestIpAddr[MAX_IDENT_LEN + 1];
  ST_INT            defaultDestPort;
  SLIP_SOCKADDR_IN  defaultDestSockAddr;

  ST_UINT8          defaultPcp;
  ST_BOOLEAN        defaultDei;
  ST_UINT16         defaultVlanId;
  ST_UINT8          defaultDscp;
  ST_UINT8          defaultEcn;

  SLIP_GOOSE_INFO   defaultGooseInfo;

  ST_VOID           *pUserInfo;

  ST_BOOLEAN        encodeInfoMessageLogged;

  union
    {
#if defined (_WIN32)
    struct
      {
      SOCKET            socket;
      } sock;
#endif

    struct
      {
      SLIP_ETHERNET_ADAPTER_CONTEXT    *pAdapterContext;
      ST_UINT16         ipId;
      ST_UINT32         spduId;
      } pcap;
    } u;
  } SLIP_SEND_CONTEXT;

/************************************************************************/
/* SLIP_SEND_MESSAGE_HEADER                                             */
/************************************************************************/

typedef struct
  {
  ST_INT32          length;
  ST_UCHAR          *pBuffer;
  ST_UCHAR          *pFrameBuffer;
  ST_INT32          footerLength;
  ST_UCHAR          *pFrameFooter;
  ST_UCHAR          *pInternalBuffer;
  ST_UINT64         writeId;
  ST_UCHAR          destMac[ETHE_MAC_LEN];
  SLIP_SOCKADDR_IN  destSockAddr;
  ST_UCHAR          sourceMac[ETHE_MAC_LEN];
  SLIP_IN_ADDR      sourceSockAddr;
  } SLIP_SEND_MESSAGE_HEADER;

/************************************************************************/
/* SLIP_CONTEXT                                                         */
/************************************************************************/

#define SLIP_MAX_COUNTER_STATS       20
#define SLIP_MAX_TIMING_STATS        20

typedef struct
  {
  ST_BOOLEAN                          enableReceiveFlowControl;
  ST_INT                              sockReceiveBufferSize;
  ST_INT32                            maxReceiveMessageSize;
  ST_INT                              maxPendingReceives;
  ST_INT                              receiveMessageQueueSize;
  ST_INT                              receiveBufferSize;
  ST_INT                              igmpVersion;
  ST_INT                              maxMulticastGroups;
  ST_INT                              maxMulticastMacs;
  SLIP_PEEK_RECEIVE_MESSAGE_HANDLER   peekReceiveMessageHandler;
  SLIP_RECEIVE_MESSAGE_HANDLER        receiveMessageHandler;

  ST_INT                              sockSendBufferSize;
  ST_BOOLEAN                          sendUdpChecksum;
  ST_BOOLEAN                          sendIpSecurityOption;
  ST_BOOLEAN                          sendVlanTag;
  ST_INT                              sendTtl;
  ST_INT                              sendMode;
  ST_INT                              maxPendingSends;
  SLIP_SEND_MESSAGE_HANDLER           sendMessageHandler;

  ST_INT                              threadPriority;

  SLIP_ALERT_HANDLER                  alertHandler;

  ST_BOOLEAN                          enableStats;
  ST_INT32                            maxTimingStatsCount;
  ST_DOUBLE                           internalReactionTimeThreshold;

  SLIP_INT_OPTION                     intOptions[MAX_CONTEXT_OPTIONS];
  SLIP_STRING_OPTION                  stringOptions[MAX_CONTEXT_OPTIONS];

  struct
    {
    struct
      {
      ST_BOOLEAN                      initialized;
      GEN_SOCK_CTXT                   sockContext;
      } sock;

    struct
      {
      SLIP_ETHERNET_ADAPTER_CONTEXT   adapterContexts[MAX_ETHERNET_ADAPTER_CONTEXTS];
      } pcap;
    } s;
  SLIP_RECEIVE_CONTEXT                *receiveContexts;
  SLIP_SEND_CONTEXT                   *sendContexts;

  ST_BOOLEAN                          started;

  GS_THREAD_CONTEXT                   *pReceiveQueueThreadContext;
  ST_EVENT_SEM                        receiveEvent;
  ST_SEMAPHORE                        receiveSemaphore;
  SLIP_RECEIVE_MESSAGE                *normalReceiveMessages;
  SLIP_RECEIVE_MESSAGE                *highReceiveMessages;

  GS_THREAD_CONTEXT                   *pSendThreadContext;
  ST_EVENT_SEM                        sendEvent;
  ST_SEMAPHORE                        sendSemaphore;
  SLIP_SEND_MESSAGE                   *normalSendMessages;
  SLIP_SEND_MESSAGE                   *highSendMessages;

  ST_BOOLEAN                          receiveQueueCapacityConditionEntered;

  SLIP_STATS_ITEM                     stats[SLIP_MAX_COUNTER_STATS + SLIP_MAX_TIMING_STATS + 3];
  SLIP_STATS_ITEM                     *pStatsCounters;
  SLIP_STATS_ITEM                     *pStatsTimings;

  SLIP_STATS_ITEM                     *pMessagesSentStatsItem;
  SLIP_STATS_ITEM                     *pMessagesReceivedStatsItem;
  SLIP_STATS_ITEM                     *pSendErrorsStatsItem;
  SLIP_STATS_ITEM                     *pReceiveErrorsStatsItem;
  SLIP_STATS_ITEM                     *pReceiveMessagesDroppedStatsItem;
  SLIP_STATS_ITEM                     *pReceiveQueueSizeStatsItem;
  SLIP_STATS_ITEM                     *pSendMessageSizeStatsItem;
  SLIP_STATS_ITEM                     *pReceiveMessageSizeStatsItem;
  SLIP_STATS_ITEM                     *pTimeOnSendQueueStatsItem;
  SLIP_STATS_ITEM                     *pTimeOnReceiveQueueStatsItem;
  SLIP_STATS_ITEM                     *pTimeToSendStatsItem;
  SLIP_STATS_ITEM                     *pTotalSendTimeStatsItem;
  SLIP_STATS_ITEM                     *pInternalReactionTimeStatsItem;
  } SLIP_CONTEXT;

/************************************************************************/
/* Implementation functions                                             */
/************************************************************************/

#define _slipAllocateError() \
  (SLIP_ERROR *) chk_calloc (1, sizeof (SLIP_ERROR))

#define _slipAllocateAlert() \
    (SLIP_ALERT *) chk_calloc (1, sizeof (SLIP_ALERT))

SLIP_CONTEXT *_slipGetSlipContext ();
ST_VOID _slipReleaseSlipContext ();

SLIP_ERROR *_slipStartWinsock ();
SLIP_ERROR *_slipStartPcap ();
ST_VOID _slipCleanupWinsock ();
ST_VOID _slipCleanupPcap ();

ST_BOOLEAN _slipCheckProviderProtocol (ST_INT provider, ST_INT protocol);

ST_VOID _slipReleaseReceiveContexts ();
ST_VOID _slipReleaseSendContexts ();

SLIP_RECEIVE_CONTEXT *_slipFindReceiveContext (SLIP_RECEIVE_CONTEXT *pReceiveContext);
SLIP_SEND_CONTEXT *_slipFindSendContext (SLIP_SEND_CONTEXT *pSendContext);

ST_BOOLEAN _slipReceiveQueueThread (GS_THREAD_CONTEXT *pThreadContext);

SLIP_ERROR *_slipReceiveContextInitializeSock (SLIP_RECEIVE_CONTEXT *pReceiveContext);
SLIP_ERROR *_slipReceiveContextTerminateSock (SLIP_RECEIVE_CONTEXT *pReceiveContext);
SLIP_ERROR *_slipReceiveContextActivateSock (SLIP_RECEIVE_CONTEXT *pReceiveContext);
SLIP_ERROR *_slipReceiveContextDeactivateSock (SLIP_RECEIVE_CONTEXT *pReceiveContext);
SLIP_ERROR *_slipReceiveContextModifyMulticastFilterSock (SLIP_RECEIVE_CONTEXT *pReceiveContext);

SLIP_ERROR *_slipReceiveContextInitializePcap (SLIP_RECEIVE_CONTEXT *pReceiveContext);
SLIP_ERROR *_slipReceiveContextTerminatePcap (SLIP_RECEIVE_CONTEXT *pReceiveContext);
SLIP_ERROR *_slipReceiveContextActivatePcap (SLIP_RECEIVE_CONTEXT *pReceiveContext);
SLIP_ERROR *_slipReceiveContextDeactivatePcap (SLIP_RECEIVE_CONTEXT *pReceiveContext);
SLIP_ERROR *_slipReceiveContextModifyMulticastFilterPcap (SLIP_RECEIVE_CONTEXT *pReceiveContext);

ST_BOOLEAN _slipPcapReceiveThread (GS_THREAD_CONTEXT *pThreadContext);

SLIP_ERROR *_slipSendContextInitializeSock (SLIP_SEND_CONTEXT *pSendContext);
SLIP_ERROR *_slipSendContextTerminateSock (SLIP_SEND_CONTEXT *pSendContext);
SLIP_ERROR *_slipSendContextActivateSock (SLIP_SEND_CONTEXT *pSendContext);
SLIP_ERROR *_slipSendContextDeactivateSock (SLIP_SEND_CONTEXT *pSendContext);

SLIP_ERROR *_slipSendContextInitializePcap (SLIP_SEND_CONTEXT *pSendContext);
SLIP_ERROR *_slipSendContextTerminatePcap (SLIP_SEND_CONTEXT *SendContext);
SLIP_ERROR *_slipSendContextActivatePcap (SLIP_SEND_CONTEXT *pSendContext);
SLIP_ERROR *_slipSendContextDeactivatePcap (SLIP_SEND_CONTEXT *pSendContext);

ST_BOOLEAN _slipSendThread (GS_THREAD_CONTEXT *pThreadContext);

SLIP_ERROR *_slipPreprocessSendMessage (SLIP_SEND_MESSAGE *pMessage);

SLIP_ERROR *_slipPreprocessSendMessageSock (SLIP_SEND_MESSAGE *pMessage);
ST_VOID _slipSendMessageSock (SLIP_SEND_MESSAGE *pMessage);

SLIP_ERROR *_slipPreprocessSendMessagePcap (SLIP_SEND_MESSAGE *pMessage);
ST_VOID _slipSendMessagePcap (SLIP_SEND_MESSAGE *pMessage);

ST_BOOLEAN _slipPostprocessReceiveMessage (SLIP_RECEIVE_MESSAGE *pMessage);

SLIP_ERROR *_slipBindSocket (SOCKET socket, ST_CHAR *pIpAddr, ST_INT port);

SLIP_ERROR *_slipGetInterfaceIndex (ST_CHAR *pAdapterName, ST_INT *index);

SLIP_ERROR *_slipGetAddrInfo (SLIP_SOCKADDR_IN *pSockAddr, ST_CHAR *pIpAddr, ST_INT port);
SLIP_ERROR *_slipGetInAddrInfo (SLIP_IN_ADDR *pSockAddr, ST_CHAR *pIpAddr, ST_INT *pIpVersion);

ST_BOOLEAN _slipIsMulticastIpAddr (SLIP_SOCKADDR_IN *pSockAddr);
SLIP_ERROR *_slipGetMulticastMac (ST_UCHAR *mac, SLIP_SOCKADDR_IN *pSockAddr);

ST_BOOLEAN _slipIsMulticastIpv4Addr (SOCKADDR_IN *pSockAddr);
SLIP_ERROR *_slipGetIpv4MulticastMac (ST_UCHAR *mac, SOCKADDR_IN *pSockAddr);
ST_BOOLEAN _slipIsMulticastIpv6Addr (SOCKADDR_IN6 *pSockAddr);
SLIP_ERROR *_slipGetIpv6MulticastMac (ST_UCHAR *mac, SOCKADDR_IN6 *pSockAddr);

SLIP_ETHERNET_ADAPTER_CONTEXT *_slipFindFreeAdapterContext ();
ST_VOID _slipReleaseAdapterContext (SLIP_ETHERNET_ADAPTER_CONTEXT *pAdapterContext);
SLIP_ERROR *_slipOpenAdapterContext (SLIP_ETHERNET_ADAPTER_CONTEXT *pAdapterContext, ST_BOOLEAN receive, ST_VOID *userContext);
SLIP_ERROR *_slipCloseAdapterContext (SLIP_ETHERNET_ADAPTER_CONTEXT *pAdapterContext);

SLIP_ERROR *_slipGetAdapterLocalMac (SLIP_ETHERNET_ADAPTER_CONTEXT *pAdapterContext, ST_CHAR *localMac);

ST_INT64 _slipGetWriteId ();
ST_INT64 _slipGetTransitId ();
ST_INT _slipGetEphemeralPort ();

SLIP_ERROR *_slipCopyMessageHop (SLIP_MESSAGE_TRANSIT_RECORD *pTransitRecord, SLIP_MESSAGE_HOP *pHop);

ST_INT _slipGetSystemError ();
ST_VOID _slipGetSystemReason (ST_INT errCode, ST_CHAR *pBuffer, ST_INT bufferSize);

DBL_LNK_PTR *_slipFindPtr (DBL_LNK_PTR *pHead, ST_VOID *ptr);

ST_BOOLEAN _slipCheckZeroMemory (ST_VOID *pMemory, ST_INT32 memorySize);

#ifdef __cplusplus
}
#endif
#endif