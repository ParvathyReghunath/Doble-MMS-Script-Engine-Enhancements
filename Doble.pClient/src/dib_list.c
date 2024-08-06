/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*      (c) Copyright Systems Integration Specialists Company, Inc.,    */
/*      2011 - 2017, All Rights Reserved                                */
/*                                                                      */
/* MODULE NAME : dib_list.c                                             */
/* PRODUCT(S)  : MMSEASE-LITE                                           */
/*                                                                      */
/* MODULE DESCRIPTION :                                                 */
/*      DIB management functions (for storing and finding addresses).   */
/*                                                                      */
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :                            */
/*                      find_dib_entry                                  */
/*                      dib_entry_create                                */
/*                      dib_entry_destroy                               */
/*                      dib_entry_destroy_all                           */
/*                      mvl_get_listen_ipv4_addr                        */
/*                      mvl_get_listen_ipv6_addr                        */
/*                                                                      */
/* MODIFICATION LOG :                                                   */
/*  Date     Who  Comments                                              */
/* --------  ---  ----------------------------------------------------  */
/* 01/09/17  JRB  Added mvl_get_listen_ipv4(ipv6)_addr functions.       */
/*                dib_entry_create: return error for duplicate entry.   */
/* 09/15/11  JRB  New                                                   */
/************************************************************************/
#include "glbtypes.h"
#include "sysincs.h"
#include "lean_a.h"
#include "sx_log.h"

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of      */
/* __FILE__ strings.                                                    */
/************************************************************************/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/* Global DIB linked list. Only accessed by functions in this file.     */
DIB_ENTRY *dib_entry_list;      /* linked list of all DIB entries.      */

/************************************************************************/
/*                      find_dib_entry                                  */
/************************************************************************/
DIB_ENTRY *find_dib_entry (ST_CHAR *arname)
  {
  DIB_ENTRY *dib_entry;
  for (dib_entry = dib_entry_list;
       dib_entry != NULL;
       dib_entry = list_get_next (dib_entry_list, dib_entry))
    {
    if (strcmp (dib_entry->name, arname) == 0)
      break;    /* found a match        */
    }
  return (dib_entry);
  }

/************************************************************************/
/*                      dib_entry_create                                */
/* Allocate a struct, copy dib_entry to it, add it to global linked list.*/
/************************************************************************/
ST_RET dib_entry_create (DIB_ENTRY *dib_entry)
  {
  DIB_ENTRY *new_dib_entry;
  if (find_dib_entry(dib_entry->name) != NULL)
    return (SD_FAILURE);  /* duplicate entry not allowed  */
  new_dib_entry = chk_malloc (sizeof (DIB_ENTRY));
  memcpy (new_dib_entry, dib_entry, sizeof (DIB_ENTRY));
  list_add_last (&dib_entry_list, new_dib_entry);
  return (SD_SUCCESS);
  }

/************************************************************************/
/*                      dib_entry_destroy                               */
/* Remove dib_entry from global linked list and free it.                */
/************************************************************************/
ST_VOID dib_entry_destroy (DIB_ENTRY *dib_entry)
  {
  list_unlink (&dib_entry_list, dib_entry);     /* remove from list     */
  chk_free (dib_entry);
  }

/************************************************************************/
/*                      dib_entry_destroy_all                           */
/* Destroy all DIB entries.                                             */
/************************************************************************/
ST_VOID dib_entry_destroy_all ()
  {
  DIB_ENTRY *dib_entry;
  /* Destroy first entry on the list, until list is empty (NULL).       */
  while ((dib_entry = dib_entry_list) != NULL)
    {
    dib_entry_destroy (dib_entry);      /* remove from list AND free it*/
    }
  }

/************************************************************************/
/*                      mvl_get_listen_ipv4_addr                        */
/************************************************************************/
ST_CHAR *mvl_get_listen_ipv4_addr (ST_CHAR *local_ar_name)
  {
  DIB_ENTRY *dib_entry;
  struct in_addr sin_addr;        /* inet_ntoa needs this addr format*/
  ST_CHAR *ip_text;

  dib_entry = find_dib_entry (local_ar_name);
  if (dib_entry)
    {
    if (dib_entry->pres_addr.useIpv6)
      {
      SXLOG_ERR1 ("ar_name=%s does not contain IPv4 address.", local_ar_name);
      return (NULL);
      }
    sin_addr.s_addr = dib_entry->pres_addr.netAddr.ip;
    ip_text = inet_ntoa (sin_addr);
    return (ip_text); /* return same static ptr that inet_ntoa returns  */
    }
  else
    {
    SXLOG_ERR1 ("ar_name=%s not found.", local_ar_name);
    return (NULL);
    }
  }

/************************************************************************/
/*                      mvl_get_listen_ipv6_addr                        */
/************************************************************************/
ST_CHAR *mvl_get_listen_ipv6_addr (ST_CHAR *local_ar_name)
  {
  DIB_ENTRY *dib_entry;
  static ST_CHAR ipText [MAX_IPV6_ADDR_LEN + 1];
  dib_entry = find_dib_entry (local_ar_name);
  if (dib_entry)
    {
    if (!dib_entry->pres_addr.useIpv6)
      {
      SXLOG_ERR1 ("ar_name=%s does not contain IPv6 address.", local_ar_name);
      return (NULL);
      }
    if (inet_ntop (AF_INET6, dib_entry->pres_addr.netAddr.ipv6Addr, ipText, sizeof(ipText)) == NULL)
      return (NULL);
    else
      return (ipText);
    }
  else
    {
    SXLOG_ERR1 ("ar_name=%s not found.", local_ar_name);
    return (NULL);
    }
  }

