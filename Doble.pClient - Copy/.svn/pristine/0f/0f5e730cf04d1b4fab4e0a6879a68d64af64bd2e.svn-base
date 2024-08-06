/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*      (c) Copyright Systems Integration Specialists Company, Inc.,    */
/*      2011 - 2018, All Rights Reserved                                */
/*                                                                      */
/* MODULE NAME : scl_dib.c                                              */
/* PRODUCT(S)  : MMSEASE-LITE                                           */
/*                                                                      */
/* MODULE DESCRIPTION :                                                 */
/*      Functions to fill DIB_ENTRY list from SCL informaion.           */
/*                                                                      */
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :                            */
/*                      scl_dib_entry_save                              */
/*                                                                      */
/* MODIFICATION LOG :                                                   */
/*  Date     Who  Comments                                              */
/* --------  ---  ----------------------------------------------------  */
/* 05/10/18  JRB  Don't ignore addr if IP="0.0.0.0" (caused confusing errors).*/
/* 03/15/18  JRB  Set ipv6ScopeId: needed for IPv6 link-local addr on Linux.*/
/* 01/09/17  JRB  scl_dib_entry_save: if IPv6 addr present, create extra*/
/*                DIB entry with suffix on name. Return -1 on any error.*/
/* 09/15/11  JRB  New                                                   */
/************************************************************************/
#include "glbtypes.h"
#include "sysincs.h"
#include "scl.h"
#include "lean_a.h"
#include "str_util.h" /* for strncpy_safe, etc. */
#include "sx_log.h"

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of      */
/* __FILE__ strings.                                                    */
/************************************************************************/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

#ifdef USE_DIB_LIST     /* this function only works in this mode        */
/************************************************************************/
/*                      scl_dib_entry_save                              */
/* Find all addresses in the SCL file. Fill in a DIB_ENTRY              */
/* structure for each address and then use "dib_entry_create" to save   */
/* the address in a global linked list.                                 */
/* Generate unique ARName by concatenating iedName, "/", and apName.    */
/************************************************************************/
ST_INT scl_dib_entry_save (SCL_INFO *scl_info)
  {
  SCL_SUBNET *scl_subnet;
  SCL_CAP *scl_cap;
  ST_INT addr_count = 0;
  ST_RET retcode;
  ST_UCHAR ipv6_all_zero [16] = {0};

  /* Communication section may contain multiple Subnetwork sections.    */
  /* Use only the first Subnetwork found with type="8-MMS".             */
  for (scl_subnet = (SCL_SUBNET *) list_find_last ((DBL_LNK *) scl_info->subnetHead);
       scl_subnet != NULL;
       scl_subnet = (SCL_SUBNET *) list_find_prev ((DBL_LNK *) scl_info->subnetHead, (DBL_LNK *) scl_subnet))
    {
    for (scl_cap = (SCL_CAP *) list_find_last ((DBL_LNK *) scl_subnet->capHead);
         scl_cap != NULL;
         scl_cap = (SCL_CAP *) list_find_prev ((DBL_LNK *) scl_subnet->capHead, (DBL_LNK *) scl_cap))
      {
      DIB_ENTRY dib_entry;    /* fill in this temporary struct        */
      memset (&dib_entry, 0, sizeof (DIB_ENTRY));     /* start clean  */

      /* Generate unique ARName by concatenating iedName, "/", and apName.*/
      strncpy_safe      (dib_entry.name, scl_cap->iedName, sizeof(dib_entry.name) - 1);
      strncat_maxstrlen (dib_entry.name, "/", sizeof(dib_entry.name) - 1);
      retcode = strncat_maxstrlen (dib_entry.name, scl_cap->apName, sizeof(dib_entry.name) - 1);
      if (retcode)
        {   /* AR Name was truncated  */
        SXLOG_ERR0 ("Cannot generate AR Name (too long)");
        return (-1);  /* error  */
        }

      /* Copy info from scl_cap->address to DIB_ENTRY.        */
      memcpy (&dib_entry.ae_title, &scl_cap->address.ae_title, sizeof (AE_TITLE));

      dib_entry.pres_addr.psel_len = scl_cap->address.psel_len;
      memcpy (dib_entry.pres_addr.psel, scl_cap->address.psel, scl_cap->address.psel_len);

      dib_entry.pres_addr.ssel_len = scl_cap->address.ssel_len;
      memcpy (dib_entry.pres_addr.ssel, scl_cap->address.ssel, scl_cap->address.ssel_len);

      dib_entry.pres_addr.tsel_len = scl_cap->address.tsel_len;
      memcpy (dib_entry.pres_addr.tsel, scl_cap->address.tsel, scl_cap->address.tsel_len);

      dib_entry.pres_addr.netAddr.ip = scl_cap->address.ip;
      /* This allocates a struct, copies to it, and adds it to global linked list.*/
      if (dib_entry_create (&dib_entry) != SD_SUCCESS)
        {
        SXLOG_ERR0 ("Cannot create DIB entry");
        return (-1);  /* error  */
        }
      else
        {
        addr_count++;
        }

      /* If ipv6Addr is present, create extra DIB entry with suffix on name.*/
      if (memcmp (scl_cap->address.ipv6Addr, ipv6_all_zero, 16) != 0)
        {
        /* add suffix to name */
        retcode = strncat_maxstrlen (dib_entry.name, IPV6_AR_NAME_SUFFIX, sizeof (dib_entry.name) - 1);
        if (retcode)
          {   /* AR Name was truncated  */
          SXLOG_ERR0 ("Cannot generate AR Name for IPv6 (too long)");
          return (-1);  /* error  */
          }
        dib_entry.pres_addr.useIpv6 = SD_TRUE;  /* CRITICAL: to indicate IPv6 is used here.  */
        memcpy (dib_entry.pres_addr.netAddr.ipv6Addr, scl_cap->address.ipv6Addr, 16);
        /* CRITICAL: Copy ipv6ScopeId from tp0_cfg. We use same ipv6ScopeId for ALL remotes.*/
        /* This is required on Linux for link-local destination.  */
        dib_entry.pres_addr.ipv6ScopeId = tp0_cfg.ipv6ScopeId;
        if (dib_entry_create (&dib_entry) != SD_SUCCESS)
          {
          SXLOG_ERR0 ("Cannot create DIB entry");
          return (-1);  /* error  */
          }
        else
          {
          addr_count++;
          }
        }
      }
    }           /* end scl_subnet loop  */

  return (addr_count);
  }
#endif  /* USE_DIB_LIST */

