/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*      (c) Copyright Systems Integration Specialists Company, Inc.,    */
/*      2017 - 2018, All Rights Reserved                                */
/*                                                                      */
/* MODULE NAME : cli_nvl.c                                              */
/* PRODUCT(S)  : MMS-LITE                                               */
/*                                                                      */
/* MODULE DESCRIPTION :                                                 */
/*      Client function to read a Named Variable List (NVL).            */
/*                                                                      */
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :                            */
/*      nvl_read                                                        */
/*      nvl_write                                                       */
/*                                                                      */
/* MODIFICATION LOG :                                                   */
/*  Date     Who  Comments                                              */
/* --------  ---  ----------------------------------------------------  */
/* 01/05/18  JRB  Add "nvl_write" function.                             */
/* 04/27/17  JRB  New function to read NVL.                             */
/************************************************************************/
#include "glbtypes.h"
#include "sysincs.h"
#include "client.h"
#include "sx_arb.h"
#include "mloguser.h"

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/************************************************************************/
/*      nvl_read                                                        */
/* Read a NVL from a remote VMD.                                        */
/* NOTE: The NVL must be defined in the remote VMD. The remote VMD may  */
/*       be initialized by calling "config_iec_remote_vmd" to process   */
/*       the SCL file of a remote server.                               */
/************************************************************************/
ST_RET nvl_read (MVL_NET_INFO *net_info, MVL_VMD_CTRL *remote_vmd_ctrl, ST_CHAR *nvlName,
  ST_INT scope, ST_CHAR *domName, ST_INT timeOut)
  {
  ST_INT num_data;
  MVL_READ_RESP_PARSE_INFO *parse_info; /* allocate array of structures */
  READ_REQ_INFO *req_info;
  ST_INT ret;
  MVL_REQ_PEND *reqCtrl;
  MVL_NVLIST_CTRL *nvl;
  OBJECT_NAME nvl_oname;
  ST_INT j;
  ST_CHAR adl [100];  /* Alternate Access definition as text (ADL format) */

  if (scope == DOM_SPEC)
    SLOGALWAYS2 ("Reading NVL='%s' in domain='%s'", nvlName, domName);
  else
    SLOGALWAYS1 ("Reading NVL='%s'", nvlName);

  nvl_oname.object_tag = scope;
  nvl_oname.domain_id = domName;      /* domain name  */
  nvl_oname.obj_name.item_id = nvlName;

  nvl = mvl_vmd_find_nvl (remote_vmd_ctrl, &nvl_oname, net_info);
  if (nvl == NULL)
    {
    SLOGALWAYS1 ("Read ERROR: cannot find NVL='%s'", nvlName);
    return (SD_FAILURE);
    }
  num_data = nvl->num_of_entries;

  req_info = (READ_REQ_INFO *) chk_calloc (1, sizeof (READ_REQ_INFO));
  req_info->spec_in_result = SD_FALSE;
  req_info->va_spec.var_acc_tag = VAR_ACC_NAMEDLIST;
  memcpy (&req_info->va_spec.vl_name, &nvl_oname, sizeof(OBJECT_NAME));

  /* NOTE: Don't need array of VARIABLE_LIST or req_info->va_spec.num_of_variables. */
  /*       The NVL defines the variables. */

  /* Allocate and initialize this array.  */
  parse_info = (MVL_READ_RESP_PARSE_INFO *) chk_calloc (num_data, sizeof (MVL_READ_RESP_PARSE_INFO));
  for (j = 0; j < num_data; j++)
    {
    parse_info[j].dest = chk_malloc (nvl->entries[j]->type_ctrl->data_size);
    parse_info[j].type_id = nvl->entries[j]->type_id;
    parse_info[j].descr_arr = SD_FALSE;
    }

  /* Send read request. */
  ret = mvla_read_variables (net_info, req_info, num_data,
                             parse_info, &reqCtrl);

  /* If request sent successfully, wait for reply.      */
  if (ret == SD_SUCCESS)
    ret = waitReqDone (reqCtrl, timeOut);

  /* Log the results. */
  if (ret == SD_SUCCESS)
    {
    SLOGALWAYS2 ("Read Results for NVL='%s' (%d variables)", nvl->name, num_data);
    for (j = 0; j < num_data; j++)
      {
      /* NOTE: There is a separate result for each variable in the NVL. */
      /* If Alternate Access is present, log it to help user understand the data.*/
      if (nvl->altAccArray && nvl->altAccArray [j])
        {
        if (ms_aa_to_adl (nvl->altAccArray [j], adl, sizeof(adl)-1) != SD_SUCCESS)
          strcpy (adl, "Error generating ADL");
        SLOGCALWAYS4 ("Variable [%d]='%s' (Alternate Access ADL=%s): Result=%s", j, nvl->entries[j]->name, adl,
                   parse_info[j].result == SD_SUCCESS ? "Success" : "Failure");
        }
      else
        SLOGCALWAYS3 ("Variable [%d]='%s': Result=%s", j, nvl->entries[j]->name,
                   parse_info[j].result == SD_SUCCESS ? "Success" : "Failure");
      /* If successful, log the data according to the type of each variable in the NVL.*/
      if (parse_info[j].result == SD_SUCCESS)
        sxd_process_arb_data (parse_info[j].dest, nvl->entries[j]->type_ctrl->rt,
                  nvl->entries[j]->type_ctrl->num_rt, NULL, &mlog_arb_log_ctrl, NULL);
      }
    }
  /* do cleanup */
  for (j = 0; j < num_data; j++)
    chk_free (parse_info[j].dest);
  chk_free (parse_info);
  chk_free (req_info);
  mvl_free_req_ctrl (reqCtrl);
  return (ret);
  }

/************************************************************************/
/*      nvl_write                                                        */
/* Write a NVL to a remote VMD.                                         */
/* NOTE: The NVL must be defined in the remote VMD. The remote VMD may  */
/*       be initialized by calling "config_iec_remote_vmd" to process   */
/*       the SCL file of a remote server.                               */
/************************************************************************/
ST_RET nvl_write (MVL_NET_INFO *net_info, MVL_VMD_CTRL *remote_vmd_ctrl, ST_CHAR *nvlName,
  ST_INT scope, ST_CHAR *domName,
  ST_VOID **data,     /* array of pointers to data  */
  ST_INT *data_size,  /* array of sizes of data    */
  ST_INT num_data,    /* number of pointers in data array */
                      /* This must match the number of variables in the NVL.*/
  ST_INT timeOut)
  {
  WRITE_REQ_INFO *req_info;
  ST_INT ret;
  MVL_REQ_PEND *reqCtrl;
  MVL_NVLIST_CTRL *nvl;
  OBJECT_NAME nvl_oname;
  ST_INT j;
  ST_CHAR adl [100];  /* Alternate Access definition as text (ADL format) */
  MVL_WRITE_REQ_INFO *wr_info;  /* ptr to array of structs  */

  if (scope == DOM_SPEC)
    SLOGALWAYS2 ("Writing NVL='%s' in domain='%s'", nvlName, domName);
  else
    SLOGALWAYS1 ("Writing NVL='%s'", nvlName);

  nvl_oname.object_tag = scope;
  nvl_oname.domain_id = domName;      /* domain name  */
  nvl_oname.obj_name.item_id = nvlName;

  nvl = mvl_vmd_find_nvl (remote_vmd_ctrl, &nvl_oname, net_info);
  if (nvl == NULL)
    {
    SLOGALWAYS1 ("NVL Write ERROR: cannot find NVL='%s'", nvlName);
    return (SD_FAILURE);
    }
  if (num_data != nvl->num_of_entries)
    {
    SLOGALWAYS1 ("NVL Write ERROR: wrong number of variables for NVL='%s'", nvlName);
    return (SD_FAILURE);
    }
  for (j = 0; j < num_data; j++)
    {
    if (data_size[j] < nvl->entries[j]->type_ctrl->data_size)
      {
      SLOGALWAYS2 ("NVL Write ERROR: size of data entry [%d] does not match size in NVL='%s'", j, nvlName);
      return (SD_FAILURE);
      }
    }

  /* CRITICAL: Must always allocate space for WRITE_REQ_INFO +  */
  /*           array of VAR_ACC_DATA   */
  /*           (see "write_req_info" definition in the Reference Manual).*/
  /*           Memory corruption may occur if this allocation is too small.*/
  req_info = (WRITE_REQ_INFO *) chk_calloc (1, sizeof (WRITE_REQ_INFO) +
                                           num_data * sizeof(VAR_ACC_DATA));

  req_info->va_spec.var_acc_tag = VAR_ACC_NAMEDLIST;
  memcpy (&req_info->va_spec.vl_name, &nvl_oname, sizeof(OBJECT_NAME));

  /* NOTE: Don't need array of VARIABLE_LIST or req_info->va_spec.num_of_variables. */
  /*       The NVL defines the variables. */

  wr_info = (MVL_WRITE_REQ_INFO *) chk_calloc (num_data, sizeof (MVL_WRITE_REQ_INFO));
  for (j = 0; j < num_data; j++)
    {
    /* CAUTION: data[j] must point to the correct data for this type_id*/
    wr_info[j].local_data = data[j];
    wr_info[j].type_id = nvl->entries[j]->type_id;
    wr_info[j].arr = SD_FALSE;
    }

  ret = mvla_write_variables (net_info, req_info, num_data, wr_info, &reqCtrl);

  /* If request sent successfully, wait for reply.      */
  if (ret == SD_SUCCESS)
    ret = waitReqDone (reqCtrl, timeOut);

  /* Log the results. */
  if (ret == SD_SUCCESS)
    {
    SLOGALWAYS2 ("Write Results for NVL='%s' (%d variables)", nvl->name, num_data);
    for (j = 0; j < num_data; j++)
      {
      /* NOTE: There is a separate result for each variable in the NVL. */
      /* If Alternate Access is present, log it to help user understand the data.*/
      if (nvl->altAccArray && nvl->altAccArray [j])
        {
        if (ms_aa_to_adl (nvl->altAccArray [j], adl, sizeof(adl)-1) != SD_SUCCESS)
          strcpy (adl, "Error generating ADL");
        SLOGCALWAYS4 ("Variable [%d]='%s' (Alternate Access ADL=%s): Result=%s", j, nvl->entries[j]->name, adl,
                    wr_info[j].result == SD_SUCCESS ? "Success" : "Failure");
        }
      else
        SLOGCALWAYS3 ("Variable [%d]='%s' : Result=%s", j, nvl->entries[j]->name,
                    wr_info[j].result == SD_SUCCESS ? "Success" : "Failure");
      }
    }
  else
    SLOGALWAYS1 ("Write FAILED for NVL='%s'", nvl->name);

  /* do cleanup */
  chk_free (wr_info);
  chk_free (req_info);
  mvl_free_req_ctrl (reqCtrl);
  return (ret);
  }

