/*******************************************************************************
File      : function.cpp
By        : Arun Lal K M
Date      : 18th Jun 2019
*******************************************************************************/

/*include*/
#include "glbtypes.h"
#include "sysincs.h"
#include "signal.h"
#include "mmsdefs.h"
#include "mms_pvmd.h"
#include "mms_pvar.h"
#include "mms_vvar.h"
#include "mms_err.h"
#include "mms_pcon.h"
#include "asn1defs.h"
#include "mmsop_en.h"

#include "mvl_acse.h"
#include "mvl_log.h"
#include "tp4api.h"
#include "clntobj.h"
#include "client.h"
#include "smpval.h"

#include "lean_a.h"     /* need for DIB_ENTRY if HARD_CODED_CFG */
#include "scl.h"
#include "sx_arb.h"     /* needed for sxd_process..     */
#include "mloguser.h"   /* needed for mlog_arb_log_ctrl */

#if defined(SISCO_STACK_CFG)
#if defined(USE_DIB_LIST)
#error USE_DIB_LIST should not be defined when SISCO_STACK_CFG is defined
#endif
#include "sstackcfg.h"
#endif

#if defined(GOOSE_RX_SUPP) || defined(GSSE_RX_SUPP) || defined(GSE_MGMT_RX_SUPP) || defined(SMPVAL_RX_SUPP)
#include "subnet.h"
#endif
#include "str_util.h"   /* for strncat_maxstrlen, etc.                  */

#include "function.h"
#include "usermap.h"

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR* SD_CONST thisFileName = __FILE__;
#endif

/*variables*/
int gInit = 0;
extern ST_BOOLEAN doIt;
MVL_CFG_INFO mvlCfg = { 0 };

/*function definitions*/

void
pClientLog (char* text, char* caption, int code)
    {
    MessageBox (0, text, caption, MB_OK);
    _ASSERT_ (0);
    }

int
addToDibTable (char* name, char* ip)
    {
    int ret = SD_FAILURE;

    DIB_ENTRY* pTempDibEntry = NULL;

    pTempDibEntry = (DIB_ENTRY*)chk_malloc (sizeof (DIB_ENTRY));
    if (pTempDibEntry)
        {
        memcpy (pTempDibEntry, &rem_dib_table[0], sizeof (DIB_ENTRY));

        if (inet_pton (AF_INET, ip, &pTempDibEntry->pres_addr.netAddr.ip))
            {
            strncpy (pTempDibEntry->name, name, sizeof (pTempDibEntry->name));

            dib_entry_create (pTempDibEntry);
            ret = SD_SUCCESS;
            }
        }

    return ret;
    }

void
removeFromDibTable (char* name)
    {
    DIB_ENTRY* pDibEntry = NULL;

    pDibEntry = find_dib_entry (name);
    if (pDibEntry)
        {
        dib_entry_destroy (pDibEntry);
        }
    }

ST_RET
waitReqDoneEx (MVL_REQ_PEND* req, ST_INT timeout)
    {
    ST_DOUBLE stopTime = sGetMsTime () + (ST_DOUBLE)timeout * 1000.0;

    while (req->done == SD_FALSE)         /* wait until done      */
        {
        if (timeout != 0 && sGetMsTime () > stopTime)
            {
            req->result = MVL_ERR_USR_TIMEOUT;
            MVL_LOG_ERR1 ("MMS request (opcode = %d) timed out", req->op);
            break;
            }
        if (doIt == SD_FALSE)       /* user hit Ctrl-C, stop waiting        */
            {
            req->result = SD_FAILURE;
            break;
            }
        doCommService ();   /* wait for events if possible & call mvl_comm_serve*/
        }   /* end loop     */
    return (req->result);
    }

ST_INT
getVarTypeString (int  el_len, int el_tag, char* temp_buf)
    {
    temp_buf[0] = '\0';

    switch (el_tag)
        {
            case RT_ARR_START:
                break;

            case RT_STR_START:
                break;

            case RT_BOOL:
                strcpy (temp_buf, "Bool");
                break;

            case RT_BIT_STRING:
                if (el_len < 0)
                    sprintf (temp_buf, "BVstring%d", abs (el_len));
                else
                    sprintf (temp_buf, "Bstring%d", el_len);
                break;

            case RT_INTEGER:
                switch (el_len)
                    {
                        case 1:
                            strcpy (temp_buf, "Byte");
                            break;
                        case 2:
                            strcpy (temp_buf, "Short");
                            break;
                        case 4:
                            strcpy (temp_buf, "Long");
                            break;
#ifdef INT64_SUPPORT
                        case 8:
                            strcpy (temp_buf, "Int64");
                            break;
#endif
                    }
                break;

            case RT_UNSIGNED:
                switch (el_len)
                    {
                        case 1:
                            strcpy (temp_buf, "Ubyte");
                            break;
                        case 2:
                            strcpy (temp_buf, "Ushort");
                            break;
                        case 4:
                            strcpy (temp_buf, "Ulong");
                            break;
#ifdef INT64_SUPPORT
                        case 8:
                            strcpy (temp_buf, "Uint64");
                            break;
#endif
                    }
                break;

            case RT_FLOATING_POINT:
                switch (el_len)
                    {
                        case 4:
                            strcpy (temp_buf, "Float");
                            break;
                        case 8:
                            strcpy (temp_buf, "Double");
                            break;
                    }
                break;

            case RT_OCTET_STRING:
                if (el_len < 0)
                    sprintf (temp_buf, "OVstring%d", abs (el_len));
                else
                    sprintf (temp_buf, "Ostring%d", el_len);
                break;

            case RT_VISIBLE_STRING:
                if (el_len < 0)
                    sprintf (temp_buf, "Vstring%d", abs (el_len));
                else
                    sprintf (temp_buf, "Fstring%d", el_len);
                break;

            case RT_GENERAL_TIME:
                strcpy (temp_buf, "Gtime");
                break;

            case RT_BINARY_TIME:
                sprintf (temp_buf, "Btime%d", el_len);
                break;

            case RT_BCD:
                sprintf (temp_buf, "Bcd%d", (ST_INT)el_len);
                break;

            case RT_BOOLEANARRAY:
                break;

            case RT_UTC_TIME:
                strcpy (temp_buf, "Utctime");
                break;

            case RT_UTF8_STRING:
                if (el_len < 0)
                    sprintf (temp_buf, "UTF8Vstring%d", (ST_INT)abs (el_len));
                else
                    sprintf (temp_buf, "UTF8string%d", (ST_INT)(el_len));
                break;

            case RT_STR_END:
                break;

            case RT_ARR_END:
                break;
            default:
                break;
        }

    if (strcmp (temp_buf, "") == 0)
        {
        return SD_FAILURE;
        }
    else
        {
        return SD_SUCCESS;
        }
    }

MVL_TYPE_ID
findOrCreateTypeId (ST_CHAR* type_name, ST_CHAR* tdl)
    {
    MVL_TYPE_ID type_id;

    if ((type_id = mvl_typename_to_typeid (type_name)) == MVL_TYPE_ID_INVALID)       /* find */
        type_id = mvl_type_id_create_from_tdl (type_name, tdl);     /* create*/

    return (type_id);
    }

MVL_TYPE_ID
getVarRuntimeType (int  el_tag, int  el_len)
    {
    char  string[64] = { 0 };
    char  rt_string[64] = { 0 };

    if (getVarTypeString (el_len, el_tag, string) == SD_SUCCESS)
        {
        sprintf (rt_string, "rt_%s", string);
        return  (findOrCreateTypeId (rt_string, string));
        }
    else
        {
        return MVL_TYPE_ID_INVALID;
        }
    }

int
init ()
    {
    int ret = SD_FAILURE;
    int j = 0;

    init_mem ();
    init_log_cfg ();

    if (SD_SUCCESS == osicfgx ("osicfg.xml", &mvlCfg))
        {
        ret = mvl_start_acse (&mvlCfg);

        if (SD_SUCCESS == ret)
            {
#if defined(USE_DIB_LIST)
            /* First copy addresses from "osicfg.xml" (stored in "loc_dib_table"  */
            /* and "rem_dib_table" arrays) to the new DIB list.                   */
            /* NOTE: Client address is probably not in the SCL file, so it should */
            /*       come from here.                                              */
            for (j = 0; j < num_loc_dib_entries; j++)
                {
                dib_entry_create (&loc_dib_table[j]);       /* add to new DIB list  */
                }
#endif  /* USE_DIB_LIST */
            }
        }

    gInit = 1;

    return ret;
    }

int
post () /*TODO: Arun*/
    {
    int ret = SD_FAILURE;

    ret = mvl_end_acse ();
    if (ret != SD_SUCCESS)
        {
        pClientLog ("CODE 15", "ERROR", -1);
        }

    gInit = 0;

    return ret;
    }

void
utcToString (MMS_UTC_TIME* data_ptr, char* buf)
    {
    STDTIME st = { 0 };
    STDTIME_STRING ss = { 0 };

    MmsUtcTimeToStdTime (data_ptr, &st);
    StdTimeToStdTimeString (&st, &ss);
    strcpy (buf, ss.str);

    return;
    }

void
stringToUtc (MMS_UTC_TIME* data_ptr, char* buf)
    {
    STDTIME st = { 0 };
    STDTIME_STRING ss = { 0 };

    strcpy (ss.str, buf);
    StdTimeStringAToStdTime (&ss, &st);
    StdTimeToMmsUtcTime (&st, data_ptr);

    strcpy (buf, ss.str);

    return;
    }

ST_RET
namedVarReadAsync (MVL_NET_INFO* net_info,
                   ST_CHAR* varName,
                   ST_INT scope,
                   ST_CHAR* domName,
                   MVL_TYPE_ID type_id,
                   ST_VOID* dataDest,
                   MVL_REQ_PEND** reqCtrl,
                   READ_REQ_INFO** req_info,
                   MVL_READ_RESP_PARSE_INFO** parse_info)
    {
    ST_INT num_data;
    VARIABLE_LIST* vl;
    ST_INT ret;

    num_data = 1;   /* this function always reads exactly 1 variable   */

    /* CRITICAL: Must always allocate space for READ_REQ_INFO +   */
    /*           array of VARIABLE_LIST                           */
    /*           (see "read_req_info" definition in the Reference Manual).*/
    /*           Memory corruption may occur if this allocation is too small.*/
    (*req_info) = (READ_REQ_INFO*)chk_calloc (1, sizeof (READ_REQ_INFO) +
                                              num_data * sizeof (VARIABLE_LIST));
    (*req_info)->spec_in_result = SD_FALSE;
    (*req_info)->va_spec.var_acc_tag = VAR_ACC_VARLIST;
    (*req_info)->va_spec.num_of_variables = 1;
    vl = (VARIABLE_LIST*)((*req_info) + 1);

    vl->alt_access_pres = SD_FALSE;
    vl->var_spec.var_spec_tag = VA_SPEC_NAMED;
    vl->var_spec.vs.name.object_tag = scope;      /* set scope    */
    if (scope == DOM_SPEC)
        vl->var_spec.vs.name.domain_id = domName;   /* set domain name      */
    vl->var_spec.vs.name.obj_name.vmd_spec = varName;

    /* IMPORTANT: start with clean structure, then set appropriate elements.*/
    (*parse_info) = chk_calloc (num_data, sizeof (MVL_READ_RESP_PARSE_INFO));
    (*parse_info)->dest = dataDest;
    (*parse_info)->type_id = type_id;
    (*parse_info)->descr_arr = SD_FALSE;

    /* Send read request. */
    ret = mvla_read_variables (net_info, (*req_info), num_data,
        (*parse_info), reqCtrl);

    /* If request sent successfully, wait for reply.      */
    if (ret != SD_SUCCESS)
        {
        chk_free ((*req_info));
        mvl_free_req_ctrl (*reqCtrl);
        }

    return ret;
    }

ST_RET get_last_string (ST_CHAR* astr, char* dst)
    {
    char* numStr;
    ST_CHAR* seps = "$"; /* delimiters to search for     */

    strcpy (dst, "");

    numStr = get_next_string (&astr, seps);       /* "astr" ptr changes   */
    while (numStr && strlen (numStr) > 0)
        {
        strcpy (dst, numStr);
        numStr = get_next_string (&astr, seps);
        }

    return (SD_SUCCESS);
    }

void string2hexString (char* input, char* output)
    {
    int loop;
    int i;

    i = 0;
    loop = 0;

    while (input[loop] != '\0')
        {
        sprintf ((char*)(output + i), "%01X", input[loop]);
        loop += 1;
        i += 1;
        }
    //insert NULL at the end of the output string
    output[i++] = '\0';
    }

void
addStringToEnd (char* str1, char* str2)
    {
    strcat(str1, ".");
    strcat(str1, str2);
    }

void
removeStringFromEnd (char* str)
    {
    int i = 0;

    for (i = strlen (str); i >= 0; i--)
        {
        if (str[i] == '.')
            {
            str[i] = '\0';
            break;
            }
        }
    }

ST_RET named_var_write_ex (MVL_NET_INFO *netInfo, ST_CHAR *varName,
  ST_INT scope, ST_CHAR *domName,
  MVL_TYPE_ID type_id, ST_VOID *dataSrc, ST_INT timeOut, int* error)
  {
  MVL_WRITE_REQ_INFO *wr_info;
  WRITE_REQ_INFO *req_info;
  VARIABLE_LIST *vl;
  ST_RET ret;
  MVL_REQ_PEND *reqCtrl;
  ST_INT num_data;

  num_data = 1;   /* this function always writes exactly 1 variable   */

  /* CRITICAL: Must always allocate space for WRITE_REQ_INFO +  */
  /*           array of VARIABLE_LIST + array of VAR_ACC_DATA   */
  /*           (see "write_req_info" definition in the Reference Manual).*/
  /*           Memory corruption may occur if this allocation is too small.*/
  req_info = (WRITE_REQ_INFO *) chk_calloc (1, sizeof (WRITE_REQ_INFO) +
                                           num_data * sizeof (VARIABLE_LIST) +
                                           num_data * sizeof(VAR_ACC_DATA));
  req_info->num_of_data = 1;
  req_info->va_spec.var_acc_tag = VAR_ACC_VARLIST;
  req_info->va_spec.num_of_variables = 1;
  vl = (VARIABLE_LIST *) (req_info + 1);

  vl->alt_access_pres = SD_FALSE;
  vl->var_spec.var_spec_tag = VA_SPEC_NAMED;
  vl->var_spec.vs.name.object_tag = scope;      /* set scope    */
  if (scope == DOM_SPEC)
    vl->var_spec.vs.name.domain_id = domName;   /* set domain name      */
  vl->var_spec.vs.name.obj_name.vmd_spec = varName;

  /* Alloc array of structs. Here only one var so alloc one struct.     */
  /* To read multiple vars, allocate more.                              */
  /* CRITICAL: response code writes to this struct. Do not free until   */
  /*           response received.                                       */
  wr_info = chk_calloc (1, sizeof (MVL_WRITE_REQ_INFO));

  wr_info->local_data = dataSrc;
  wr_info->type_id = type_id;
  wr_info->arr = SD_FALSE;

  /* Send write request.        */
  ret = mvla_write_variables (netInfo, req_info, 1, wr_info, &reqCtrl);

  /* If request sent successfully, wait for response.   */
  if (ret == SD_SUCCESS)
    ret = waitReqDone (reqCtrl, timeOut);

  chk_free (req_info);
  mvl_free_req_ctrl (reqCtrl);
  if (ret == SD_SUCCESS)        /* response received OK.        */
      {
      ret = wr_info->result;      /* return single variable result        */
      *error = wr_info->failure;
      }
  chk_free (wr_info);

  return (ret);
  }

void 
writeRespToString (int err, char* str)
    {
    switch (err)
        {
            case 3:
                {
                strcpy (str, "Object access denied");
                }
                break;
            case 11:
                {
                strcpy (str, "Object non existent");
                }
                break;
            default:
                {
                strcpy (str, "Unknown");
                }
                break;
        }
    }


void 
addCauseToString (int err, char* str)
    {
    switch (err)
        {
            case ADD_CAUSE_NOT_SUPPORTED:
                {
                strcpy (str, "NOT SUPPORTED");
                }
                break;
            case ADD_CAUSE_BLOCKED_BY_SWITCHING_HIERARCHY:
                {
                strcpy (str, "BLOCKED BY SWITCHING HIERARCHY");
                }
                break;
            case ADD_CAUSE_SELECT_FAILED:
                {
                strcpy (str, "SELECT FAILED");
                }
                break;
            case ADD_CAUSE_INVALID_POSITION:
                {
                strcpy (str, "INVALID POSITION");
                }
                break;
            case ADD_CAUSE_POSITION_REACHED:
                {
                strcpy (str, "POSITION REACHED");
                }
                break;
            case ADD_CAUSE_PARAMETER_CHANGE_IN_EXECUTION:
                {
                strcpy (str, "PARAMETER CHANGE IN EXECUTION");
                }
                break;
            case ADD_CAUSE_STEP_LIMIT:
                {
                strcpy (str, "STEP LIMIT");
                }
                break;
            case ADD_CAUSE_BLOCKED_BY_MODE:
                {
                strcpy (str, "BLOCKED BY MODE");
                }
                break;
            case ADD_CAUSE_BLOCKED_BY_PROCESS:
                {
                strcpy (str, "BLOCKED BY PROCESS");
                }
                break;
            case ADD_CAUSE_BLOCKED_BY_INTERLOCKING:
                {
                strcpy (str, "BLOCKED BY INTERLOCKING");
                }
                break;
            case ADD_CAUSE_BLOCKED_BY_SYNCHROCHECK:
                {
                strcpy (str, "BLOCKED BY SYNCHROCHECK");
                }
                break;
            case ADD_CAUSE_COMMAND_ALREADY_IN_EXECUTION:
                {
                strcpy (str, "COMMAND ALREADY IN EXECUTION");
                }
                break;
            case ADD_CAUSE_BLOCKED_BY_HEALTH:
                {
                strcpy (str, "BLOCKED BY HEALTH");
                }
            case ADD_CAUSE_1_OF_N_CONTROL:
                {
                strcpy (str, "1 OF N CONTROL");
                }
                break;
            case ADD_CAUSE_ABORTION_BY_CANCEL:
                {
                strcpy (str, "ABORTION BY CANCEL");
                }
                break;
            case ADD_CAUSE_TIME_LIMIT_OVER:
                {
                strcpy (str, "TIME LIMIT OVER");
                }
                break;
            case ADD_CAUSE_ABORTION_BY_TRIP:
                {
                strcpy (str, "ABORTION BY TRIP");
                }
                break;
            case ADD_CAUSE_OBJECT_NOT_SELECTED:
                {
                strcpy (str, "OBJECT NOT SELECTED");
                }
                break;
            case ADD_CAUSE_OBJECT_ALREADY_SELECTED:
                {
                strcpy (str, "OBJECT ALREADY SELECTED");
                }
                break;
            case ADD_CAUSE_NO_ACCESS_AUTHORITY:
                {
                strcpy (str, "NO ACCESS AUTHORITY");
                }
                break;
            case ADD_CAUSE_ENDED_WITH_OVERSHOOT:
                {
                strcpy (str, "ENDED WITH OVERSHOOT");
                }
                break;
            case ADD_CAUSE_ABORTION_DUE_TO_DEVIATION:
                {
                strcpy (str, "ABORTION DUE TO DEVIATION");
                }
                break;
            case ADD_CAUSE_ABORTION_BY_COMMUNICATION_LOSS:
                {
                strcpy (str, "ABORTION BY COMMUNICATION LOSS");
                }
                break;
            case ADD_CAUSE_BLOCKED_BY_COMMAND:
                {
                strcpy (str, "BLOCKED BY COMMAND");
                }
                break;
            case ADD_CAUSE_NONE:
                {
                strcpy (str, "NONE");
                }
                break;
            case ADD_CAUSE_INCONSISTENT_PARAMETERS:
                {
                strcpy (str, "INCONSISTENT PARAMETERS");
                }
                break;
            case ADD_CAUSE_LOCKED_BY_OTHER_CLIENT:
                {
                strcpy (str, "LOCKED BY OTHER CLIENT");
                }
                break;
            default:
                {
                strcpy (str, "");
                }
                break;
        }
    }
   
/************************************************************************/
/*                      datamap_cfg_var_cleanup                         */
/* Clean up data mapping info in this var.                              */
/************************************************************************/

static ST_VOID datamap_cfg_var_cleanup (MVL_VAR_ASSOC *var)
  {
  DATA_MAP_HEAD *map_head_sorted;
  ST_UINT j;
  DATA_MAP *map_entry;    /* current array entry  */

  /* var->user_info points to DATA_MAP_HEAD struct      */
  map_head_sorted = var->user_info;
  if (map_head_sorted)
    {
    for (j = 0; j < map_head_sorted->map_count; j++)
      { /* map_arr has many NULL entries for non-primitive elements*/
      map_entry = map_head_sorted->map_arr [j];
      if (map_entry != NULL)
        {
        if (map_entry->deadband_info)
          chk_free (map_entry->deadband_info);
        chk_free (map_entry);
        }
      }
    chk_free (map_head_sorted);
    var->user_info = NULL;
    }
  }
 
/************************************************************************/
/*                      datamap_cfg_destroy                             */
/* Unmap all data mapped by datamap_cfg_read and free associated buffers.*/
/* CRITITAL: must be called BEFORE variables removed, or it will be     */
/*   impossible to remove the mapping.                                  */
/************************************************************************/
ST_VOID datamap_cfg_destroy ()
  {
  ST_INT didx, vidx;      /* indices to domain & variable arrays  */
  MVL_DOM_CTRL *dom;
  MVL_VAR_ASSOC *var;

  for (didx = 0; didx < mvl_vmd.num_dom; didx++)
    {
    dom = mvl_vmd.dom_tbl [didx];
    for (vidx = 0; vidx < dom->num_var_assoc; vidx++)
      {
      var = dom->var_assoc_tbl[vidx];
      /* Clean up data mapping info in this var.        */
      datamap_cfg_var_cleanup (var);
      }
    }
  }

/************************************************************************/
/*                      all_obj_destroy_scd                             */
/* Destroy all objects created by all_obj_create.                       */
/************************************************************************/
ST_RET all_obj_destroy_scd (MVL_VMD_CTRL **vmd_ctrl_arr, ST_UINT vmd_count)
  {
  ST_RET ret = SD_SUCCESS;
  ST_UINT j;
  /* CRITICAL: datamap_cfg_destroy must be called BEFORE mvl_vmd_destroy*/
  /*           if datamap_cfg_read was called at startup.               */
#if 1
  datamap_cfg_destroy ();
#endif
  mvl61850_rpt_ctrl_destroy_all ();     /* destroy rpt_ctrls before VMD */
  mvl61850_log_ctrl_destroy_all ();     /* destroy log_ctrls before VMD */

  /* Destroy ALL VMDs.  */
  /* NOTE: One of these VMDs may be the global VMD "mvl_vmd".           */
  for (j = 0; j < vmd_count; j++)
    {
    ret = mvl_vmd_destroy (vmd_ctrl_arr[j]);
    if (ret)
      {
      printf ("Error destroying VMD %p", vmd_ctrl_arr[j]);
      }
    }
  chk_free (vmd_ctrl_arr);
  return (ret);
  }
