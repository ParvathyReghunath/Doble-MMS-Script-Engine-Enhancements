/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,       */
/*          2003 - 2004, All Rights Reserved                            */
/*                                                                      */
/* MODULE NAME : sslThrds.h                                             */
/* PRODUCT(S)  : Sisco MMS Security Toolkit                             */
/*                                                                      */
/* MODULE DESCRIPTION :                                                 */
/*                                                                      */
/* This is the Sisco SSL Toolkit thread header file. This is used by the*/
/* OpenSSL libraries to support multiple threads.                       */
/*                                                                      */
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :                            */
/*                                                                      */
/* MODIFICATION LOG :                                                   */
/*  Date     Who   Rev                     Comments                     */
/* --------  ---  ------   -------------------------------------------  */
/* 02/12/04  ASK    02     Use pthreads for non-Win32 platforms         */
/* 08/22/02  ASK    01     Created                                      */
/************************************************************************/

#ifndef SSLE_THRD_INCLUDED
#define SSLE_THRD_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

/* References to page numbers refer to the book, "Network Security with */
/* OpenSSL", first edition, 2002.                                       */

#if defined(_WIN32)
#include <windows.h>
#endif

/* Thread Infrastructure, required for OpenSSL's multithreading features*/
/* refer to pages 76 - 77                                               */
#if defined(_WIN32)
#define MUTEX_TYPE HANDLE
#define MUTEX_SETUP(x)    (x) = CreateMutex(NULL, FALSE, NULL)
#define MUTEX_CLEANUP(x)  CloseHandle(x)
#define MUTEX_LOCK(x)     WaitForSingleObject((x), INFINITE)
#define MUTEX_UNLOCK(x)   ReleaseMutex(x)
#define THREAD_ID         GetCurrentThreadId()
#else
#define MUTEX_TYPE        pthread_mutex_t
#define MUTEX_SETUP(x)    pthread_mutex_init(&(x), NULL)
#define MUTEX_CLEANUP(x)  pthread_mutex_destroy(&(x))
#define MUTEX_LOCK(x)     pthread_mutex_lock(&(x))
#define MUTEX_UNLOCK(x)   pthread_mutex_unlock(&(x))
#define THREAD_ID         pthread_self()
#endif

/************************************************************************/
/*                   Functions defined in sslbbThd.c                    */
/************************************************************************/

ST_RET _thread_setup(ST_VOID);
ST_RET _thread_cleanup(ST_VOID);

/************************************************************************/
#ifdef __cplusplus
  }
#endif

#endif /* SSLE_THRD_INCLUDED */
/************************************************************************/
