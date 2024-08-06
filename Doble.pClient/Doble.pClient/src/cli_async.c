/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*      (c) Copyright Systems Integration Specialists Company, Inc.,    */
/*      2015 - 2015, All Rights Reserved                                */
/*                                                                      */
/* MODULE NAME : cli_async.c                                            */
/* PRODUCT(S)  : MMS-LITE                                               */
/*                                                                      */
/* MODULE DESCRIPTION :                                                 */
/*      Examples of Asynchronous request functions.                     */
/*                                                                      */
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :                            */
/*      named_var_read_async                                            */
/*      named_var_read_async_done                                       */
/*      named_var_write_async                                           */
/*      named_var_write_async_done                                      */
/*                                                                      */
/* MODIFICATION LOG :                                                   */
/*  Date     Who  Comments                                              */
/* --------  ---  ----------------------------------------------------  */
/* 07/15/16  JRB  Add USE_TYPE_PTR option.                              */
/* 01/06/15  JRB  New functions to demonstrate Asynchronous requests.   */
/************************************************************************/
#include "glbtypes.h"
#include "sysincs.h"
#include "client.h"
#include "sx_arb.h"
#include "mloguser.h"
/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of      */
/* __FILE__ strings.                                                    */
/************************************************************************/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/************************************************************************/
/*                       named_var_read_async                           */
/* Read a single named variable asynchronously.                         */
/* "u_req_done" (user function) is called when response is received.    */
/*   scope = VMD_SPEC, DOM_SPEC, or AA_SPEC                             */
/* CRITICAL: request information must be allocated because it is        */
/*           referenced later when the response is received.            */
/* NOTE: this function always reads 1 variable, but it could be modified*/
/*       to read multiple variables (see mvla_read_variables documentation).*/
/************************************************************************/
ST_RET named_var_read_async (MVL_NET_INFO *net_info,
  ST_CHAR *varName,
  ST_INT scope,
  ST_CHAR *domName,
  MVL_TYPE_ID type_id,
  ST_VOID *dataDest,  /* must point to valid memory until resp rcvd  */
  ST_VOID (*u_req_done)(MVL_REQ_PEND *reqCtrl))  /* called when resp rcvd*/
  {
  MVL_READ_RESP_PARSE_INFO *parse_info;    /* array of structures */
  READ_REQ_INFO *req_info;
  VARIABLE_LIST *vl;
  ST_INT ret;
  MVL_REQ_PEND *reqCtrl;
  ST_INT num_data = 1;  /* this always reads exactly 1 variable   */

  /* CRITICAL: Must always allocate space for READ_REQ_INFO +   */
  /*           array of VARIABLE_LIST                           */
  /*           (see "read_req_info" definition in the Reference Manual).*/
  /*           Memory corruption may occur if this allocation is too small.*/
  req_info = (READ_REQ_INFO *) chk_calloc (1, sizeof (READ_REQ_INFO) +
                                           num_data * sizeof (VARIABLE_LIST));

  req_info->spec_in_result = SD_FALSE;
  req_info->va_spec.var_acc_tag = VAR_ACC_VARLIST;
  req_info->va_spec.num_of_variables = num_data;

  vl = (VARIABLE_LIST *) (req_info + 1);
  vl->alt_access_pres = SD_FALSE;
  vl->var_spec.var_spec_tag = VA_SPEC_NAMED;
  vl->var_spec.vs.name.object_tag = scope;      /* set scope    */
  if (scope == DOM_SPEC)
    vl->var_spec.vs.name.domain_id = domName;   /* set domain name      */
  vl->var_spec.vs.name.obj_name.vmd_spec = varName;


  /* Allocate & init array of ..PARSE_INFO structures.  */
  parse_info = chk_calloc (num_data, sizeof (MVL_READ_RESP_PARSE_INFO));
  parse_info->dest = dataDest;
  parse_info->type_id = type_id;
  parse_info->descr_arr = SD_FALSE;

  /* Send read request. */
  ret = mvla_read_variables (net_info, req_info, num_data,
                             parse_info, &reqCtrl);
  if (ret == SD_SUCCESS)
    {
    /* set done function pointer. */
    reqCtrl->u_req_done = u_req_done;
    /* Save "req_info" in "v" to use in "u_req_done" function. */
    /* NOTE: must free it in "u_req_done" function.            */
    reqCtrl->v = req_info;
    }
  else
    {
    chk_free (req_info);
    chk_free (parse_info);
    }
  return (ret);
  }
/************************************************************************/
/*                       named_var_read_async_done                      */
/* USER FUNCTION, MODIFY AS NEEDED.                                     */
/* Called when Asynchronous Read Response is received.                  */
/************************************************************************/
ST_VOID named_var_read_async_done (MVL_REQ_PEND *reqCtrl)
  {
  READ_REQ_INFO *req_info;
  VARIABLE_LIST *vl;
  ST_CHAR *varName0;  /* first (and only) variable name */
  MVL_TYPE_CTRL *type_ctrl;
  /* Find request information saved in "named_var_read_async".  */
  /* NOTE: always exactly 1 variable in VARIABLE_LIST.          */
  assert (reqCtrl->u.rd.num_data == 1);
  req_info = (READ_REQ_INFO *) reqCtrl->v;
  vl = (VARIABLE_LIST *) (req_info + 1);
  varName0 = vl->var_spec.vs.name.obj_name.vmd_spec;

  /* Read request may completely fail, or there may be an error for the one and only variable.*/
  if (reqCtrl->result)
    printf ("\n read error = 0x%X (no variables read)", reqCtrl->result);
  else if (reqCtrl->u.rd.parse_info->result)
    printf ("\n read error for variable [0] (%s) = 0x%X", varName0, reqCtrl->u.rd.parse_info->result);
  else
    {
    printf ("\n '%s' read successful", varName0);
    /* Do this to just log the data. Works for any variable.  */
    SLOGALWAYS1 ("Read response DATA for variable '%s':", varName0);
    type_ctrl = mvl_type_ctrl_find (reqCtrl->u.rd.parse_info->type_id);
    sxd_process_arb_data(reqCtrl->u.rd.parse_info->dest, type_ctrl->rt, type_ctrl->num_rt,
                         NULL,  /* pointer to user data. Not needed.    */
                         &mlog_arb_log_ctrl,    /* function ptr table   */
                         NULL); /* pointer to "elPres" array. Always NULL.*/
    }
  /* free data buffer allocated BEFORE named_var_read_async (see client.c).*/
  chk_free (reqCtrl->u.rd.parse_info->dest);
  /* free buffers allocated by named_var_read_async */
  chk_free (reqCtrl->u.rd.parse_info);
  chk_free (req_info);
  mvl_free_req_ctrl (reqCtrl);
  }
/************************************************************************/
/*                      named_var_write_async                           */
/* Write a single named variable asynchronously.                        */
/* "u_req_done" (user function) is called when response is received.    */
/*   scope = VMD_SPEC, DOM_SPEC, or AA_SPEC                             */
/*                                                                      */
/* CRITICAL: request information must be allocated because it is        */
/*           referenced later when the response is received.            */
/* NOTE: this function always writes 1 variable, but it could be modified*/
/*       to write multiple variables (see mvla_write_variables documentation).*/
/************************************************************************/
ST_RET named_var_write_async (MVL_NET_INFO *netInfo,
  ST_CHAR *varName,
  ST_INT scope,
  ST_CHAR *domName,
  MVL_TYPE_ID type_id,
  ST_VOID *dataSrc,
  ST_VOID (*u_req_done)(MVL_REQ_PEND *reqCtrl))  /* called when resp rcvd*/
  {
  MVL_WRITE_REQ_INFO *wr_info;
  WRITE_REQ_INFO *req_info;
  VARIABLE_LIST *vl;
  ST_RET ret;
  MVL_REQ_PEND *reqCtrl;
  ST_INT num_data = 1;  /* this always writes exactly 1 variable   */

  /* CRITICAL: Must always allocate space for WRITE_REQ_INFO +  */
  /*           array of VARIABLE_LIST + array of VAR_ACC_DATA   */
  /*           (see "write_req_info" definition in the Reference Manual).*/
  /*           Memory corruption may occur if this allocation is too small.*/
  req_info = (WRITE_REQ_INFO *) chk_calloc (1, sizeof (WRITE_REQ_INFO) +
                                           num_data * sizeof (VARIABLE_LIST) +
                                           num_data * sizeof(VAR_ACC_DATA));
  req_info->num_of_data = num_data;
  req_info->va_spec.var_acc_tag = VAR_ACC_VARLIST;
  req_info->va_spec.num_of_variables = 1;
  vl = (VARIABLE_LIST *) (req_info + 1);

  vl->alt_access_pres = SD_FALSE;
  vl->var_spec.var_spec_tag = VA_SPEC_NAMED;
  vl->var_spec.vs.name.object_tag = scope;      /* set scope    */
  if (scope == DOM_SPEC)
    vl->var_spec.vs.name.domain_id = domName;   /* set domain name      */
  vl->var_spec.vs.name.obj_name.vmd_spec = varName;

  /* Alloc array of structs. One struct for each variable.              */
  /* CRITICAL: response code writes to this array of structs.   */
  /*           Do not free until response is processed.         */
  wr_info = chk_calloc (num_data, sizeof (MVL_WRITE_REQ_INFO));

  wr_info->local_data = dataSrc;
  wr_info->type_id = type_id;
  wr_info->arr = SD_FALSE;

  /* Send write request.        */
  ret = mvla_write_variables (netInfo, req_info, 1, wr_info, &reqCtrl);

  if (ret == SD_SUCCESS)
    {
    /* set done function pointer. */
    reqCtrl->u_req_done = u_req_done;
    /* Save "req_info" in "v" to use in "u_req_done" function. */
    /* NOTE: must free it in "u_req_done" function.            */
    reqCtrl->v = req_info;
    }
  return (ret);
  }
/************************************************************************/
/*                      named_var_write_async_done                      */
/* USER FUNCTION, MODIFY AS NEEDED.                                     */
/* Called when Asynchronous Write Response is received.                 */
/************************************************************************/
ST_VOID named_var_write_async_done (MVL_REQ_PEND *reqCtrl)
  {
  WRITE_REQ_INFO *req_info;
  VARIABLE_LIST *vl;
  ST_CHAR *varName0;  /* first (and only) variable name */
  /* Find request information saved in "named_var_write_async".  */
  /* NOTE: always exactly 1 variable in VARIABLE_LIST.          */
  assert (reqCtrl->u.wr.num_data == 1);
  req_info = (WRITE_REQ_INFO *) reqCtrl->v;
  vl = (VARIABLE_LIST *) (req_info + 1);
  varName0 = vl->var_spec.vs.name.obj_name.vmd_spec;

  /* Write request may completely fail, or there may be an error for the one and only variable.*/
  if (reqCtrl->result)
    printf ("\n write error = 0x%X (no variables read)", reqCtrl->result);
  else if (reqCtrl->u.wr.w_info->result)
    printf ("\n write error for variable [0] (%s) = 0x%X", varName0, reqCtrl->u.wr.w_info->result);
  else
    {
    printf ("\n '%s' write successful", varName0);
    }
  /* free resources allocated by asynchronous request function */
  chk_free (reqCtrl->u.wr.w_info);
  chk_free (req_info);
  mvl_free_req_ctrl (reqCtrl);
  }
