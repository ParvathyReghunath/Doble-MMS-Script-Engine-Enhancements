/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*      (c) Copyright Systems Integration Specialists Company, Inc.,    */
/*      2018-2018 All Rights Reserved                                   */
/*                                                                      */
/* MODULE NAME : mvl61850_ltrk.c                                        */
/* PRODUCT(S)  : MMS Lite                                               */
/*                                                                      */
/* MODULE DESCRIPTION :                                                 */
/*      Code to support IEC 61850 LTRK as a Server.                     */
/*                                                                      */
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :                            */
/*                                                                      */
/* MODIFICATION LOG :                                                   */
/*  Date     Who  Comments                                              */
/* --------  ---  ----------------------------------------------------  */
/* 01/17/19  JRB  Fix memcpy to "LocbTrk->trgOps".                      */
/* 07/23/18  JRB  Initial revision to support LTRK.                     */
/************************************************************************/
#include "glbtypes.h"
#include "sysincs.h"
#include "mvl_uca.h"
#include "mvl_log.h"
#include "acse2.h"      /* need ACSE_CONN       */
#include "stdtime.h"
#include "stdtime_mms_utctime.h"
#include "str_util.h"

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of      */
/* __FILE__ strings.                                                    */
/************************************************************************/
#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/************************************************************************/
/*                      mvl61850_enum_info_find                         */
/* Find the ENUM_INFO for this attribute.                               */
/* RETURNS: pointer to ENUM_INFO if attribute is an Enum.               */
/*          NULL if attribute is NOT an Enum.                           */
/************************************************************************/
ENUM_INFO *mvl61850_enum_info_find (RUNTIME_CTRL *rt_ctrl, ST_INT comp_rt_index)
  {
  ST_INT enum_info_idx;
  ST_UINT16 *enum_info_idx_arr;
  ENUM_INFO *enum_info = NULL;  /* chg only if valid Enum found. */

  /* CRITICAL: SCL processing code must set "rt_ctrl->reserved_2" pointing*/
  /*           to array of indexes (NULL if no enum in this LN).        */
  enum_info_idx_arr = (ST_UINT16 *) rt_ctrl->reserved_2;

  if (comp_rt_index < 0 || comp_rt_index >= rt_ctrl->rt_num)
    {
    /* This error should never happen. Debug this code if it does.  */
    MVL_LOG_ERR0 ("Type definition incorrect for Enum check.");
    }
  else if (enum_info_idx_arr == NULL)
    {
    /* This error should never happen. Debug this code if it does.  */
    MVL_LOG_ERR0 ("Enum info not found for this LN.");
    }
  else
    {
    enum_info_idx = enum_info_idx_arr[comp_rt_index];
    /* NOTE: index of 0 means this attribute is NOT an Enum. */
    if (enum_info_idx >= mvl_vmd.enum_info_count)
      {
      /* This error should never happen. Debug this code if it does.  */
      MVL_LOG_ERR0 ("CRITICAL: Enum info corrupted.");
      }
    else if (enum_info_idx > 0)
      {
      enum_info = &mvl_vmd.enum_info_arr [enum_info_idx];
      }
    }
  return (enum_info);
  }

/************************************************************************/
/*                      setOriginatorID                                 */
/* Set originatorID for Tracking.                                       */
/* IEC 61850-7-2 just says originatorID (OVstring64) "contains the ID of the client".*/
/* This assumes that means the IPv4 or IPv6 address of the client.      */
/* NOTE: originatorID is optional so may be NULL.                       */
/************************************************************************/
ST_VOID setOriginatorID (MVL_NET_INFO *net_info, MMS_OVSTRING *originatorID)
  {
  ACSE_CONN *acse_conn;

  /* originatorID is optional, so ptr may be NULL. If so, do nothing. */
  if (originatorID == NULL)
    return;

  if (net_info == NULL)
    {
    originatorID->len = 0; /* don't know client so clear the ID. */
    return;
    }

  acse_conn = (ACSE_CONN *) net_info->acse_conn_id;
  if (acse_conn == NULL)
    {
    originatorID->len = 0; /* don't know client so clear the ID. */
    }
  else if (acse_conn->useIpv6)
    {
    MVLU_LOG_FLOW0 ("Setting originatorID for IPv6 connection.");
    memcpy (originatorID->data, acse_conn->remNetAddr.ipv6Addr, 16);
    originatorID->len = 16;
    }
  else
    {
    MVLU_LOG_FLOW0 ("Setting originatorID for IPv4 connection.");
    memcpy (originatorID->data, &acse_conn->remNetAddr.ip, 4);
    originatorID->len = 4;
    }
  }

/* These strings used many times in MVL_LOG_ERR1 macros.  */
ST_CHAR *mandatory_attr_not_present = "Mandatory attribute '%s' not present.";
ST_CHAR *attr_wrong_type = "Attribute '%s' is the wrong type.";
/************************************************************************/
/* RETURNS:                                                             */
/*    SD_SUCCESS: if tracking structure fully mapped (or not present).  */
/*    SD_FAILURE: if any problem detected (may log multiple errors).    */
/************************************************************************/
ST_RET ltrkInitControl (
  MVL_VAR_ASSOC *ltrk_var,             /* LTRK Logical Node  */
  ST_CHAR *CtlTrkName,
  ST_UINT el_tag,     /* expected RUNTIME_TYPE el_tag for ctlVal */
  ST_INT  el_len,     /* expected RUNTIME_TYPE el_len for ctlVal */
  LTRK_CTL_TRK **CtlTrkOut)
  {
  ST_CHAR fcd [MAX_IDENT_LEN + 1]; /* with tracking structure name added*/
  ST_CHAR prefix [MAX_IDENT_LEN + 1]; /* fcd with "$" added */
  ST_CHAR leafName [MAX_IDENT_LEN + 1];
  ST_RET retCode = SD_SUCCESS;  /* change only if partially mapped  */
  ST_VOID *dataPtr;
  RUNTIME_TYPE *rt_type;
  LTRK_CTL_TRK *CtlTrk = NULL; /* allocated only if fcd is found */

  strcpy (fcd, "SR$");
  strncat_maxstrlen (fcd, CtlTrkName, MAX_IDENT_LEN);

  dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, fcd, &rt_type);
  if (dataPtr)
    {
    /* found this tracking structure  */
    CtlTrk = chk_calloc (1, sizeof (LTRK_CTL_TRK));

    /* Create prefix including "$" to use with each attribute name. */
    strcpy (prefix, fcd);
    strncat_maxstrlen (prefix, "$", MAX_IDENT_LEN);

    /* Common (CST) attributes. */
    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "objRef", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      CtlTrk->objRef = dataPtr;
      if (rt_type->el_tag != RT_VISIBLE_STRING || rt_type->u.p.el_len != -MVL61850_MAX_OBJREF_LEN)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "serviceType", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      CtlTrk->serviceType = dataPtr;
      if (rt_type->el_tag != RT_INTEGER || rt_type->u.p.el_len != 1)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "errorCode", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      CtlTrk->errorCode = dataPtr;
      if (rt_type->el_tag != RT_UNSIGNED || rt_type->u.p.el_len != 1)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "t", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      CtlTrk->t = dataPtr;
      if (rt_type->el_tag != RT_UTC_TIME)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    /* Attributes specifically for any Control Tracking.  */
    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "ctlVal", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      CtlTrk->ctlVal = dataPtr;
      /* CRITICAL: ctlVal must be the right type for this Tracking object.*/
      if (rt_type->el_tag != el_tag || rt_type->u.p.el_len != el_len)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "origin$orCat", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      CtlTrk->origin_orCat = dataPtr;
      if (rt_type->el_tag != RT_INTEGER || rt_type->u.p.el_len != 1)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "origin$orIdent", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      CtlTrk->origin_orIdent = dataPtr;
      if (rt_type->el_tag != RT_OCTET_STRING || rt_type->u.p.el_len != -64)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "ctlNum", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      CtlTrk->ctlNum = dataPtr;
      if (rt_type->el_tag != RT_UNSIGNED || rt_type->u.p.el_len != 1)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "T", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      CtlTrk->T = dataPtr;
      if (rt_type->el_tag != RT_UTC_TIME)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "Test", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      CtlTrk->Test = dataPtr;
      if (rt_type->el_tag != RT_BOOL)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "Check", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      CtlTrk->Check = dataPtr;
      if (rt_type->el_tag != RT_BIT_STRING || rt_type->u.p.el_len != -2)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "respAddCause", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      CtlTrk->respAddCause = dataPtr;
      if (rt_type->el_tag != RT_UNSIGNED || rt_type->u.p.el_len != 1)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    /* Optional "originatorID". Do NOT change "retCode" if not found. */
    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "originatorID", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      CtlTrk->originatorID = dataPtr;
      if (rt_type->el_tag != RT_OCTET_STRING || rt_type->u.p.el_len != -64)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }

    /* Optional "operTm". Do NOT change "retCode" if not found. */
    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "operTm", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      CtlTrk->operTm = dataPtr;
      if (rt_type->el_tag != RT_UTC_TIME)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }

    /* On any error, free the structure and clear the pointer.  */
    if (retCode)
      {
      chk_free (CtlTrk);
      CtlTrk = NULL;
      }
    }

  *CtlTrkOut = CtlTrk; /* set caller's ptr  */
  return (retCode);
  }

/************************************************************************/
/* RETURNS:                                                             */
/*    SD_SUCCESS: if tracking structure fully mapped (or not present).  */
/*    SD_FAILURE: if any problem detected (may log multiple errors).    */
/************************************************************************/
ST_RET ltrkInitUrcbTrk (MVL_VAR_ASSOC *ltrk_var,  /* LTRK Logical Node  */
  LTRK_URCB_TRK **UrcbTrkOut)  /* if successful, save allocated ptr here */
  {
  ST_VOID *dataPtr;
  RUNTIME_TYPE *rt_type;
  ST_RET retCode = SD_SUCCESS;  /* change if error detected.  */
  ST_CHAR leafName [MAX_IDENT_LEN + 1];
  ST_CHAR *fcd    = "SR$UrcbTrk";
  ST_CHAR *prefix = "SR$UrcbTrk$";
  LTRK_URCB_TRK *UrcbTrk = NULL; /* allocated only if fcd is found */

  dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, fcd, &rt_type);
  if (dataPtr)
    {
    UrcbTrk = chk_calloc (1, sizeof (*UrcbTrk));

    /* Common (CST) attributes. */
    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "objRef", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      UrcbTrk->objRef = dataPtr;
      if (rt_type->el_tag != RT_VISIBLE_STRING || rt_type->u.p.el_len != -MVL61850_MAX_OBJREF_LEN)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "serviceType", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      UrcbTrk->serviceType = dataPtr;
      if (rt_type->el_tag != RT_INTEGER || rt_type->u.p.el_len != 1)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "errorCode", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      UrcbTrk->errorCode = dataPtr;
      if (rt_type->el_tag != RT_UNSIGNED || rt_type->u.p.el_len != 1)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "t", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      UrcbTrk->t = dataPtr;
      if (rt_type->el_tag != RT_UTC_TIME)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    /* Attributes specifically for UrcbTrk. */
    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "rptID", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      UrcbTrk->rptID = dataPtr;
      if (rt_type->el_tag != RT_VISIBLE_STRING || rt_type->u.p.el_len != -MVL61850_MAX_OBJREF_LEN)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "rptEna", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      UrcbTrk->rptEna = dataPtr;
      if (rt_type->el_tag != RT_BOOL)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "resv", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      UrcbTrk->resv = dataPtr;
      if (rt_type->el_tag != RT_BOOL)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "datSet", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      UrcbTrk->datSet = dataPtr;
      if (rt_type->el_tag != RT_VISIBLE_STRING || rt_type->u.p.el_len != -MVL61850_MAX_OBJREF_LEN)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "confRev", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      UrcbTrk->confRev = dataPtr;
      if (rt_type->el_tag != RT_UNSIGNED || rt_type->u.p.el_len != 4)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "optFlds", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      UrcbTrk->optFlds = dataPtr;
      if (rt_type->el_tag != RT_BIT_STRING || rt_type->u.p.el_len != -10)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "bufTm", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      UrcbTrk->bufTm = dataPtr;
      if (rt_type->el_tag != RT_UNSIGNED || rt_type->u.p.el_len != 4)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "sqNum", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      UrcbTrk->sqNum = dataPtr;
      if (rt_type->el_tag != RT_UNSIGNED || rt_type->u.p.el_len != 2)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "trgOps", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      UrcbTrk->trgOps = dataPtr;
      if (rt_type->el_tag != RT_BIT_STRING || rt_type->u.p.el_len != -6)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "intgPd", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      UrcbTrk->intgPd = dataPtr;
      if (rt_type->el_tag != RT_UNSIGNED || rt_type->u.p.el_len != 4)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "gi", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      UrcbTrk->gi = dataPtr;
      if (rt_type->el_tag != RT_BOOL)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    /* Optional "originatorID". Do NOT change "retCode" if not found. */
    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "originatorID", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      UrcbTrk->originatorID = dataPtr;
      if (rt_type->el_tag != RT_OCTET_STRING || rt_type->u.p.el_len != -64)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }

    if (retCode)
      {
      chk_free (UrcbTrk);
      }
    else
      *UrcbTrkOut = UrcbTrk; /* set caller's ptr  */
    }

  return (retCode);
  }

/************************************************************************/
/* RETURNS:                                                             */
/*    SD_SUCCESS: if tracking structure fully mapped (or not present).  */
/*    SD_FAILURE: if any problem detected (may log multiple errors).    */
/************************************************************************/
ST_RET ltrkInitBrcbTrk (MVL_VAR_ASSOC *ltrk_var,  /* LTRK Logical Node  */
  LTRK_BRCB_TRK **BrcbTrkOut)  /* if successful, save allocated ptr here */
  {
  ST_VOID *dataPtr;
  RUNTIME_TYPE *rt_type;
  ST_RET retCode = SD_SUCCESS;  /* change if error detected.  */
  ST_CHAR leafName [MAX_IDENT_LEN + 1];
  ST_CHAR *fcd    = "SR$BrcbTrk";
  ST_CHAR *prefix = "SR$BrcbTrk$";
  LTRK_BRCB_TRK *BrcbTrk = NULL; /* allocated only if fcd is found */

  dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, fcd, &rt_type);
  if (dataPtr)
    {
    BrcbTrk = chk_calloc (1, sizeof (*BrcbTrk));

    /* Common (CST) attributes. */
    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "objRef", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      BrcbTrk->objRef = dataPtr;
      if (rt_type->el_tag != RT_VISIBLE_STRING || rt_type->u.p.el_len != -MVL61850_MAX_OBJREF_LEN)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "serviceType", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      BrcbTrk->serviceType = dataPtr;
      if (rt_type->el_tag != RT_INTEGER || rt_type->u.p.el_len != 1)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "errorCode", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      BrcbTrk->errorCode = dataPtr;
      if (rt_type->el_tag != RT_UNSIGNED || rt_type->u.p.el_len != 1)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "t", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      BrcbTrk->t = dataPtr;
      if (rt_type->el_tag != RT_UTC_TIME)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    /* Attributes specifically for BrcbTrk. */
    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "rptID", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      BrcbTrk->rptID = dataPtr;
      if (rt_type->el_tag != RT_VISIBLE_STRING || rt_type->u.p.el_len != -MVL61850_MAX_OBJREF_LEN)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "rptEna", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      BrcbTrk->rptEna = dataPtr;
      if (rt_type->el_tag != RT_BOOL)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "datSet", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      BrcbTrk->datSet = dataPtr;
      if (rt_type->el_tag != RT_VISIBLE_STRING || rt_type->u.p.el_len != -MVL61850_MAX_OBJREF_LEN)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "confRev", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      BrcbTrk->confRev = dataPtr;
      if (rt_type->el_tag != RT_UNSIGNED || rt_type->u.p.el_len != 4)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "optFlds", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      BrcbTrk->optFlds = dataPtr;
      if (rt_type->el_tag != RT_BIT_STRING || rt_type->u.p.el_len != -10)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "bufTm", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      BrcbTrk->bufTm = dataPtr;
      if (rt_type->el_tag != RT_UNSIGNED || rt_type->u.p.el_len != 4)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "sqNum", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      BrcbTrk->sqNum = dataPtr;
      if (rt_type->el_tag != RT_UNSIGNED || rt_type->u.p.el_len != 2)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "trgOps", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      BrcbTrk->trgOps = dataPtr;
      if (rt_type->el_tag != RT_BIT_STRING || rt_type->u.p.el_len != -6)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "intgPd", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      BrcbTrk->intgPd = dataPtr;
      if (rt_type->el_tag != RT_UNSIGNED || rt_type->u.p.el_len != 4)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "gi", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      BrcbTrk->gi = dataPtr;
      if (rt_type->el_tag != RT_BOOL)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "purgeBuf", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      BrcbTrk->purgeBuf = dataPtr;
      if (rt_type->el_tag != RT_BOOL)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "entryID", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      BrcbTrk->entryID = dataPtr;
      if (rt_type->el_tag != RT_OCTET_STRING || rt_type->u.p.el_len != 8)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "timeOfEntry", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      BrcbTrk->timeOfEntry = dataPtr;
      if (rt_type->el_tag != RT_BINARY_TIME || rt_type->u.p.el_len != 6)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    /* Optional "originatorID". Do NOT change "retCode" if not found. */
    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "originatorID", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      BrcbTrk->originatorID = dataPtr;
      if (rt_type->el_tag != RT_OCTET_STRING || rt_type->u.p.el_len != -64)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }

    /* Optional "resvTms". Do NOT change "retCode" if not found. */
    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "resvTms", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      BrcbTrk->resvTms = dataPtr;
      if (rt_type->el_tag != RT_INTEGER || rt_type->u.p.el_len != 2)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }

    if (retCode)
      {
      chk_free (BrcbTrk);
      }
    else
      *BrcbTrkOut = BrcbTrk;  /* set caller's ptr  */
    }
  return (retCode);
  }

/************************************************************************/
/* RETURNS:                                                             */
/*    SD_SUCCESS: if tracking structure fully mapped (or not present).  */
/*    SD_FAILURE: if any problem detected (may log multiple errors).    */
/************************************************************************/
ST_RET ltrkInitLocbTrk (MVL_VAR_ASSOC *ltrk_var,  /* LTRK Logical Node  */
  LTRK_LOCB_TRK **LocbTrkOut)  /* if successful, save allocated ptr here */
  {
  ST_VOID *dataPtr;
  RUNTIME_TYPE *rt_type;
  ST_RET retCode = SD_SUCCESS;  /* change if error detected.  */
  ST_CHAR leafName [MAX_IDENT_LEN + 1];
  ST_CHAR *fcd    = "SR$LocbTrk";
  ST_CHAR *prefix = "SR$LocbTrk$";
  LTRK_LOCB_TRK *LocbTrk = NULL; /* allocated only if fcd is found */

  dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, fcd, &rt_type);
  if (dataPtr)
    {
    LocbTrk = chk_calloc (1, sizeof (*LocbTrk));

    /* Common (CST) attributes. */
    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "objRef", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      LocbTrk->objRef = dataPtr;
      if (rt_type->el_tag != RT_VISIBLE_STRING || rt_type->u.p.el_len != -MVL61850_MAX_OBJREF_LEN)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "serviceType", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      LocbTrk->serviceType = dataPtr;
      if (rt_type->el_tag != RT_INTEGER || rt_type->u.p.el_len != 1)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "errorCode", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      LocbTrk->errorCode = dataPtr;
      if (rt_type->el_tag != RT_UNSIGNED || rt_type->u.p.el_len != 1)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "t", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      LocbTrk->t = dataPtr;
      if (rt_type->el_tag != RT_UTC_TIME)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    /* Attributes specifically for LocbTrk. */
    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "logEna", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      LocbTrk->logEna = dataPtr;
      if (rt_type->el_tag != RT_BOOL)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "datSet", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      LocbTrk->datSet = dataPtr;
      if (rt_type->el_tag != RT_VISIBLE_STRING || rt_type->u.p.el_len != -MVL61850_MAX_OBJREF_LEN)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    /* Tissue 1622 says to remove optFlds, so it is NOT included here.*/
    /* bufTm listed in 61850-7-2 but NOT mapped in 61850-8-1, so it is NOT included here.*/
    /* 61850-8-1 and Tissue 1630 say to add oldEntrTm, newEntrTm, oldEnt, newEnt, not included in 61850-7-2.*/
    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "oldEntrTm", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      LocbTrk->oldEntrTm = dataPtr;
      if (rt_type->el_tag != RT_BINARY_TIME || rt_type->u.p.el_len != 6)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "newEntrTm", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      LocbTrk->newEntrTm = dataPtr;
      if (rt_type->el_tag != RT_BINARY_TIME || rt_type->u.p.el_len != 6)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "oldEnt", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      LocbTrk->oldEnt = dataPtr;
      if (rt_type->el_tag != RT_OCTET_STRING || rt_type->u.p.el_len != 8)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "newEnt", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      LocbTrk->newEnt = dataPtr;
      if (rt_type->el_tag != RT_OCTET_STRING || rt_type->u.p.el_len != 8)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "trgOps", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      LocbTrk->trgOps = dataPtr;
      if (rt_type->el_tag != RT_BIT_STRING || rt_type->u.p.el_len != -6)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "intgPd", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      LocbTrk->intgPd = dataPtr;
      if (rt_type->el_tag != RT_UNSIGNED || rt_type->u.p.el_len != 4)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "logRef", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      if (rt_type->el_tag != RT_VISIBLE_STRING || rt_type->u.p.el_len != -MVL61850_MAX_OBJREF_LEN)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      LocbTrk->logRef = dataPtr;
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    /* Optional "originatorID". Do NOT change "retCode" if not found. */
    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "originatorID", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      LocbTrk->originatorID = dataPtr;
      if (rt_type->el_tag != RT_OCTET_STRING || rt_type->u.p.el_len != -64)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }

    if (retCode)
      {
      chk_free (LocbTrk);
      }
    else
      *LocbTrkOut = LocbTrk;  /* set caller's ptr  */
    }

  return (retCode);
  }

/************************************************************************/
/* RETURNS:                                                             */
/*    SD_SUCCESS: if tracking structure fully mapped (or not present).  */
/*    SD_FAILURE: if any problem detected (may log multiple errors).    */
/************************************************************************/
ST_RET ltrkInitGocbTrk (MVL_VAR_ASSOC *ltrk_var,  /* LTRK Logical Node  */
  LTRK_GOCB_TRK **GocbTrkOut)  /* if successful, save allocated ptr here */
  {
  ST_VOID *dataPtr;
  RUNTIME_TYPE *rt_type;
  ST_RET retCode = SD_SUCCESS;  /* change if error detected.  */
  ST_CHAR leafName [MAX_IDENT_LEN + 1];
  ST_CHAR *fcd    = "SR$GocbTrk";
  ST_CHAR *prefix = "SR$GocbTrk$";
  LTRK_GOCB_TRK *GocbTrk = NULL; /* allocated only if fcd is found */

  dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, fcd, &rt_type);
  if (dataPtr)
    {
    GocbTrk = chk_calloc (1, sizeof (*GocbTrk));

    /* Common (CST) attributes. */
    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "objRef", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      GocbTrk->objRef = dataPtr;
      if (rt_type->el_tag != RT_VISIBLE_STRING || rt_type->u.p.el_len != -MVL61850_MAX_OBJREF_LEN)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "serviceType", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      GocbTrk->serviceType = dataPtr;
      if (rt_type->el_tag != RT_INTEGER || rt_type->u.p.el_len != 1)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "errorCode", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      GocbTrk->errorCode = dataPtr;
      if (rt_type->el_tag != RT_UNSIGNED || rt_type->u.p.el_len != 1)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "t", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      GocbTrk->t = dataPtr;
      if (rt_type->el_tag != RT_UTC_TIME)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    /* Attributes specifically for GocbTrk. */
    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "goEna", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      GocbTrk->goEna = dataPtr;
      if (rt_type->el_tag != RT_BOOL)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "goID", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      GocbTrk->goID = dataPtr;
      /* Make sure type is ObjectReference. */
      if (rt_type->el_tag != RT_VISIBLE_STRING || rt_type->u.p.el_len != -MVL61850_MAX_OBJREF_LEN)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "datSet", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      GocbTrk->datSet = dataPtr;
      if (rt_type->el_tag != RT_VISIBLE_STRING || rt_type->u.p.el_len != -MVL61850_MAX_OBJREF_LEN)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "confRev", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      GocbTrk->confRev = dataPtr;
      if (rt_type->el_tag != RT_UNSIGNED || rt_type->u.p.el_len != 4)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "ndsCom", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      GocbTrk->ndsCom = dataPtr;
      if (rt_type->el_tag != RT_BOOL)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    /* Get separate ptr to each attribute of dstAddress.  */
    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "dstAddress$Addr", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      GocbTrk->dstAddress_Addr = dataPtr;
      if (rt_type->el_tag != RT_OCTET_STRING || rt_type->u.p.el_len != 6)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "dstAddress$PRIORITY", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      GocbTrk->dstAddress_PRIORITY = dataPtr;
      if (rt_type->el_tag != RT_UNSIGNED || rt_type->u.p.el_len != 1)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "dstAddress$VID", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      GocbTrk->dstAddress_VID = dataPtr;
      if (rt_type->el_tag != RT_UNSIGNED || rt_type->u.p.el_len != 2)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "dstAddress$APPID", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      GocbTrk->dstAddress_APPID = dataPtr;
      if (rt_type->el_tag != RT_UNSIGNED || rt_type->u.p.el_len != 2)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    /* Optional "originatorID". Do NOT change "retCode" if not found. */
    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "originatorID", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      GocbTrk->originatorID = dataPtr;
      if (rt_type->el_tag != RT_OCTET_STRING || rt_type->u.p.el_len != -64)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }

    if (retCode)
      {
      chk_free (GocbTrk);
      }
    else
      *GocbTrkOut = GocbTrk;  /* set caller's ptr  */
    }

  return (retCode);
  }

/************************************************************************/
/*                      ltrkInitMsvcbTrk                                */
/* Allocate structure to save important "MsvcbTrk" information.         */
/* If multicast=SD_FALSE, this saves "UsvcbTrk" information instead.    */
/* RETURNS:                                                             */
/*    SD_SUCCESS: if tracking structure fully mapped (or not present).  */
/*    SD_FAILURE: if any problem detected (may log multiple errors).    */
/************************************************************************/
ST_RET ltrkInitMsvcbTrk (MVL_VAR_ASSOC *ltrk_var, /* LTRK Logical Node  */
  ST_BOOLEAN multicast,
  LTRK_MSVCB_TRK **MsvcbTrkOut)  /* if successful, save allocated ptr here */
  {
  ST_VOID *dataPtr;
  RUNTIME_TYPE *rt_type;
  ST_RET retCode = SD_SUCCESS;  /* change if error detected.  */
  ST_CHAR leafName [MAX_IDENT_LEN + 1];
  ST_CHAR *fcd;
  ST_CHAR *prefix;
  LTRK_MSVCB_TRK *MsvcbTrk = NULL; /* allocated only if fcd is found */

  if (multicast)
    {
    fcd    = "SR$MsvcbTrk";
    prefix = "SR$MsvcbTrk$";
    }
  else
    {
    fcd    = "SR$UsvcbTrk";
    prefix = "SR$UsvcbTrk$";
    }

  dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, fcd, &rt_type);
  if (dataPtr)
    {
    MsvcbTrk = chk_calloc (1, sizeof (*MsvcbTrk));

    /* Common (CST) attributes. */
    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "objRef", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      MsvcbTrk->objRef = dataPtr;
      if (rt_type->el_tag != RT_VISIBLE_STRING || rt_type->u.p.el_len != -MVL61850_MAX_OBJREF_LEN)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "serviceType", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      MsvcbTrk->serviceType = dataPtr;
      if (rt_type->el_tag != RT_INTEGER || rt_type->u.p.el_len != 1)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "errorCode", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      MsvcbTrk->errorCode = dataPtr;
      if (rt_type->el_tag != RT_UNSIGNED || rt_type->u.p.el_len != 1)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "t", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      MsvcbTrk->t = dataPtr;
      if (rt_type->el_tag != RT_UTC_TIME)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    /* Attributes specifically for MsvcbTrk. */
    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "svEna", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      MsvcbTrk->svEna = dataPtr;
      if (rt_type->el_tag != RT_BOOL)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    strcpy (leafName, prefix);
    if (multicast)
      strncat_maxstrlen (leafName, "msvID", MAX_IDENT_LEN);
    else
      strncat_maxstrlen (leafName, "usvID", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      MsvcbTrk->msvID = dataPtr;
      if (rt_type->el_tag != RT_VISIBLE_STRING || rt_type->u.p.el_len != -MVL61850_MAX_OBJREF_LEN)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "datSet", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      MsvcbTrk->datSet = dataPtr;
      if (rt_type->el_tag != RT_VISIBLE_STRING || rt_type->u.p.el_len != -MVL61850_MAX_OBJREF_LEN)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "confRev", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      MsvcbTrk->confRev = dataPtr;
      if (rt_type->el_tag != RT_UNSIGNED || rt_type->u.p.el_len != 4)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "smpMod", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      MsvcbTrk->smpMod = dataPtr;
      if (rt_type->el_tag != RT_INTEGER || rt_type->u.p.el_len != 1)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "smpRate", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      MsvcbTrk->smpRate = dataPtr;
      if (rt_type->el_tag != RT_UNSIGNED || rt_type->u.p.el_len != 2)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "optFlds", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      MsvcbTrk->optFlds = dataPtr;
      if (rt_type->el_tag != RT_BIT_STRING || rt_type->u.p.el_len != -5)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    /* Get separate ptr to each attribute of dstAddress.  */
    /* This should be basically the same as for GocbTrk.  */
    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "dstAddress$Addr", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      MsvcbTrk->dstAddress_Addr = dataPtr;
      if (rt_type->el_tag != RT_OCTET_STRING || rt_type->u.p.el_len != 6)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "dstAddress$PRIORITY", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      MsvcbTrk->dstAddress_PRIORITY = dataPtr;
      if (rt_type->el_tag != RT_UNSIGNED || rt_type->u.p.el_len != 1)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "dstAddress$VID", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      MsvcbTrk->dstAddress_VID = dataPtr;
      if (rt_type->el_tag != RT_UNSIGNED || rt_type->u.p.el_len != 2)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "dstAddress$APPID", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      MsvcbTrk->dstAddress_APPID = dataPtr;
      if (rt_type->el_tag != RT_UNSIGNED || rt_type->u.p.el_len != 2)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    /* Optional "originatorID". Do NOT change "retCode" if not found. */
    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "originatorID", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      MsvcbTrk->originatorID = dataPtr;
      if (rt_type->el_tag != RT_OCTET_STRING || rt_type->u.p.el_len != -64)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE; /* attribute found but wrong type */
        }
      }

    /* Optional "noASDU". Do NOT change "retCode" if not found. */
    /* DEBUG: noASDU is NOT in MTS in 61850-7-2 but is mapped in 61850-8-1. */
    /* Not clear what this means, so we assume noASDU is optional.  */
    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "noASDU", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      MsvcbTrk->noASDU = dataPtr;
      if (rt_type->el_tag != RT_UNSIGNED || rt_type->u.p.el_len != 2)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE; /* attribute found but wrong type */
        }
      }

    if (retCode)
      {
      chk_free (MsvcbTrk);
      }
    else
      *MsvcbTrkOut = MsvcbTrk;  /* set caller's ptr  */
    }

  return (retCode);
  }

/************************************************************************/
/* RETURNS:                                                             */
/*    SD_SUCCESS: if tracking structure fully mapped (or not present).  */
/*    SD_FAILURE: if any problem detected (may log multiple errors).    */
/************************************************************************/
ST_RET ltrkInitSgcbTrk (MVL_VAR_ASSOC *ltrk_var,  /* LTRK Logical Node  */
  LTRK_SGCB_TRK **SgcbTrkOut)  /* if successful, save allocated ptr here */
  {
  ST_VOID *dataPtr;
  RUNTIME_TYPE *rt_type;
  ST_RET retCode = SD_SUCCESS;  /* change if error detected.  */
  ST_CHAR leafName [MAX_IDENT_LEN + 1];
  ST_CHAR *fcd    = "SR$SgcbTrk";
  ST_CHAR *prefix = "SR$SgcbTrk$";
  LTRK_SGCB_TRK *SgcbTrk = NULL; /* allocated only if fcd is found */

  dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, fcd, &rt_type);
  if (dataPtr)
    {
    SgcbTrk = chk_calloc (1, sizeof (*SgcbTrk));

    /* Common (CST) attributes. */
    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "objRef", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      SgcbTrk->objRef = dataPtr;
      if (rt_type->el_tag != RT_VISIBLE_STRING || rt_type->u.p.el_len != -MVL61850_MAX_OBJREF_LEN)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "serviceType", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      SgcbTrk->serviceType = dataPtr;
      if (rt_type->el_tag != RT_INTEGER || rt_type->u.p.el_len != 1)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "errorCode", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      SgcbTrk->errorCode = dataPtr;
      if (rt_type->el_tag != RT_UNSIGNED || rt_type->u.p.el_len != 1)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "t", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      SgcbTrk->t = dataPtr;
      if (rt_type->el_tag != RT_UTC_TIME)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    /* Attributes specifically for SgcbTrk. */
    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "numOfSG", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      SgcbTrk->numOfSG = dataPtr;
      if (rt_type->el_tag != RT_UNSIGNED || rt_type->u.p.el_len != 1)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "actSG", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      SgcbTrk->actSG = dataPtr;
      if (rt_type->el_tag != RT_UNSIGNED || rt_type->u.p.el_len != 1)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "editSG", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      SgcbTrk->editSG = dataPtr;
      if (rt_type->el_tag != RT_UNSIGNED || rt_type->u.p.el_len != 1)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "cnfEdit", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      SgcbTrk->cnfEdit = dataPtr;
      if (rt_type->el_tag != RT_BOOL)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "lActTim", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      SgcbTrk->lActTim = dataPtr;
      if (rt_type->el_tag != RT_UTC_TIME)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    /* Optional "originatorID". Do NOT change "retCode" if not found. */
    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "originatorID", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      SgcbTrk->originatorID = dataPtr;
      if (rt_type->el_tag != RT_OCTET_STRING || rt_type->u.p.el_len != -64)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }

    /* Optional "resvTms". Do NOT change "retCode" if not found. */
    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "resvTms", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      SgcbTrk->resvTms = dataPtr;
      if (rt_type->el_tag != RT_UNSIGNED || rt_type->u.p.el_len != 2)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }

    if (retCode)
      {
      chk_free (SgcbTrk);
      }
    else
      *SgcbTrkOut = SgcbTrk;  /* set caller's ptr  */
    }

  return (retCode);
  }

/************************************************************************/
/* RETURNS:                                                             */
/*    SD_SUCCESS: if tracking structure fully mapped (or not present).  */
/*    SD_FAILURE: if any problem detected (may log multiple errors).    */
/************************************************************************/
ST_RET ltrkInitGenTrk (MVL_VAR_ASSOC *ltrk_var,   /* LTRK Logical Node  */
  LTRK_GEN_TRK **GenTrkOut)  /* if successful, save allocated ptr here */
  {
  ST_VOID *dataPtr;
  RUNTIME_TYPE *rt_type;
  ST_RET retCode = SD_SUCCESS;  /* change if error detected.  */
  ST_CHAR leafName [MAX_IDENT_LEN + 1];
  ST_CHAR *fcd    = "SR$GenTrk";
  ST_CHAR *prefix = "SR$GenTrk$";
  LTRK_GEN_TRK *GenTrk = NULL; /* allocated only if fcd is found */

  dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, fcd, &rt_type);
  if (dataPtr)
    {
    GenTrk = chk_calloc (1, sizeof (*GenTrk));

    /* Common (CST) attributes. */
    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "objRef", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      GenTrk->objRef = dataPtr;
      if (rt_type->el_tag != RT_VISIBLE_STRING || rt_type->u.p.el_len != -MVL61850_MAX_OBJREF_LEN)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "serviceType", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      GenTrk->serviceType = dataPtr;
      if (rt_type->el_tag != RT_INTEGER || rt_type->u.p.el_len != 1)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "errorCode", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      GenTrk->errorCode = dataPtr;
      if (rt_type->el_tag != RT_UNSIGNED || rt_type->u.p.el_len != 1)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "t", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      GenTrk->t = dataPtr;
      if (rt_type->el_tag != RT_UTC_TIME)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }
    else
      {
      MVL_LOG_ERR1 (mandatory_attr_not_present, leafName);
      retCode = SD_FAILURE;
      }

    /* GenTrk contains ONLY the Common (CST) attributes above. */

    /* Optional "originatorID". Do NOT change "retCode" if not found. */
    strcpy (leafName, prefix);
    strncat_maxstrlen (leafName, "originatorID", MAX_IDENT_LEN);
    dataPtr = mvlu_get_leaf_data_ptr (ltrk_var, leafName, &rt_type);
    if (dataPtr)
      {
      GenTrk->originatorID = dataPtr;
      if (rt_type->el_tag != RT_OCTET_STRING || rt_type->u.p.el_len != -64)
        {
        MVL_LOG_ERR1 (attr_wrong_type, leafName);
        retCode = SD_FAILURE;
        }
      }

    if (retCode)
      {
      chk_free (GenTrk);
      }
    else
      *GenTrkOut = GenTrk;  /* set caller's ptr  */
    }

  return (retCode);
  }

/************************************************************************/
/*                      ltrkInitAll                                     */
/* Allocate and initialize all information needed to support the LTRK   */
/* Logical Node in this Logical Device.                                 */
/************************************************************************/
ST_RET ltrkInitAll (MVL_DOM_CTRL *dom)
  {
  ST_INT j;
  MVL_VAR_ASSOC *ltrk_var;
  LTRK_INFO *ltrk_info; /* extra info allocated to support LTRK */
  RUNTIME_CTRL *ltrk_rt_ctrl;   /* need this for EncTrk */
  ST_RET retCode = SD_SUCCESS;  /* change if error detected.  */

  /* Find variable definition for "LTRK" (if present). */
  if ((ltrk_var = dom->ltrk_var) == NULL)
    {
    return (SD_SUCCESS);  /* LTRK not present so no tracking  */
    }

  /* Allocate a structure to store extra information for LTRK.  */
  /* Save the pointer in the domain for easy access.  */
  dom->ltrk_info = chk_calloc (1, sizeof (LTRK_INFO));
  ltrk_info = dom->ltrk_info;

  /* Init UrcbTrk.  */
  retCode = ltrkInitUrcbTrk (ltrk_var, &ltrk_info->UrcbTrk);
  if (retCode)
    return (retCode); /* stop on first error  */

  /* Init BrcbTrk.  */
  retCode = ltrkInitBrcbTrk (ltrk_var, &ltrk_info->BrcbTrk);
  if (retCode)
    return (retCode); /* stop on first error  */

  /* Init LocbTrk.  */
  retCode = ltrkInitLocbTrk (ltrk_var, &ltrk_info->LocbTrk);
  if (retCode)
    return (retCode); /* stop on first error  */

  /* Init GocbTrk.  */
  retCode = ltrkInitGocbTrk (ltrk_var, &ltrk_info->GocbTrk);
  if (retCode)
    return (retCode); /* stop on first error  */

  /* Init MsvbTrk.  */
  retCode = ltrkInitMsvcbTrk (ltrk_var, SD_TRUE, &ltrk_info->MsvcbTrk);
  if (retCode)
    return (retCode); /* stop on first error  */

  /* Init UsvbTrk (uses same function as MsvcbTrk). */
  retCode = ltrkInitMsvcbTrk (ltrk_var, SD_FALSE, &ltrk_info->UsvcbTrk);
  if (retCode)
    return (retCode); /* stop on first error  */

  /* Init SgcbTrk.  */
  retCode = ltrkInitSgcbTrk (ltrk_var, &ltrk_info->SgcbTrk);
  if (retCode)
    return (retCode); /* stop on first error  */

  /* Init GenTrk.  */
  retCode = ltrkInitGenTrk (ltrk_var, &ltrk_info->GenTrk);
  if (retCode)
    return (retCode); /* stop on first error  */

  /* Use the same function to map any control.  */
  /* ltrkInitControl will return NULL if object not found. */
  retCode = ltrkInitControl (ltrk_var, "SpcTrk", RT_BOOL, 1, &ltrk_info->SpcTrk);
  if (retCode)
    return (retCode); /* stop on first error  */

  retCode = ltrkInitControl (ltrk_var, "DpcTrk", RT_BOOL, 1, &ltrk_info->DpcTrk);
  if (retCode)
    return (retCode); /* stop on first error  */

  retCode = ltrkInitControl (ltrk_var, "IncTrk", RT_INTEGER, 4, &ltrk_info->IncTrk);
  if (retCode)
    return (retCode); /* stop on first error  */

  /* CRITICAL: EncTrk must be sorted so EncTrk1 is in ltrk_rt_ctrl->EncTrkInfoArr [0], etc.*/
  /*           Some array entries may be uninitialized. */
  ltrk_rt_ctrl = ltrk_var->type_ctrl->rt_ctrl;  /* need this for EncTrk */
  for (j = 0; j < MAX_ENC_TRK; j++)
    {
    if (ltrk_rt_ctrl->EncTrkCtlValType [j] != NULL)
      {
      /* If this entry was initialized, call ltrkInitControl. */
      ST_CHAR EncTrkName [10];
      sprintf (EncTrkName, "EncTrk%d", j+1);
      retCode = ltrkInitControl (ltrk_var, EncTrkName, RT_INTEGER, 1, &ltrk_info->EncTrkArr [j]);
      if (retCode)
        return (retCode); /* stop on first error  */
      }
    }
  retCode = ltrkInitControl (ltrk_var, "BscTrk", RT_BIT_STRING, 2, &ltrk_info->BscTrk); /* Dbpos*/
  if (retCode)
    return (retCode); /* stop on first error  */

  retCode = ltrkInitControl (ltrk_var, "BacTrk", RT_BIT_STRING, 2, &ltrk_info->BacTrk); /* Dbpos*/
  if (retCode)
    return (retCode); /* stop on first error  */

  retCode = ltrkInitControl (ltrk_var, "IscTrk", RT_INTEGER, 1, &ltrk_info->IscTrk);  /* INT8*/
  if (retCode)
    return (retCode); /* stop on first error  */

  retCode = ltrkInitControl (ltrk_var, "ApcIntTrk", RT_INTEGER, 4, &ltrk_info->ApcIntTrk);
  if (retCode)
    return (retCode); /* stop on first error  */

  retCode = ltrkInitControl (ltrk_var, "ApcFTrk", RT_FLOATING_POINT, 4, &ltrk_info->ApcFTrk);

  return (retCode);
  }

/************************************************************************/
/* UrcbTrk contains pointers directly to MMS data. This function copies */
/* data to those pointers.                                              */
/************************************************************************/
ST_VOID ltrkUpdateUrcbTrk (MVL_NET_INFO *netInfo, MVL_DOM_CTRL *domCtrl, MVLU_RPT_CTRL *rptCtrl, ST_INT8 errorCode, ST_INT8 serviceType)
  {
  STDTIME stdTime;
  LTRK_URCB_TRK *UrcbTrk;

  /* Find UrcbTrk, if present in "LTRK". */
  if (domCtrl->ltrk_info == NULL ||
      domCtrl->ltrk_info->UrcbTrk == NULL)
    {
    return; /* No LTRK or no UrcbTrk. Cannot track. */
    }

  UrcbTrk = domCtrl->ltrk_info->UrcbTrk;

  /* CRITICAL: Set flag to indicate it was updated. Cleared when rpt sent.  */
  domCtrl->ltrk_info->ltrkUpdated.UrcbTrk = SD_TRUE;

  /* Get the current time and convert to UTC.  */
  GetStdTime (&stdTime);
  StdTimeToMmsUtcTime (&stdTime, UrcbTrk->t);

  strcpy (UrcbTrk->objRef, rptCtrl->objRefAcsi);  /* ObjectReference in ACSI syntax*/

  *UrcbTrk->serviceType = serviceType;
  *UrcbTrk->errorCode = errorCode;

  setOriginatorID (netInfo, UrcbTrk->originatorID);

  strcpy (UrcbTrk->rptID, rptCtrl->basrcb.RptID);
  *UrcbTrk->rptEna = rptCtrl->basrcb.RptEna;
  *UrcbTrk->resv = rptCtrl->basrcb.Resv;
  strcpy (UrcbTrk->datSet, rptCtrl->basrcb.DatSetNa);
  *UrcbTrk->confRev = rptCtrl->basrcb.ConfRev;

  memcpy (UrcbTrk->optFlds, &rptCtrl->basrcb.OptFlds, 4); /* BVstring10: 2 byte len + 2 byte data*/

  *UrcbTrk->bufTm = rptCtrl->basrcb.BufTim;
  *UrcbTrk->sqNum = rptCtrl->basrcb.SqNum;

  memcpy (UrcbTrk->trgOps, &rptCtrl->basrcb.TrgOps, 3);  /* BVstring6: 2 byte len + 1 byte data*/

  *UrcbTrk->intgPd = rptCtrl->basrcb.IntgPd;
  *UrcbTrk->gi = 0; /* not saved. Always reset to 0 after writing*/
  }

/************************************************************************/
/* BrcbTrk contains pointers directly to MMS data. This function copies */
/* data to those pointers.                                              */
/************************************************************************/
ST_VOID ltrkUpdateBrcbTrk (MVL_NET_INFO *netInfo, MVL_DOM_CTRL *domCtrl, MVLU_RPT_CTRL *rptCtrl, ST_INT8 errorCode, ST_INT8 serviceType)
  {
  STDTIME stdTime;
  LTRK_BRCB_TRK *BrcbTrk;

  /* Find BrcbTrk, if present in "LTRK". */
  if (domCtrl->ltrk_info == NULL ||
      domCtrl->ltrk_info->BrcbTrk == NULL)
    {
    return; /* No LTRK or no BrcbTrk. Cannot track. */
    }

  BrcbTrk = domCtrl->ltrk_info->BrcbTrk;

  /* CRITICAL: Set flag to indicate it was updated. Cleared when rpt sent.  */
  domCtrl->ltrk_info->ltrkUpdated.BrcbTrk = SD_TRUE;

  /* Get the current time and convert to UTC.  */
  GetStdTime (&stdTime);
  StdTimeToMmsUtcTime (&stdTime, BrcbTrk->t);

  strcpy (BrcbTrk->objRef, rptCtrl->objRefAcsi);  /* ObjectReference in ACSI syntax*/

  *BrcbTrk->serviceType = serviceType;
  *BrcbTrk->errorCode = errorCode;

  setOriginatorID (netInfo, BrcbTrk->originatorID);

  strcpy (BrcbTrk->rptID, rptCtrl->basrcb.RptID);
  *BrcbTrk->rptEna = rptCtrl->basrcb.RptEna;
  strcpy (BrcbTrk->datSet, rptCtrl->basrcb.DatSetNa);
  *BrcbTrk->confRev = rptCtrl->basrcb.ConfRev;

  memcpy (BrcbTrk->optFlds, &rptCtrl->basrcb.OptFlds, 4); /* BVstring10: 2 byte len + 2 byte data*/

  *BrcbTrk->bufTm = rptCtrl->basrcb.BufTim;
  *BrcbTrk->sqNum = rptCtrl->basrcb.SqNum;

  memcpy (BrcbTrk->trgOps, &rptCtrl->basrcb.TrgOps, 3);  /* BVstring6: 2 byte len + 1 byte data*/

  *BrcbTrk->intgPd = rptCtrl->basrcb.IntgPd;
  *BrcbTrk->gi = 0; /* not saved. Always reset to 0 after writing*/

  *BrcbTrk->purgeBuf = 0; /* not saved. Always reset to 0 after writing*/
  memcpy (BrcbTrk->entryID, rptCtrl->basrcb.EntryID, 8);
  *BrcbTrk->timeOfEntry = rptCtrl->basrcb.TimeofEntry;
  /* resvTms is optional in BrcbTrk. Set it only if pointer was initialized.*/
  if (BrcbTrk->resvTms)
    *BrcbTrk->resvTms = rptCtrl->basrcb.ResvTms;
  }

/************************************************************************/
/* LocbTrk contains pointers directly to MMS data. This function copies */
/* data to those pointers.                                              */
/************************************************************************/
ST_VOID ltrkUpdateLocbTrk (MVL_NET_INFO *netInfo, MVL_DOM_CTRL *domCtrl, MVL61850_LOG_CTRL *logCtrl, ST_INT8 errorCode, ST_INT8 serviceType)
  {
  STDTIME stdTime;
  LTRK_LOCB_TRK *LocbTrk;

  /* Find LocbTrk, if present in "LTRK". */
  if (domCtrl->ltrk_info == NULL ||
      domCtrl->ltrk_info->LocbTrk == NULL)
    {
    return; /* No LTRK or no LocbTrk. Cannot track. */
    }

  LocbTrk = domCtrl->ltrk_info->LocbTrk;

  /* CRITICAL: Set flag to indicate it was updated. Cleared when rpt sent.  */
  domCtrl->ltrk_info->ltrkUpdated.LocbTrk = SD_TRUE;

  /* Get the current time and convert to UTC.  */
  GetStdTime (&stdTime);
  StdTimeToMmsUtcTime (&stdTime, LocbTrk->t);

  strcpy (LocbTrk->objRef, logCtrl->objRefAcsi);  /* ObjectReference in ACSI syntax*/

  *LocbTrk->serviceType = serviceType;
  *LocbTrk->errorCode = errorCode;

  setOriginatorID (netInfo, LocbTrk->originatorID);

  *LocbTrk->logEna = *logCtrl->lcb_attr.LogEna;
  strcpy (LocbTrk->datSet, logCtrl->lcb_attr.DatSet);

  /* Tissue 1622 (Part 7-2 of Edition 2) says to remove optFlds from LTS, so it is not in LocbTrk.*/

  /* NOTE: "bufTm" was not mapped in 61850-8-1, so it is NOT included in LocbTrk.*/

  memcpy (LocbTrk->trgOps, logCtrl->lcb_attr.TrgOps, 3);  /* BVstring6: 2 byte len + 1 byte data*/

  *LocbTrk->intgPd = *logCtrl->lcb_attr.IntgPd;

  strcpy (LocbTrk->logRef, logCtrl->lcb_attr.LogRef);
  }

/************************************************************************/
/* GocbTrk contains pointers directly to MMS data. This function copies */
/* data to those pointers.                                              */
/************************************************************************/
ST_VOID ltrkUpdateGocbTrk (MVL_NET_INFO *netInfo,
  MVL_DOM_CTRL *domCtrl,
  GSE_IEC_CTRL *gooseCtrl,
  ST_BOOLEAN GoEna,
  ST_UINT8 *dstMAC,
  ST_UINT16 etype_tci,
  ST_UINT16 etype_appID,
  ST_INT8 errorCode,
  ST_INT8 serviceType)
  {
  STDTIME stdTime;
  LTRK_GOCB_TRK *GocbTrk;

  /* Find GocbTrk, if present in "LTRK". */
  if (domCtrl->ltrk_info == NULL ||
      domCtrl->ltrk_info->GocbTrk == NULL)
    {
    return; /* No LTRK or no GocbTrk. Cannot track. */
    }

  GocbTrk = domCtrl->ltrk_info->GocbTrk;

  /* CRITICAL: Set flag to indicate it was updated. Cleared when rpt sent.  */
  domCtrl->ltrk_info->ltrkUpdated.GocbTrk = SD_TRUE;

  /* Get the current time and convert to UTC.  */
  GetStdTime (&stdTime);
  StdTimeToMmsUtcTime (&stdTime, GocbTrk->t);

  /* objRef must be ACSI syntax. */
  strcpy (GocbTrk->objRef, gooseCtrl->objRefAcsi);

  *GocbTrk->serviceType = serviceType;
  *GocbTrk->errorCode = errorCode;

  setOriginatorID (netInfo, GocbTrk->originatorID);

  *GocbTrk->goEna = GoEna;
  strncpy_safe (GocbTrk->goID, gooseCtrl->appID, MVL61850_MAX_OBJREF_LEN);
  strncpy_safe (GocbTrk->datSet, gooseCtrl->dataSetRef, MVL61850_MAX_OBJREF_LEN);
  *GocbTrk->confRev = gooseCtrl->confRev;
  *GocbTrk->ndsCom = gooseCtrl->needsCommissioning;
  memcpy (GocbTrk->dstAddress_Addr, dstMAC, CLNP_MAX_LEN_MAC);  /* Ostring6 */
  *GocbTrk->dstAddress_PRIORITY = (ST_UINT8) (etype_tci >> 13); /* top 3 bits of 16*/
  *GocbTrk->dstAddress_VID = etype_tci & 0x0FFF;  /* low 12 bits of 16  */
  *GocbTrk->dstAddress_APPID = etype_appID;
  }

/************************************************************************/
/*                      ltrkUpdateMsvcbTrk                                  */
/* Function to update MsvcbTrk (or UsvcbTrk).                           */
/* LTRK_MSVCB_TRK contains pointers directly to MMS data. This function      */
/* copies data to those pointers.                                       */
/************************************************************************/
ST_VOID ltrkUpdateMsvcbTrk (MVL_NET_INFO *netInfo, MVL_DOM_CTRL *domCtrl, SMPVAL_PUBLISHER *smpvalPub, ST_INT8 errorCode, ST_INT8 serviceType)
  {
  STDTIME stdTime;
  LTRK_MSVCB_TRK *MsvcbTrk; /* pointer to MsvcbTrk (or UsvcbTrk) */

  /* Find "LTRK". */
  if (domCtrl->ltrk_info == NULL)
    {
    return; /* No LTRK. Cannot track. */
    }

  /* Find MsvcbTrk or UsvcbTrk. */
  if (smpvalPub->multicast)
    {
    MsvcbTrk = domCtrl->ltrk_info->MsvcbTrk;
    }
  else
    {
    MsvcbTrk = domCtrl->ltrk_info->UsvcbTrk;
    }
  if (MsvcbTrk == NULL)
    return; /* Expected MsvcbTrk or UsvcbTrk not present. Cannot track. */

  /* CRITICAL: Set flag to indicate it was updated. Cleared when rpt sent.  */
  if (smpvalPub->multicast)
    domCtrl->ltrk_info->ltrkUpdated.MsvcbTrk = SD_TRUE;
  else
    domCtrl->ltrk_info->ltrkUpdated.UsvcbTrk = SD_TRUE;

  /* Get the current time and convert to UTC.  */
  GetStdTime (&stdTime);
  StdTimeToMmsUtcTime (&stdTime, MsvcbTrk->t);

  strcpy (MsvcbTrk->objRef, smpvalPub->objRefAcsi);  /* ObjectReference in ACSI syntax*/

  *MsvcbTrk->serviceType = serviceType;
  *MsvcbTrk->errorCode = errorCode;

  setOriginatorID (netInfo, MsvcbTrk->originatorID);

  /* Copy data from smpvalPub to MsvcbTrk.  */
  /* NOTE: All mandatory pointers were set at startup (only MsvcbTrk->noASDU is optional).*/
  *MsvcbTrk->svEna = *smpvalPub->SvEna;
  strcpy (MsvcbTrk->datSet, smpvalPub->DatSet);
  strcpy (MsvcbTrk->msvID, smpvalPub->MsvID);

  *MsvcbTrk->confRev = *smpvalPub->ConfRev;
  *MsvcbTrk->smpRate = *smpvalPub->SmpRate;
  memcpy (MsvcbTrk->optFlds, smpvalPub->OptFlds, 3); /* BVstring5: 2 byte len + 1 byte data*/
  *MsvcbTrk->smpMod = *smpvalPub->SmpMod;

  memcpy (MsvcbTrk->dstAddress_Addr, smpvalPub->DstAddress_Addr, CLNP_MAX_LEN_MAC);
  *MsvcbTrk->dstAddress_PRIORITY = *smpvalPub->DstAddress_PRIORITY;
  *MsvcbTrk->dstAddress_VID = *smpvalPub->DstAddress_VID;
  *MsvcbTrk->dstAddress_APPID = *smpvalPub->DstAddress_APPID;

  /* DEBUG: noASDU is NOT in MTS in Part 7-2 but is mapped in Part 8-1. */
  /* Not clear what this means, so we assume noASDU is optional in MsvcbTrk.*/
  /* Set "noASDU" in MsvcbTrk only if it is present.  */
  if (MsvcbTrk->noASDU != NULL)
    *MsvcbTrk->noASDU = *smpvalPub->noASDU;
  }


/************************************************************************/
/*                      ltrkUpdateCtlTrkCommon                          */
/* Update "common" Control tracking attributes (caller updates ctlVal). */
/************************************************************************/
static ST_VOID ltrkUpdateCtlTrkCommon (MVL_NET_INFO *netInfo,
  MVL_DOM_CTRL *domCtrl,
  ST_CHAR *varName,
  LTRK_CTL_TRK_VALUES *CtlTrkValues,
  ST_CHAR *baseName,  /* base of attrName to find (e.g. "CO$Mod$Oper$").*/
  LTRK_CTL_TRK *CtlTrk)
  {
  STDTIME stdTime;
  ST_CHAR attrName [MAX_IDENT_LEN + 1];
  ST_CHAR ctlVarName [MAX_IDENT_LEN + 1];
  ST_CHAR *tmpPtr;  /* to loop through ctlVarName */
  ST_VOID *dataPtr;
  RUNTIME_TYPE *rtType;
  ST_INT dollarCount;

  /* Get the current time and convert to UTC.  */
  GetStdTime (&stdTime);
  StdTimeToMmsUtcTime (&stdTime, CtlTrk->t);

  /* Generate ObjectReference. For controls, this is the reference of the   */
  /* controlled data object (i.e. Oper, SBOw, etc. attribute not included), */
  /* so we must strip off text after DO name. */
  strcpy (ctlVarName, varName);
  tmpPtr = ctlVarName;
  dollarCount = 0;
  while (*tmpPtr)
    {
    if (*tmpPtr == '$')
      dollarCount++;
    if (dollarCount >= 3)
      {
      /* Replace third '$' (after DO name) with NULL terminator and stop. */
      *tmpPtr = '\0';
      break;
      }
    tmpPtr++;
    }
  mvl61850_objref_acsi_create (ctlVarName, domCtrl->name, CtlTrk->objRef);

  *CtlTrk->serviceType = CtlTrkValues->serviceType;
  *CtlTrk->errorCode = CtlTrkValues->errorCode;

  setOriginatorID (netInfo, CtlTrk->originatorID);

  /* NOTE: operTm is optional so may be NULL. */
  strcpy (attrName, baseName);
  strncat_maxstrlen (attrName, "operTm", MVL61850_MAX_OBJREF_LEN);
  dataPtr = mvlu_get_leaf_data_ptr (CtlTrkValues->base_va, attrName, &rtType);
  if (dataPtr)
    memcpy (CtlTrk->operTm, dataPtr, sizeof(MMS_UTC_TIME));  /* copy to avoid alignment fault*/

  strcpy (attrName, baseName);
  strncat_maxstrlen (attrName, "origin$orCat", MAX_IDENT_LEN);
  dataPtr = mvlu_get_leaf_data_ptr (CtlTrkValues->base_va, attrName, &rtType);
  if (dataPtr)
    *CtlTrk->origin_orCat = *(ST_INT8 *) dataPtr;
  else
    MVL_LOG_ERR1 ("Cannot find attribute '%s' for Control Tracking", attrName);

  strcpy (attrName, baseName);
  strncat_maxstrlen (attrName, "origin$orIdent", MAX_IDENT_LEN);
  dataPtr = mvlu_get_leaf_data_ptr (CtlTrkValues->base_va, attrName, &rtType);
  if (dataPtr)
    memcpy (CtlTrk->origin_orIdent, dataPtr, 66); /* OVstring64: 2 byte len, 64 byte data*/
  else
    MVL_LOG_ERR1 ("Cannot find attribute '%s' for Control Tracking", attrName);

  strcpy (attrName, baseName);
  strncat_maxstrlen (attrName, "ctlNum", MAX_IDENT_LEN);
  dataPtr = mvlu_get_leaf_data_ptr (CtlTrkValues->base_va, attrName, &rtType);
  if (dataPtr)
    *CtlTrk->ctlNum = *(ST_UINT8 *) dataPtr;
  else
    MVL_LOG_ERR1 ("Cannot find attribute '%s' for Control Tracking", attrName);

  strcpy (attrName, baseName);
  strncat_maxstrlen (attrName, "T", MAX_IDENT_LEN);
  dataPtr = mvlu_get_leaf_data_ptr (CtlTrkValues->base_va, attrName, &rtType);
  if (dataPtr)
    memcpy (CtlTrk->T, dataPtr, sizeof(MMS_UTC_TIME));  /* copy to avoid alignment fault*/
  else
    MVL_LOG_ERR1 ("Cannot find attribute '%s' for Control Tracking", attrName);

  strcpy (attrName, baseName);
  strncat_maxstrlen (attrName, "Test", MAX_IDENT_LEN);
  dataPtr = mvlu_get_leaf_data_ptr (CtlTrkValues->base_va, attrName, &rtType);
  if (dataPtr)
    *CtlTrk->Test = *(ST_BOOLEAN *) dataPtr;
  else
    MVL_LOG_ERR1 ("Cannot find attribute '%s' for Control Tracking", attrName);

  strcpy (attrName, baseName);
  strncat_maxstrlen (attrName, "Check", MAX_IDENT_LEN);
  dataPtr = mvlu_get_leaf_data_ptr (CtlTrkValues->base_va, attrName, &rtType);
  if (dataPtr)
    memcpy (CtlTrk->Check, dataPtr, 3); /* BVstring2: 2 byte len + 1 byte data */
  else
    MVL_LOG_ERR1 ("Cannot find attribute '%s' for Control Tracking", attrName);

  *CtlTrk->respAddCause = CtlTrkValues->respAddCause;
  }

/************************************************************************/
/************************************************************************/
ST_VOID ltrkUpdateSpcTrk (MVL_NET_INFO *netInfo,
  MVL_DOM_CTRL *domCtrl,
  ST_CHAR *varName,
  LTRK_CTL_TRK_VALUES *CtlTrkValues,
  ST_CHAR *baseName,
  ST_VOID *dataPtr,
  RUNTIME_TYPE *rtType)
  {
  LTRK_INFO *ltrk_info = domCtrl->ltrk_info;
  if (ltrk_info->SpcTrk)
    {
    /* Data must be ST_BOOLEAN. */
    if (rtType->el_tag == RT_BOOL)
      {
      *(ST_BOOLEAN *) ltrk_info->SpcTrk->ctlVal = *(ST_BOOLEAN *) dataPtr;
      ltrkUpdateCtlTrkCommon (netInfo, domCtrl, varName, CtlTrkValues, baseName, ltrk_info->SpcTrk);
      ltrk_info->ltrkUpdated.SpcTrk = SD_TRUE;
      }
    else
      {
      MVL_LOG_ERR1 ("Cannot track control '%s' because 'ctlVal' is wrong type for 'SpcTrk'.", varName);
      }
    }
  }
/************************************************************************/
/************************************************************************/
ST_VOID ltrkUpdateDpcTrk (MVL_NET_INFO *netInfo,
  MVL_DOM_CTRL *domCtrl,
  ST_CHAR *varName,
  LTRK_CTL_TRK_VALUES *CtlTrkValues,
  ST_CHAR *baseName,
  ST_VOID *dataPtr,
  RUNTIME_TYPE *rtType)
  {
  LTRK_INFO *ltrk_info = domCtrl->ltrk_info;
  if (ltrk_info->DpcTrk)
    {
    /* Data must be ST_BOOLEAN. */
    if (rtType->el_tag == RT_BOOL)
      {
      *(ST_BOOLEAN *) ltrk_info->DpcTrk->ctlVal = *(ST_BOOLEAN *) dataPtr;
      ltrkUpdateCtlTrkCommon (netInfo, domCtrl, varName, CtlTrkValues, baseName, ltrk_info->DpcTrk);
      ltrk_info->ltrkUpdated.DpcTrk = SD_TRUE;
      }
    else
      {
      MVL_LOG_ERR1 ("Cannot track control '%s' because 'ctlVal' is wrong type for 'DpcTrk'.", varName);
      }
    }
  }
/************************************************************************/
/************************************************************************/
ST_VOID ltrkUpdateIncTrk (MVL_NET_INFO *netInfo,
  MVL_DOM_CTRL *domCtrl,
  ST_CHAR *varName,
  LTRK_CTL_TRK_VALUES *CtlTrkValues,
  ST_CHAR *baseName,
  ST_VOID *dataPtr,
  RUNTIME_TYPE *rtType)
  {
  LTRK_INFO *ltrk_info = domCtrl->ltrk_info;
  if (ltrk_info->IncTrk)
    {
    /* Data must be ST_INT32. */
    if (rtType->el_tag == RT_INTEGER && rtType->u.p.el_len == 4)
      {
      *(ST_INT32 *) ltrk_info->IncTrk->ctlVal = *(ST_INT32 *) dataPtr;
      ltrkUpdateCtlTrkCommon (netInfo, domCtrl, varName, CtlTrkValues, baseName, ltrk_info->IncTrk);
      ltrk_info->ltrkUpdated.IncTrk = SD_TRUE;
      }
    else
      {
      MVL_LOG_ERR1 ("Cannot track control '%s' because 'ctlVal' is wrong type for 'IncTrk'.", varName);
      }
    }
  }
/************************************************************************/
/************************************************************************/
ST_VOID ltrkUpdateBscTrk (MVL_NET_INFO *netInfo,
  MVL_DOM_CTRL *domCtrl,
  ST_CHAR *varName,
  LTRK_CTL_TRK_VALUES *CtlTrkValues,
  ST_CHAR *baseName,
  ST_VOID *dataPtr,
  RUNTIME_TYPE *rtType)
  {
  LTRK_INFO *ltrk_info = domCtrl->ltrk_info;
  if (ltrk_info->BscTrk)
    {
    /* Data must be ST_UINT8. */
    if (rtType->el_tag == RT_UNSIGNED && rtType->u.p.el_len == 1)
      {
      *(ST_UINT8 *) ltrk_info->BscTrk->ctlVal = *(ST_UINT8 *) dataPtr;  /* Bstring2 (1 byte) */
      ltrkUpdateCtlTrkCommon (netInfo, domCtrl, varName, CtlTrkValues, baseName, ltrk_info->BscTrk);
      ltrk_info->ltrkUpdated.BscTrk = SD_TRUE;
      }
    else
      {
      MVL_LOG_ERR1 ("Cannot track control '%s' because 'ctlVal' is wrong type for 'BscTrk'.", varName);
      }
    }
  }
/************************************************************************/
/************************************************************************/
ST_VOID ltrkUpdateBacTrk (MVL_NET_INFO *netInfo,
  MVL_DOM_CTRL *domCtrl,
  ST_CHAR *varName,
  LTRK_CTL_TRK_VALUES *CtlTrkValues,
  ST_CHAR *baseName,
  ST_VOID *dataPtr,
  RUNTIME_TYPE *rtType)
  {
  LTRK_INFO *ltrk_info = domCtrl->ltrk_info;
  if (ltrk_info->BacTrk)
    {
    /* Data must be ST_UINT8. */
    if (rtType->el_tag == RT_UNSIGNED && rtType->u.p.el_len == 1)
      {
      *(ST_UINT8 *) ltrk_info->BacTrk->ctlVal = *(ST_UINT8 *) dataPtr;  /* Bstring2 (1 byte) */
      ltrkUpdateCtlTrkCommon (netInfo, domCtrl, varName, CtlTrkValues, baseName, ltrk_info->BacTrk);
      ltrk_info->ltrkUpdated.BacTrk = SD_TRUE;
      }
    else
      {
      MVL_LOG_ERR1 ("Cannot track control '%s' because 'ctlVal' is wrong type for 'BacTrk'.", varName);
      }
    }
  }
/************************************************************************/
/************************************************************************/
ST_VOID ltrkUpdateIscTrk (MVL_NET_INFO *netInfo,
  MVL_DOM_CTRL *domCtrl,
  ST_CHAR *varName,
  LTRK_CTL_TRK_VALUES *CtlTrkValues,
  ST_CHAR *baseName,
  ST_VOID *dataPtr,
  RUNTIME_TYPE *rtType)
  {
  LTRK_INFO *ltrk_info = domCtrl->ltrk_info;
  if (ltrk_info->IscTrk)
    {
    /* Data must be ST_INT8. */
    if (rtType->el_tag == RT_INTEGER && rtType->u.p.el_len == 1)
      {
      *(ST_INT8 *) ltrk_info->IscTrk->ctlVal = *(ST_INT8 *) dataPtr;
      ltrkUpdateCtlTrkCommon (netInfo, domCtrl, varName, CtlTrkValues, baseName, ltrk_info->IscTrk);
      ltrk_info->ltrkUpdated.IscTrk = SD_TRUE;
      }
    else
      {
      MVL_LOG_ERR1 ("Cannot track control '%s' because 'ctlVal' is wrong type for 'IscTrk'.", varName);
      }
    }
  }
/************************************************************************/
/* NOTE: For APC, "ctlVal" is a structure.                              */
/*       We need to find the "i" and/or "f" attribute in the structure. */
/************************************************************************/
ST_VOID ltrkUpdateApcTrk (MVL_NET_INFO *netInfo,
  MVL_DOM_CTRL *domCtrl,
  ST_CHAR *varName,
  LTRK_CTL_TRK_VALUES *CtlTrkValues,
  ST_CHAR *baseName)
  {
  ST_CHAR attrName [MAX_IDENT_LEN + 1];
  ST_VOID *dataPtr;
  RUNTIME_TYPE *rtType;
  LTRK_INFO *ltrk_info = domCtrl->ltrk_info;

  /* If "i" is present, copy "i" value to "ApcIntTrk->ctlVal".  */
  strcpy (attrName, baseName);
  strncat_maxstrlen (attrName, "ctlVal$i", MAX_IDENT_LEN);
  dataPtr = mvlu_get_leaf_data_ptr (CtlTrkValues->base_va, attrName, &rtType);
  if (dataPtr && ltrk_info->ApcIntTrk)
    {
    /* Data must be ST_INT32. */
    if (rtType->el_tag == RT_INTEGER && rtType->u.p.el_len == 4)
      {
      *(ST_INT32 *) ltrk_info->ApcIntTrk->ctlVal = *(ST_INT32 *) dataPtr;
      ltrkUpdateCtlTrkCommon (netInfo, domCtrl, varName, CtlTrkValues, baseName, ltrk_info->ApcIntTrk);
      ltrk_info->ltrkUpdated.ApcIntTrk = SD_TRUE;
      }
    else
      {
      MVL_LOG_ERR1 ("Cannot track control '%s' because 'ctlVal' is wrong type for 'ApcIntTrk'.", varName);
      }
    }

  /* If "f" is present, copy "f" value to "ApcFTrk->ctlVal".  */
  strcpy (attrName, baseName);
  strncat_maxstrlen (attrName, "ctlVal$f", MAX_IDENT_LEN);
  dataPtr = mvlu_get_leaf_data_ptr (CtlTrkValues->base_va, attrName, &rtType);
  if (dataPtr && ltrk_info->ApcFTrk)
    {
    /* Data must be ST_FLOAT. */
    if (rtType->el_tag == RT_FLOATING_POINT && rtType->u.p.el_len == 4)
      {
      *(ST_FLOAT *) ltrk_info->ApcFTrk->ctlVal = *(ST_FLOAT *) dataPtr;
      ltrkUpdateCtlTrkCommon (netInfo, domCtrl, varName, CtlTrkValues, baseName, ltrk_info->ApcFTrk);
      ltrk_info->ltrkUpdated.ApcFTrk = SD_TRUE;
      }
    else
      {
      MVL_LOG_ERR1 ("Cannot track control '%s' because 'ctlVal' is wrong type for 'ApcFTrk'.", varName);
      }
    }
  }
/************************************************************************/
/* NOTE: ENC is unusual. May be multiple instances.                     */
/************************************************************************/
ST_VOID ltrkUpdateEncTrk (MVL_NET_INFO *netInfo,
  MVL_DOM_CTRL *domCtrl,
  ST_CHAR *varName,
  LTRK_CTL_TRK_VALUES *CtlTrkValues,
  ST_CHAR *baseName,
  ST_VOID *dataPtr,
  RUNTIME_TYPE *rtType)
  {
  RUNTIME_CTRL *ltrk_rt_ctrl; /* type of the LTRK LN  */
  RUNTIME_CTRL *rt_ctrl;      /* type of the LN containing this control */
  ENUM_INFO *enum_info;
  ST_INT rt_idx;
  ST_INT EncTrkIdx;
  LTRK_CTL_TRK *EncTrk;
  LTRK_INFO *ltrk_info = domCtrl->ltrk_info;

  /* Find the type of "LTRK" & store it in "ltrk_rt_ctrl".  */
  ltrk_rt_ctrl = domCtrl->ltrk_var->type_ctrl->rt_ctrl;

  rt_ctrl = CtlTrkValues->base_va->type_ctrl->rt_ctrl;

  rt_idx = rtType - rt_ctrl->rt_first;
  assert (rt_idx < rt_ctrl->rt_num);
  enum_info = mvl61850_enum_info_find (rt_ctrl, rt_idx);
  if (!enum_info)
    {
    /* cdc=ENC, but "ctlVal" type is not Enum. This is a bad data model.  */
    MVL_LOG_ERR4 ("Cannot track ENC Control '%s/%s': 'ctlVal' type is NOT Enum (el_tag=%d, el_size=%d). Check SCL.",
                  domCtrl->name, varName, rtType->el_tag, rtType->el_size);
    return;
    }

  /* Compare "enum_info->id" to values in "ltrk_rt_ctrl->EncTrkCtlValType" array.*/
  /* If match found use that array index. */
  for (EncTrkIdx = 0; EncTrkIdx < MAX_ENC_TRK; EncTrkIdx++)
    {
    ST_CHAR *id = ltrk_rt_ctrl->EncTrkCtlValType [EncTrkIdx];
    if (id && strcmp (id, enum_info->id) == 0)
      {
      break;  /* found EncTrk with same ctlVal type */
      }
    }
  if (EncTrkIdx >= MAX_ENC_TRK)
    {
    /* This error may be common if EncTrk is not configured. NERR log is used so it can easily be disabled. */
    MVL_LOG_NERR3 ("Cannot track ENC Control '%s/%s': 'ctlVal' Enum type='%s' does not match 'ctlVal' type in any EncTrk object.",
                   domCtrl->name, varName, enum_info->id);
    return;    /* This Control is not tracked. Stop now. */
    }

  /* EncTrkIdx is now the index to the correct EncTrk object in this LTRK.  */
  EncTrk = ltrk_info->EncTrkArr [EncTrkIdx];
  if (EncTrk == NULL)
    {
    /* This can happen if the EncTrkArr initialization failed.  */
    MVL_LOG_ERR3 ("Cannot track ENC Control '%s/%s': 'ctlVal'. Enum type='%s'. Something failed during initialization. See previous log messages.",
                   domCtrl->name, varName, enum_info->id);
    return;    /* This Control cannot be tracked. Stop now. */
    }

  /* Data must be ST_INT8. */
  if (rtType->el_tag == RT_INTEGER && rtType->u.p.el_len == 1)
    {
    *(ST_INT8 *) EncTrk->ctlVal = *(ST_INT8 *) dataPtr;
    ltrkUpdateCtlTrkCommon (netInfo, domCtrl, varName, CtlTrkValues, baseName, EncTrk);
    ltrk_info->ltrkUpdated.EncTrkArr [EncTrkIdx] = SD_TRUE;
    }
  else
    {
    MVL_LOG_ERR1 ("Cannot track control '%s' because 'ctlVal' is wrong type for EncTrk.", varName);
    }
  }
/************************************************************************/
/*                      ltrkUpdateCtlTrk                                */
/* Update Tracking information for ANY control.                         */
/************************************************************************/
ST_VOID ltrkUpdateCtlTrk (MVL_NET_INFO *netInfo,
  MVL_DOM_CTRL *domCtrl,
  ST_CHAR *varName,
  LTRK_CTL_TRK_VALUES *CtlTrkValues)
  {
  ST_CHAR baseName [MAX_IDENT_LEN + 1];
  ST_CHAR attrName [MAX_IDENT_LEN + 1];
  ST_VOID *dataPtr;
  RUNTIME_TYPE *rtType;
  ST_CHAR *firstdollar;
  ST_CHAR *lastdollar;

  if (domCtrl->ltrk_info == NULL)
    {
    return; /* No LTRK. Cannot track. */
    }

  /* Generate base name of the control. */
  firstdollar = strchr (varName, '$');
  if (firstdollar)
    {
    strcpy (baseName, firstdollar + 1);
    /* If attr name may be "SBO", etc. Just replace it with "Oper". Only way to find "ctlVal" type.*/
    lastdollar = strrchr (baseName, '$');
    if (lastdollar)
      {
      *(lastdollar+1) = '\0'; /* truncate */
      strncat_maxstrlen (baseName, "Oper$", MAX_IDENT_LEN);
      }
    }
  else
    {
    /* Cannot generate baseName so stop now.  */
    MVL_LOG_ERR1 ("Cannot track Control '%s': variable name is invalid.", varName);
    return;
    }

  /* Need ptr to "ctlVal" and type. */
  strcpy (attrName, baseName);
  strncat_maxstrlen (attrName, "ctlVal", MAX_IDENT_LEN);
  dataPtr = mvlu_get_leaf_data_ptr (CtlTrkValues->base_va, attrName, &rtType);
  if (dataPtr == NULL)
    {
    MVL_LOG_ERR1 ("Cannot track Control '%s': no 'ctlVal' in 'Oper'.", varName);
    return;
    }

  /* Use CDC to know which Tracking object to update. */
  switch (rtType->cdc)
    {
    case CDC_SPC:
      ltrkUpdateSpcTrk (netInfo, domCtrl, varName, CtlTrkValues, baseName, dataPtr, rtType);
      break;
    case CDC_DPC:
      ltrkUpdateDpcTrk (netInfo, domCtrl, varName, CtlTrkValues, baseName, dataPtr, rtType);
      break;
    case CDC_INC:
      ltrkUpdateIncTrk (netInfo, domCtrl, varName, CtlTrkValues, baseName, dataPtr, rtType);
      break;
    case CDC_BSC:
      ltrkUpdateBscTrk (netInfo, domCtrl, varName, CtlTrkValues, baseName, dataPtr, rtType);
      break;
    case CDC_BAC:
      ltrkUpdateBacTrk (netInfo, domCtrl, varName, CtlTrkValues, baseName, dataPtr, rtType);
      break;
    case CDC_ISC:
      ltrkUpdateIscTrk (netInfo, domCtrl, varName, CtlTrkValues, baseName, dataPtr, rtType);
      break;
    case CDC_APC:
      ltrkUpdateApcTrk (netInfo, domCtrl, varName, CtlTrkValues, baseName);
      break;
    case CDC_ENC:
      ltrkUpdateEncTrk (netInfo, domCtrl, varName, CtlTrkValues, baseName, dataPtr, rtType);
      break;
    default:
      MVL_LOG_ERR1 ("Cannot track Control '%s': invalid CDC. Check CDC in SCL file.", varName);
      break;
    }
  }


