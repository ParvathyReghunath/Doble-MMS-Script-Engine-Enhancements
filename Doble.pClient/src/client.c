/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,       */
/*              1986 - 2018, All Rights Reserved                        */
/*                                                                      */
/* MODULE NAME : client.c                                               */
/* PRODUCT(S)  : MMSEASE-LITE                                           */
/*                                                                      */
/* MODULE DESCRIPTION :                                                 */
/*                                                                      */
/*   This module shows an example of client doing identify, read and    */
/* write with a server.                                                 */
/*                                                                      */
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :                            */
/*      NONE                                                            */
/*                                                                      */
/* MODIFICATION LOG :                                                   */
/*  Date     Who   Rev                  Comments                        */
/* --------  ---  ------   -------------------------------------------  */
/* 04/12/18  JRB  For SNAP_LITE_THR, use fixed serverName="remote1"     */
/*                (using first name in siscostackcfg.xml was confusing).*/
/* 03/08/18  JRB  Del obsolete "u_mvl61850_ctl_oper_*" functions.        */
/* 01/08/18  JRB  test_iec_remote_vmd: add call to new nvl_write.       */
/* 05/05/17  JRB  Decode CommandTermination or LastApplError report.    */
/* 04/27/17  JRB  test_iec_remote_vmd: add loop calling nvl_read.       */
/* 04/14/17  JRB  Del file functions (use new u_ifile.c).               */
/* 02/17/17  RKR  Add an argument to send Identifies, rekey testing     */
/* 01/20/17  JRB  Add IPv6 support: make 2nd IPv6 connection if configured.*/
/*                Add runTests function and call for each connection.   */
/*                Specify SCL, IED, AP on cmd line for "-m iecremote" option.*/
/*                mvla_fdir: chg fs_name to "*" for 61850-8-1 Tissue 1036.*/
/*                scl2_datatype_create_all: chg arg to 0 to support MHAI.*/
/*                Print warning if scl_parse returns SX_ERR_CONVERT, but continue.*/
/*                Del obsolete cmd line option "-m ucarpt".             */
/* 01/18/17  JRB  If S_SEC_ENABLED, check for revoked, expired certificates.*/
/* 07/15/16  JRB  Add USE_TYPE_PTR option.                              */
/* 01/07/14  JRB  Demonstrate new asynchronous read/write functions.    */
/*                named_var_read/named_var_write: allocate req_info so  */
/*                data alignment is correct on all platforms.           */
/*                Close files when file errors detected.                */
/* 07/28/14  JRB  For WIN32, don't call sscSetCfgFilePath.              */
/* 06/27/14  EJV  Ported for MMS-LITE-SECURE (defining SNAP_LITE_THR).  */
/*                Added code to avoid endless loop if wrong arg passed. */
/*                Del ulCheckSecurityConfiguration, secManCfgChange.    */
/* 03/02/12  JRB           test_iec_control: fix oper_data_buf size.    */
/* 02/17/12  JRB           Add USE_DIB_LIST option.                     */
/*                         Move scl_info to main for more flexibility.  */
/*                         Assume all reports are IEC 61850 reports.    */
/*                         Use strncat_maxstrlen.                       */
/* 04/29/11  JRB           Change to new SCL file for iecremote option. */
/*                         Check for remote_vmd_ctrl == NULL.           */
/* 03/31/11  JRB           Add mvl_free_mms_objs. Call it and call      */
/*                         osicfgx_unconf* & logcfgx_unconf* before exit.*/
/* 03/25/11  JRB           u_set_all_leaf..: add arg.                   */
/* 06/23/10  JRB           Call subnet_serve (NEW), clnp_init, clnp_end.*/
/*                         Del call to obsolete clnp_snet_read_hook_add */
/*                         and related code (handled by new subnet_serve).*/
/*                         Use new defines GOOSE_RX_SUPP, GSSE_RX_SUPP, */
/*                         GSE_MGMT_RX_SUPP, SMPVAL_RX_SUPP, SUBNET_THREADS*/
/*                         Del #ifdef MOSI.                             */
/* 11/17/09  JRB     66    mvla_initiate_req_ex: add arg clientARName.  */
/*                         Fix HARD_CODED_CFG for new DIB_ENTRY.        */
/*                         Del MAP30_ACSE support.                      */
/*                         Del refs to secManCfgXmlFile (obsolete).     */
/*                         Add casts on some printfs.                   */
/* 06/04/08  JRB     65    Retry connectToServer.                       */
/*                         connectToServer: abort on timeout (critical).*/
/*                         disconnectFromServer: return result.         */
/*                         Add code to test mvla_fget.                  */
/* 05/14/08  GLB     64    Added slog_ipc_std_cmd_service               */
/* 10/15/07  JRB     63    Add rcb to linked list BEFORE rcb_enable.    */
/* 12/04/06  JRB     62    Fix ..vmd_destroy calls.                     */
/* 11/21/06  JRB     61    Chk mode before destroying remote_vmd_ctrl.  */
/* 11/07/06  JRB     60    Add command line option "iecremote" to test  */
/*                         configuring with SCL.                        */
/*                         Add '01' suffix in PosReport, MeaReport.     */
/* 06/23/06  MDE     59    Added time_t cast                            */
/* 03/20/06  JRB     58    Add code to receive 61850 Sampled Value msgs.*/
/*                         Set up callback funct clnp_snet_etype_process*/
/*                         to process all received Ethertype packets.   */
/* 12/14/05  EJV     57    init_log_cfg: have fun proto for all options.*/
/* 10/25/05  EJV     56    S_SEC_ENABLED: added identify_response_info. */
/*                         Call slog_start, slog_end, slogIpcEvent.     */
/*                         init_mem: del dupl mvl_init_glb_vars call.   */
/* 09/09/05  JRB     55    Don't print BUILD_NUM.                       */
/* 08/05/05  EJV     54    Added ssleLogMaskMapCtrl.                    */
/* 07/22/05  JRB     53    Allow one conn to control multiple RCBs      */
/*                         (user_info must point to ALL_RCB_INFO).      */
/*                         Del test_uca_rpt, use test_iec_rpt for UCA too.*/
/*                         Exit if 'x' key is hit.                      */
/* 07/15/05  JRB     52    Add test_iec_control.                        */
/*                         Add #ifdef MVL_GOOSE_SUPPORT.                */
/*                         test_iec_rpt: pass dom & RCB name as args.   */
/*                         Fix mvla_status call.                        */
/*                         Ignore rpts received containing NVL.         */
/* 06/07/05  EJV     51    Set gsLogMaskMapCtrl if S_MT_SUPPORT defined */
/* 06/06/05  EJV     50    gsLogMaskMapCtrl not on QNX.                 */
/* 05/24/05  EJV     49    Added logCfgAddMaskGroup (&xxxLogMaskMapCtrl)*/
/*                         Use logcfgx_ex to parse logging config file. */
/* 05/20/05  JRB     48    Don't print "Check log file". May not be one.*/
/* 05/20/05  JRB     47    Fix keyboard input: Use getch on WIN32, QNX. */
/*                         Use term_init, term_rest, getchar elsewhere. */
/* 08/10/04  EJV     46    secManCfgXmlFile: all lowcase "secmancfg.xml"*/
/*                         u_mvl_connect_cnf(ind)_ex: chg/typecasted ret*/
/* 08/06/04  ASK     45    Change path to secManCfg.xml for !WIN32      */
/* 01/22/04  EJV     44    waitReqDone: replaced time() w/ sGetMsTime() */
/*                         Don't call ulFreeAssocSecurity on disconnect.*/
/*                         named_var_write: added chk_free()            */
/*                         Deleted mvl_local_cl_ar_name.                */
/* 01/06/04  EJV     43    Added security support (S_SEC_ENABLED code). */
/*                         Added LITESECURE version printf.             */
/* 01/09/03  JRB     42    Fix connectToServer return value on errors.  */
/* 12/17/03  JRB     41    61850-8-1 FDIS changes:                      */
/*                         Chg OptFlds from bvstring9 to bvstring10.    */
/*                         Set OPTFLD_BITNUM_CONFREV bit in OptFlds.    */
/* 12/15/03  JRB     40    Clear MVL_READ_RESP_PARSE_INFO before each read*/
/* 12/11/03  JRB     39    DON'T use MVLLOG_REQ/RESP/IND/CONF, they     */
/*                         don't exist.                                 */
/* 11/17/03  JRB     38    Test new IEC rpt code: Call rpt_typeids_find,*/
/*                         rcb_info_create/destroy, u_iec_rpt_ind.      */
/*                         Include client.h. Make waitReqDone global.   */
/*                         Use new named_var_read, named_var_write.     */
/*                         Add '-m' cmd line arg.                       */
/*                         Pass (MVL_NET_INFO *) to functions.          */
/* 09/30/03  EJV     37    Repl connect_ind_fun w/u_mvl_connect_ind_ex  */
/* 07/17/03  EJV     36    Added param to mvla_initiate_req_ex          */
/* 04/23/03  JRB     35    Read sample UTF8Vstring var (UTF8Vstring13test).*/
/* 04/14/03  JRB     34    Chg main return from void to "int".          */
/*                         Eliminate compiler warnings.                 */
/* 10/17/02  ASK     33    Added ACSE authentication sample code.       */
/* 03/06/02  JRB     32    Del ADLC code.                               */
/* 02/25/02  MDE     31    Now get max PDU size from mvl_cfg_info       */
/* 02/25/02  MDE     30    MAP30_ACSE cleanup                           */
/* 02/12/02  JRB     29    Fix QNX warning.                             */
/* 02/08/02  JRB     28    Pass MVL_CFG.. to osicfgx & mvl_start_acse,  */
/*                         del mvl_init_mms.. call & mvl_local_ar.. var.*/
/* 01/24/02  EJV     27    Added sample for DOM_SPEC journals           */
/* 11/06/01  JRB     26    Replace "synchronous" client calls with      */
/*                         calls to "async" functs, then waitReqDone.   */
/*                         Del _WIN32 Sleep in doCommService.           */
/* 10/09/01  JRB     25    Use new MMSLITE_* defines for product ID.    */
/* 09/21/01  JRB     24    Chg serverName to match sample cfg files.    */
/* 05/25/01  RKR     23    Updated version to 4.01P12                   */
/* 01/04/01  EJV     22    Removed "conio.h"                            */
/* 01/05/00  MDE     21    Added MAP30_ACSE code                        */
/* 11/27/00  JRB     20    Use old CFG files for Reduced Stack & Trim-7.*/
/* 11/07/00  GLB     19    Added XML configuration files                */
/* 07/17/00  RKR     18    Updated version to 4.01P6                    */
/* 03/13/00  JRB     17    Print all errors in Hex.                     */
/* 12/15/99  GLB     16    Print informative message when program exits */
/* 10/15/99  GLB     15    Added file Rename and Obtain FIle            */
/* 03/19/99  RKR     14    Changed client & server to local1 & remote1  */
/* 03/03/99  MDE     13    Removed reference to local_data_size         */
/* 11/16/98  MDE     12    Cleanup, use new conclude, add mem_chk stuff */
/* 11/13/98  JRB     11    Wait for event before calling mvl_comm_serve */
/*                         if stack is MOSI (i.e. osi???e.lib used).    */
/* 10/13/98  JRB     10    Use MVL_JOURNAL_ENTRY, just like server.     */
/*                         Call read_log_cfg_file to set up logging.    */
/* 10/12/98  MDE     09    Added GetVar, GetDom, NameList services      */
/* 09/21/98  MDE     08    Minor lint cleanup                           */
/* 08/12/98  MDE     07    Don't include mvl_defs.h (in mvl_acse.h)     */
/* 07/16/98  MDE     06    Changed frsmid to ST_INT32                   */
/* 06/30/98  MDE     05    Use new Client Info Rpt code                 */
/* 05/22/98  JRB     04    Use new "Identify" request interface.        */
/* 05/15/98  JRB     03    Added several new MMS Request tests.         */
/*                         Reset doIt before disconnect so it finishes. */
/*                         Add u_mvl_ident_ind. Del mvl_ident_resp_info.*/
/* 12/29/97  MDE     02    Added ^c handler, removed keyboard stuff     */
/* 09/10/97  MDE     01    MMS-LITE V4.0 Release                        */
/************************************************************************/

#if 0
/* move outside #if to enable */
#define HARD_CODED_CFG
#define ACSE_AUTH_ENABLED     /* for password authentication without  */
/* security                             */
/*  S_SEC_ENABLED               for certificate based authentication &  */
/*                              SSL encryption, put this define in      */
/*                              application's DSP/MAK file              */
#endif



/************************************************************************/
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

MVL_VMD_CTRL * config_iec_remote_vmd (
    ST_CHAR * scl_filename,
    ST_CHAR * ied_name,
    ST_CHAR * access_point_name,
    SCL_INFO * remote_scl_info);   /* filled in by this function   */
ST_VOID log_iec_remote_vmd_var_names (MVL_VMD_CTRL* vmd_ctrl);
ST_VOID test_iec_remote_vmd (
    MVL_NET_INFO* net_info,
    MVL_VMD_CTRL* vmd_ctrl);
static ST_VOID runTests (MVL_NET_INFO* clientNetInfo,
                         ST_INT test_mode,
                         RPT_TYPEIDS* rpt_typeids,
                         MVL_VMD_CTRL* remote_vmd_ctrl);
static ST_VOID  sendIdentify (MVL_NET_INFO* clientNetInfo,
                              ST_INT identifyIterations);

#if defined(MVL_GOOSE_SUPPORT)
/* Need parts of "iecgoose" sample app  */
#include "iec_demo.h"   /* definitions from "iecgoose" sample app       */
/* Prototype here because it needs struct def from "iec_demo.h".        */
IEC_GOOSE_RX_USER_INFO* goose_init (ST_VOID);
#endif

/*----------------------------------------------------------------------*/
/* NOTE: The S_SEC_ENABLED delimits the security related code.          */
/*----------------------------------------------------------------------*/
#if defined(S_SEC_ENABLED) && defined(ACSE_AUTH_ENABLED)
#error Only one S_SEC_ENABLED or ACSE_AUTH_ENABLED may be defined
#endif

#if defined(S_SEC_ENABLED)
#include "mmslusec.h"

#if defined(SNAP_LITE_THR)
#include "snap_l.h"
#include "snaplog.h"
#endif  /* defined(SNAP_LITE_THR) */
#endif  /* defined(S_SEC_ENABLED) */

#include "fkeydefs.h"
#ifdef kbhit    /* CRITICAL: fkeydefs may redefine kbhit. DO NOT want that.*/
#undef kbhit
#endif

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of      */
/* __FILE__ strings.                                                    */
/************************************************************************/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR* SD_CONST thisFileName = __FILE__;
#endif

/************************************************************************/
/* Identify server information                                          */

IDENT_RESP_INFO identify_response_info =
    {
    "SISCO",              /* Vendor       */
    MMSLITE_NAME,         /* Model        */
    MMSLITE_VERSION,  0,  /* Version      */
    };


/************************************************************************/
void ctrlCfun (int);
static ST_VOID kbService (ST_VOID);

MVL_NET_INFO* connectToServer (ST_CHAR* clientARName, ST_CHAR* serverARName);

ST_RET disconnectFromServer (MVL_NET_INFO* clientNetInfo);

static ST_VOID disc_ind_fun (MVL_NET_INFO* cc, ST_INT discType);

static ST_RET getFile (MVL_NET_INFO* clientNetInfo, ST_CHAR* loc_file, ST_CHAR* rem_file);

ST_VOID test_simple_requests (MVL_NET_INFO* clientNetInfo);
RCB_INFO* test_iec_rpt (MVL_NET_INFO* clientNetInfo, RPT_TYPEIDS* rpt_typeids,
                        ST_CHAR* dom_name,  /* domain which contains the RCB*/
                        ST_CHAR* rcb_name); /* RCB (e.g. "LLN0$BR$PosReport")*/

ST_INT timeOut = 60;    /* timeout (in seconds) passed to waitReqDone   */
time_t startTime;

ST_BOOLEAN doIt = SD_TRUE;


/************************************************************************/
#if defined(HARD_CODED_CFG)

DIB_ENTRY localDibTable [] =
    {
      {
      NULL,       /* next (Linked List ptr)*/     /* NOTE: added for new DIB_ENTRY*/
      NULL,       /* prev (Linked List ptr)*/     /* NOTE: added for new DIB_ENTRY*/
      0,          /* reserved             */
      "local1",   /* name[65]             */
      SD_TRUE,    /* local                */
        {                /* ae_title             */      /* NOTE: added for new DIB_ENTRY*/
        SD_TRUE,  /* AP_title_pres        */
          {       /* AP_title             */
          4,      /* num_comps            */
          1, 3, 9999, 105
          },
        SD_FALSE, /* AP_inv_id_pres       */
        0,        /* AP_invoke_id         */
        SD_TRUE,  /* AE_qual_pres         */
        33,       /* AE_qual              */
        SD_FALSE, /* AE_inv_id_pres       */
        0,        /* AE_invoke_id         */
        },        /* end ae_title         */      /* NOTE: added for new DIB_ENTRY*/

        {                              /* PRES_ADDR                            */
        4,                        /* P-selector length                    */
        "\x00\x00\x00\x01",       /* P-selector                           */
        2,                        /* S-selector length                    */
        "\x00\x01",               /* S-selector                           */
        TP_TYPE_TP4,              /* Transport type                       */
        2,                        /* T-selector length                    */
        "\x00\x01",               /* T-selector                           */
        0,                        /* Network address length               */
        ""                        /* Network address (ignored on local)   */
        }
      }
    };

DIB_ENTRY remoteDibTable [] =
    {
      {
      NULL,       /* next (Linked List ptr)*/     /* NOTE: added for new DIB_ENTRY*/
      NULL,       /* prev (Linked List ptr)*/     /* NOTE: added for new DIB_ENTRY*/
      0,          /* reserved             */
      "remote1",  /* name[65]             */
      SD_TRUE,    /* local                */
        {                /* ae_title             */      /* NOTE: added for new DIB_ENTRY*/
        SD_TRUE,  /* AP_title_pres        */
          {       /* AP_title             */
          4,      /* num_comps            */
          1, 3, 9999, 106
          },
        SD_FALSE, /* AP_inv_id_pres       */
        0,        /* AP_invoke_id         */
        SD_TRUE,  /* AE_qual_pres         */
        33,       /* AE_qual              */
        SD_FALSE, /* AE_inv_id_pres       */
        0,        /* AE_invoke_id         */
        },        /* end ae_title         */      /* NOTE: added for new DIB_ENTRY*/

        {                              /* PRES_ADDR                            */
        4,                        /* P-selector length                    */
        "\x00\x00\x00\x01",       /* P-selector                           */
        2,                        /* S-selector length                    */
        "\x00\x01",               /* S-selector                           */
        TP_TYPE_TP4,              /* Transport type                       */
        2,                        /* T-selector length                    */
        "\x00\x01",               /* T-selector                           */
        4,                        /* Network address length               */
        "\x49\x00\x11\x11"        /* Network address                      */
        }
      }
    };

#endif  /* defined(HARD_CODED_CFG)      */

ST_VOID init_mem (ST_VOID);
ST_VOID init_log_cfg (ST_VOID);


/************************************************************************/
/* Global variables                                                     */
/************************************************************************/
typedef struct
    {
    ST_INT cmd_term_num_va;       /* num of vars received in CommandTermination*/
    ST_CHAR oper_name[MAX_IDENT_LEN + 1];
    MVL61850_LAST_APPL_ERROR LastApplError; /* for decoded LastApplError  */
    } MY_CONTROL_INFO;
/* NOTE: this global variable can be avoided if a (MVL_CONTROL_INFO *)  */
/*   is saved in the "user_info" member of MVL_NET_INFO, but to do so,  */
/*   you must make sure "user_info" is not used for something else.     */
MY_CONTROL_INFO my_control_info;
MVL_TYPE_ID LastApplErrorTypeId;  /* type for decoding LastApplError    */

/************************************************************************/
/*                      mvl_free_mms_objs                               */
/* Free objects allocated by "mvl_init_mms_objs".                       */
/* CAUTION: This function only works for this sample. If objects are    */
/*          allocated and initialized by Foundry, as in other samples,  */
/*          it is very difficult to free them.                          */
/************************************************************************/
ST_VOID mvl_free_mms_objs (ST_VOID)
    {
    /* Free buffers allocated at startup by "mvl_init_mms_objs".  */
    if (mvl_vmd.var_assoc_tbl)
        M_FREE (MSMEM_STARTUP, mvl_vmd.var_assoc_tbl);
    if (mvl_vmd.dom_tbl)
        M_FREE (MSMEM_STARTUP, mvl_vmd.dom_tbl);
    if (mvl_vmd.nvlist_tbl)
        M_FREE (MSMEM_STARTUP, mvl_vmd.nvlist_tbl);
    if (mvl_vmd.jou_tbl)
        M_FREE (MSMEM_STARTUP, mvl_vmd.jou_tbl);
    if (mvl_type_ctrl)
        M_FREE (MSMEM_STARTUP, mvl_type_ctrl);
    }

/************************************************************************/
/*                      domvar_type_id_create                           */
/* Get the type for any domain-specific var and create a type_id.       */
/************************************************************************/
MVL_TYPE_ID domvar_type_id_create (MVL_NET_INFO* clientNetInfo, ST_CHAR* dom_name, ST_CHAR* var_name)
    {
    MVL_REQ_PEND* reqCtrl;

    GETVAR_REQ_INFO getvar_req;
    MVL_TYPE_ID oper_type_id = MVL_TYPE_ID_INVALID;       /* start with invalid type id   */
    ST_RET ret;

    /* Get the type of this "Oper" attribute & create type.       */
    /* Would be more efficient to do this just once before this function.*/
    getvar_req.req_tag = GETVAR_NAME;
    getvar_req.name.object_tag = DOM_SPEC;        /* always DOM_SPEC for 61850 variables*/
    getvar_req.name.obj_name.vmd_spec = var_name;
    getvar_req.name.domain_id = dom_name;

    ret = mvla_getvar (clientNetInfo, &getvar_req, &reqCtrl);
    if (ret == SD_SUCCESS)
        ret = waitReqDone (reqCtrl, timeOut);
    if (ret != SD_SUCCESS)
        printf ("Error getting type of variable '%s' in domain '%s'", var_name, dom_name);
    else
        {
        /* Don't care about name so pass NULL.                      */
        oper_type_id = mvl_type_id_create (NULL, reqCtrl->u.getvar.resp_info->type_spec.data,
                                           reqCtrl->u.getvar.resp_info->type_spec.len);
        }
    mvl_free_req_ctrl (reqCtrl);  /* Done with request struct     */
    return (oper_type_id);
    }

/************************************************************************/
/*                      test_iec_control                                */
/* Save control info in global variable (my_control_info). It is needed */
/* later when u_mvl_info_rpt_ind is called (i.e. rpt received).         */
/* See earlier note about how avoid using global my_control_info.       */
/************************************************************************/
ST_VOID test_iec_control (MVL_NET_INFO* clientNetInfo, RPT_TYPEIDS* rpt_typeids,
                          ST_CHAR* dom_name,
                          ST_CHAR* sbo_name,
                          ST_CHAR* oper_name,
                          MVL_TYPE_ID oper_type_id)  /* type_id for "oper_name" arg  */
    {
    ST_RET ret;
    ST_INT j;
    ST_CHAR oper_ref[66];  /* Reference returned when SBO read     */
    ST_CHAR* oper_data_buf; /* "Oper" data. Get size from RUNTIME_TYPE & alloc*/
    RUNTIME_TYPE* rt_first; /* first type in array          */
    RUNTIME_TYPE* rt_type;  /* current type being processed */
    ST_INT rt_num;          /* num of types in array        */

    printf ("\nTesting '%s'.\n", oper_name);

    oper_data_buf = NULL; /* initialize. Should be allocated below.       */

    /* Read "SBO" */
    /* Type should really be vstring64, but using vstring65 is OK.        */
    ret = named_var_read (clientNetInfo, sbo_name, DOM_SPEC, dom_name,
                          rpt_typeids->vstring65, oper_ref, timeOut);
    if (ret)
        printf ("SBO Read ERROR");
    else
        printf ("SBO read SUCCESS = %s\n", oper_ref);

    /* FOR DEMONSTRATION ONLY: find "ctlVal" & "operTm" components        */
    /* of "Oper" and write dummy data to them.                            */
    ret = mvl_get_runtime (oper_type_id, &rt_first, &rt_num);
    if (ret == SD_SUCCESS)
        {
        ST_INT data_offset = 0;     /* use to compute offset to components  */
        /* Allocate "Oper" data buffer. Loop through type to init data.*/
        oper_data_buf = chk_calloc (1, rt_first->offset_to_last);
        for (j = 0; j < rt_num; j++)
            {
            rt_type = &rt_first[j];
            if (strcmp (ms_comp_name_find (rt_type), "ctlVal") == 0)
                {
                if (rt_type->el_tag == RT_INTEGER && rt_type->u.p.el_len == 4)
                    * (ST_INT32*)(oper_data_buf + data_offset) = 9;
                }
            else if (strcmp (ms_comp_name_find (rt_type), "operTm") == 0)
                {
                if (rt_type->el_tag == RT_UTC_TIME)
                    {
                    MMS_UTC_TIME* utc_time;
                    utc_time = (MMS_UTC_TIME*)(oper_data_buf + data_offset);
                    utc_time->secs = (ST_UINT32)time (NULL);
                    utc_time->fraction = 99;
                    utc_time->qflags = 0xFf;
                    }
                }
            data_offset += rt_type->el_size;
            assert (data_offset <= rt_first->offset_to_last);
            } /* end "for"    */
        }
    else
        printf ("Error getting RUNTIME_TYPE info. Dummy values not written to 'Oper'.\n");

    /* NOTE: -1 indicates "CommandTermination" not received yet.          */
    /*  This must be set before "named_var_write" because                 */
    /*  CommandTermination may be received before it returns.             */

    my_control_info.cmd_term_num_va = -1;
    strcpy (my_control_info.oper_name, oper_name);        /* save oper_name for later*/

    /* NOTE: if this is SBO control and SBO read failed, this write       */
    /*   to "Oper" should fail, but try it anyway.                        */
    ret = named_var_write (clientNetInfo, oper_name, DOM_SPEC, dom_name,  /* no domain name*/
                           oper_type_id, oper_data_buf, timeOut);
    if (ret != SD_SUCCESS)
        {
        printf ("Control Activation Response received: FAILURE\n");
        /* If LastApplError was set by a Report, display its data. */
        if (my_control_info.LastApplError.CntrlObj[0] != 0)
            {
            printf ("LastApplError data:\n");
            printf ("CntrlObj = %s\n", my_control_info.LastApplError.CntrlObj);
            printf ("Error    = %d\n", (ST_INT)my_control_info.LastApplError.Error);
            printf ("orCat    = %d\n", (ST_INT)my_control_info.LastApplError.Origin.orCat);
            /* orIdent is Octet string (harder to display so skip it for now). */
            printf ("ctlNum   = %u\n", (ST_UINT)my_control_info.LastApplError.ctlNum);
            printf ("AddCause = %d\n", (ST_INT)my_control_info.LastApplError.AddCause);
            }
        }
    else
        printf ("Control Activation Response received: SUCCESS\n");

    /* if buffer was allocated, free it.  */
    if (oper_data_buf)
        chk_free (oper_data_buf);

    /* Wait here for CommandTermination info rpt before returning to main loop.*/
    if (ret == SD_SUCCESS)
        {
        time_t timeout = time (NULL) + 5;    /* DEBUG: chg timeout as needed.*/

        while (doIt)
            {
            doCommService ();
            if (time (NULL) > timeout)
                {
                printf ("Timeout waiting for CommandTermination\n");
                break;
                }
            /* .._num_va is set by u_mvl_info_rpt_ind when report received.   */
            if (my_control_info.cmd_term_num_va >= 0)
                {
                /* If 2 vars in rpt, must be error. If 1 var, must be OK.       */
                printf ("Command Termination received: %s\n",
                        my_control_info.cmd_term_num_va == 1 ? "SUCCESS" : "FAILURE");
                if (my_control_info.cmd_term_num_va == 2)
                    {
                    /* LastApplError should contain error details.  */
                    printf ("LastApplError data:\n");
                    printf ("CntrlObj = %s\n", my_control_info.LastApplError.CntrlObj);
                    printf ("Error    = %d\n", (ST_INT)my_control_info.LastApplError.Error);
                    printf ("orCat    = %d\n", (ST_INT)my_control_info.LastApplError.Origin.orCat);
                    /* orIdent is Octet string (harder to display so skip it for now). */
                    printf ("ctlNum   = %u\n", (ST_UINT)my_control_info.LastApplError.ctlNum);
                    printf ("AddCause = %d\n", (ST_INT)my_control_info.LastApplError.AddCause);
                    }
                break;
                }
            }
        }
    return;
    }


#if defined(SNAP_LITE_THR)
/************************************************************************/
/*                           setServerName                              */
/* This function will set serverName to FIRST Remote AR Name configured */
/* in the siscostackcfg.xml for the "MMS-LITE Client" application.      */
/************************************************************************/
ST_RET setServerName (ST_CHAR* serverName, ST_UINT maxLen)
    {
    ST_RET rtn = SD_FAILURE;
    SSC_STACK_CFG* sscStackCfg;
    SSC_APP_AR_NAME* arname;        /* current entry being processed      */

    sscStackCfg = sscAccessStackCfg ();   /* protect access to global     */

    /* NOTE: This may be NULL.  */
    if (sscStackCfg)
        {
        /* Loop through AR Names to find remote Server.   */
        for (arname = sscStackCfg->appNetwork->appArNameList;
             arname != NULL;
             arname = (SSC_APP_AR_NAME*)list_get_next (sscStackCfg->appNetwork->appArNameList, arname))
            {
            if (arname->role == SSC_AR_NAME_ROLE_REMOTE)
                {
                /* use first Remote AR Name foundfor ServerName */
                strncpy_safe (serverName, arname->arName, maxLen - 1);
                rtn = SD_SUCCESS;
                break;
                }
            }
        }

    sscReleaseStackCfg ();                /* protect access to global     */
    return (rtn);
    }
#endif /* defined(SNAP_LITE_THR) */

/************************************************************************/
/*                       main                                           */
/************************************************************************/

int main_sisco (int argc, char* argv [])
    {
#define TEST_MODE_SIMPLE        0
#define TEST_MODE_IEC_RPT       1
#define TEST_MODE_IEC_CONTROL   3
#define TEST_MODE_IEC_REMOTE    4
#define TEST_MODE_IDENTIFY      5
    ST_INT test_mode = TEST_MODE_SIMPLE;    /* test mode. default to simple test    */
    ST_RET ret;
    MVL_CFG_INFO mvlCfg;
    /* NOTE: serverName gets overwritten if USE_DIB_LIST or SISCO_STACK_CFG is defined.   */
    /* These should be big enough for maximum name generated from SCL.  */
    ST_CHAR serverName[MAX_AR_LEN + 1] = "remote1";
    ST_CHAR serverNameIpv6[MAX_AR_LEN + 1] = "remote1/IPV6";
    MVL_NET_INFO* clientNetInfo;
    MVL_NET_INFO* clientNetInfoIpv6 = NULL; /* 2nd connection using IPv6, if configured.*/
    RPT_TYPEIDS rpt_typeids;
    ST_CHAR* usageString = "usage: %s [-m test_mode]\n  -m test_mode\t Testing mode (iecrpt, ieccontrol, or iecremote)\n"
        "  For IEC 61850 Reporting test:\n"
        "    -m iecrpt\n"
        "  For IEC 61850 Control test:\n"
        "    -m ieccontrol\n"
        "  For IEC 61850 Remote test (gets data model and address from SCL):\n"
        "    -m iecremote sclFileName iedName apName\n";
    ST_INT j;
#if defined(MVL_GOOSE_SUPPORT)
    IEC_GOOSE_RX_USER_INFO* goosehandle = NULL;
    ST_DOUBLE TimeOld;
    ST_DOUBLE TimeNew;
    ST_LONG   TimeElapsed;
#endif
    ALL_RCB_INFO* all_rcb_info;     /* used to track all RCBs on one conn   */
    RCB_INFO* rcb_info;
    MVL_VMD_CTRL* remote_vmd_ctrl = NULL; /* this is set only for "iecremote" option*/
    SCL_INFO remote_scl_info;       /* filled in by config_iec_remote_vmd   */
    ST_CHAR* sclFileName;   /* for TEST_MODE_IEC_REMOTE */
    ST_CHAR* iedName;       /* for TEST_MODE_IEC_REMOTE */
    ST_CHAR* apName;        /* for TEST_MODE_IEC_REMOTE */
    ST_INT  identifyIterations = 0;

#if defined(SNAP_LITE_THR)
    ST_THREAD_HANDLE thSnapThr;
    ST_THREAD_ID     tidSnapThr;
#endif

#if defined(S_SEC_ENABLED)
    ST_DOUBLE lastCertCheckTime = 0;
#endif

#if defined(SISCO_STACK_CFG)
    /* "appName" must match an App_Name element in "siscostackcfg.xml".*/
    ST_CHAR* appName = "app_client";  /* case sensitive */
#endif

  /* Process command line arguments.    */
    for (j = 1; j < argc; )
        {
        if (strcmp (argv[j], "-m") == 0)
            {
            if (j + 2 > argc)
                {
                printf ("Not enough arguments\n");
                printf (usageString, argv[0]);
                exit (1);
                }
            if (strcmp (argv[j + 1], "iecrpt") == 0)
                {
                test_mode = TEST_MODE_IEC_RPT;
                j += 2; /* skip ahead */
                }
            else if (strcmp (argv[j + 1], "ieccontrol") == 0)
                {
                test_mode = TEST_MODE_IEC_CONTROL;
                j += 2; /* skip ahead */
                }
            else if (strcmp (argv[j + 1], "iecremote") == 0)
                {
                if (j + 5 > argc)
                    {
                    printf ("Not enough arguments\n");
                    printf (usageString, argv[0]);
                    exit (1);
                    }
                test_mode = TEST_MODE_IEC_REMOTE;
                sclFileName = argv[j + 2];
                iedName = argv[j + 3];
                apName = argv[j + 4];
                j += 5; /* skip ahead  */
                }
            else if (strcmp (argv[j + 1], "identify") == 0)
                {
                if (j + 3 > argc)
                    {
                    printf ("Not enough arguments\n");
                    printf (usageString, argv[0]);
                    exit (1);
                    }
                test_mode = TEST_MODE_IDENTIFY;
                identifyIterations = atoi (argv[j + 2]);
                j += 3; /* skip ahead  */
                }
            else
                {
                printf ("Unrecognized value for '-m' option\n");
                printf (usageString, argv[0]);
                exit (1);
                }
            }
#if 0 /* NOTE: add something like this to support a new argument (e.g. "-a").*/
        else if (strcmp (argv[j], "-a") == 0)
            {
            /* Add code to process this argument and increment index "j" as needed.*/
            }
#endif
        else
            /* some other non-supported argument */
            {
            printf ("Unrecognized argument\n");
            printf (usageString, argv[0]);
            exit (1);
            }
        }   /* end loop checking command line args  */

    setbuf (stdout, NULL);    /* do not buffer the output to stdout   */
    setbuf (stderr, NULL);    /* do not buffer the output to stderr   */

    printf ("%s Version %s\n", MMSLITE_NAME, MMSLITE_VERSION);
#if defined(S_SEC_ENABLED)
    printf ("Security Enabled\n");
#endif
    puts (MMSLITE_COPYRIGHT);


#ifdef S_MT_SUPPORT
    /* init glbsem explicitly to avoid auto-initialization calls from multiple threads */
    if ((ret = gs_init ()) != SD_SUCCESS)
        {
        printf ("gs_init() failed");
        exit (1);
        }
#endif

    /* init stime explicitly to avoid auto-initialization calls from multiple threads */
    if ((ret = stimeInit ()) != SD_SUCCESS)
        {
        printf ("stimeInit() failed");
        exit (1);
        }

#if defined(NO_GLB_VAR_INIT)
    mvl_init_glb_vars ();
#endif

    init_mem ();          /* Set up memory allocation tools               */

    init_log_cfg ();

    SLOGALWAYS2 ("%s Version %s", MMSLITE_NAME, MMSLITE_VERSION);
#if defined(S_SEC_ENABLED)
    SLOGCALWAYS0 ("Security Enabled");
#endif

    /* We want to know about connection activity                            */
    u_mvl_disc_ind_fun = disc_ind_fun;

#if defined(S_SEC_ENABLED) && !defined(_WIN32)
    /* For Linux, etc., set the directory to find "certstore". */
    /* NOTE: "/usr/sisco" is default. You can skip this if you use that path.*/
    sscSetSiscoInstallPath ("/usr/sisco");
#endif

#if defined(HARD_CODED_CFG)
    num_loc_dib_entries = sizeof (localDibTable) / sizeof (DIB_ENTRY);
    loc_dib_table = localDibTable;
    num_rem_dib_entries = sizeof (remoteDibTable) / sizeof (DIB_ENTRY);
    rem_dib_table = remoteDibTable;
#if 0   /* Only call config functions necessary for stack being used.   */
    tp4_config ();         /* see tp4_hc.c         */
    clnp_config ();        /* see clnp_hc.c        */
    adlcConfigure ();      /* see adlc_hc.c        */
#endif
  /* Fill in mvlCfg.    */
    mvlCfg.num_calling = 4;
    mvlCfg.num_called = 0;
    mvlCfg.max_msg_size = 8000;
    strcpy (mvlCfg.local_ar_name, "local1");

#elif defined(SISCO_STACK_CFG)
#if !defined(_WIN32)
    /* Set path to find "siscostackcfg.xml". Change if file is moved. */
    /* NOTE: "/usr/sisco/network" is default. You can skip this if you use that path.*/
    sscSetCfgFilePath ("/usr/sisco/network");
#endif
    /* "appName" must match an App_Name element in "siscostackcfg.xml".*/
    ret = sscConfigMvl (appName, &mvlCfg);
    if (ret)
        {
        printf ("\n Stack configuration error = 0x%X. Check 'siscostackcfg.xml'.\n", ret);
        exit (1);
        }
#else /* default config */

    /* Read the configuration from a file */
    ret = osicfgx ("osicfg.xml", &mvlCfg);        /* This fills in mvlCfg */
    if (ret != SD_SUCCESS)
        {
        printf ("\n Stack configuration failed, err = 0x%X. Check configuration.\n", ret);
        exit (1);
        }
#endif  /* default config */

    /* Set the ^c catcher */
    signal (SIGINT, ctrlCfun);

#if defined(S_SEC_ENABLED)
#if defined(SNAP_LITE_THR)
    ret = snapThrStart (&thSnapThr, &tidSnapThr, 2000);
    if (ret)
        {
        printf ("\n SNAP-Lite thread failed to start. ret  = 0x%X.\n", ret);
        exit (1);
        }
#else /* !defined(SNAP_LITE_THR) */
    ret = ulInitSecurity ();
    if (ret != SD_SUCCESS)
        {
        printf ("Unable to Initialize Security Components (%04x)", ret);
        exit (1);
        }
#endif /* !defined(SNAP_LITE_THR) */
#endif  /* defined(S_SEC_ENABLED) */

    mvl_max_dyn.types = 1000;     /* CRITICAL: must be before mvl_init_mms_objs*/
    /* (called by mvl_start_acse)           */
#if defined(GOOSE_RX_SUPP) || defined(GSSE_RX_SUPP) || defined(GSE_MGMT_RX_SUPP) || defined(SMPVAL_RX_SUPP)
  /* NOTE: clnp_init required for GSSE. It also calls clnp_snet_init    */
  /*       which is required for GOOSE, GSE_MGMT, SMPVAL.               */
    ret = clnp_init (1200);       /* big enough for any GSSE msg  */
    if (ret)
        {
        printf ("clnp_init error 0x%X", ret);
        exit (49);
        }
#endif

    ret = mvl_start_acse (&mvlCfg);       /* MAKE SURE mvlCfg is filled in*/
    if (ret != SD_SUCCESS)
        {
        printf ("\n mvl_start_acse () failed, err = 0x%X.\n", ret);
        exit (1);
        }

    if ((ret = rpt_typeids_find (&rpt_typeids)) != SD_SUCCESS)
        {
        printf ("rpt_typeids_find () failed = 0x%X.\n", ret);
        exit (1);
        }

#if defined(MVL_GOOSE_SUPPORT)
    /* NOTE: this GOOSE initialization must be AFTER mvl_start_acse so    */
    /*   that required types can be found.                                */
    goosehandle = goose_init ();
#endif

#if defined(GOOSE_RX_SUPP) || defined(GSSE_RX_SUPP) || defined(GSE_MGMT_RX_SUPP) || defined(SMPVAL_RX_SUPP)

    /* CRITICAL: to receive Multicast packets (for GOOSE, SMPVAL, etc.),  */
    /* must set multicast filter using clnp_snet_set_multicast_filter     */
    /* or clnp_snet_rx_all_multicast_start (accept ALL multicast packets).*/
    /* NOTE: this is AFTER goose_init to override filter set by goose_init.*/

    if ((ret = clnp_snet_rx_all_multicast_start ()) != SD_SUCCESS)
        {
        SLOGALWAYS1 ("Error 0x%04X setting multicast filter.", ret);
        printf ("Error 0x%04X setting multicast filter.\n", ret);
        }
#endif

#if !defined(NO_KEYBOARD) && !defined(_WIN32) && !defined(__QNX__)
    term_init (); /* makes getchar work right     */
#endif

#if defined(USE_DIB_LIST)
  /* First copy addresses from "osicfg.xml" (stored in "loc_dib_table"  */
  /* and "rem_dib_table" arrays) to the new DIB list.                   */
  /* NOTE: Client address is probably not in the SCL file, so it should */
  /*       come from here.                                              */
    for (j = 0; j < num_loc_dib_entries; j++)
        {
        dib_entry_create (&loc_dib_table[j]);       /* add to new DIB list  */
        }
    for (j = 0; j < num_rem_dib_entries; j++)
        {
        dib_entry_create (&rem_dib_table[j]);       /* add to new DIB list  */
        }
#endif  /* USE_DIB_LIST */

    /* For this test_mode, process SCL before connecting to server.       */
    if (test_mode == TEST_MODE_IEC_REMOTE)
        {
        /* Use SCL to configure a remote VMD (used later by                 */
        /* "test_iec_remote_vmd" after a connection is established).        */
        /* NOTE: could configure several VMDs like this using different SCL files.*/
        remote_vmd_ctrl = config_iec_remote_vmd (
            sclFileName,  /* from command line  */
            iedName,      /* from command line  */
            apName,       /* from command line  */
            &remote_scl_info);
        if (remote_vmd_ctrl == NULL)
            {
            printf ("\nconfig_iec_remote_vmd failed for SCL File = '%s'. Check configuration.\n", sclFileName);
            exit (100);
            }
        log_iec_remote_vmd_var_names (remote_vmd_ctrl);

#ifdef USE_DIB_LIST     /* only works with new dib      */
        /* Add addresses from SCL file to new global linked list    */
        if (scl_dib_entry_save (&remote_scl_info) < 0)
            {
            printf ("ERROR creating DIB entries from SCL.");
            exit (100);
            }

        /* Generate "IPv4" AR Name from cmd line arguments "iedName" and "apName".*/
        /* This must match the name generated by the SCL parser.  */
        strncpy_safe (serverName, iedName, sizeof (serverName) - 1);
        strncat_maxstrlen (serverName, "/", sizeof (serverName) - 1);
        ret = strncat_maxstrlen (serverName, apName, sizeof (serverName) - 1);
        if (ret)
            {
            printf ("ERROR: generated AR Name = '%s'\n was truncated. Check command line arguments.\n",
                    serverName);
            exit (100);
            }

        /* Generate "IPv6" AR Name from cmd line arguments "iedName" and "apName".*/
        /* This must match the name generated by the SCL parser (suffix added). */
        strcpy (serverNameIpv6, serverName);
        ret = strncat_maxstrlen (serverNameIpv6, IPV6_AR_NAME_SUFFIX, sizeof (serverNameIpv6) - 1);
        if (ret)
            {
            printf ("ERROR: generated AR Name = '%s'\n was truncated. Check command line arguments.\n",
                    serverNameIpv6);
            exit (100);
            }

        printf ("\n\n IPv4 Remote AR Name generated from cmd line args = '%s'.\n"
                " IPv6 Remote AR Name generated from cmd line args = '%s'.\n",
                serverName, serverNameIpv6);
#endif  /* USE_DIB_LIST */
        scl_info_destroy (&remote_scl_info);        /* Don't need SCL info anymore  */
        }

    /* Try more than once to connect to Server.   */
    for (j = 0; j < 5; j++)
        {
        printf ("\n Connecting to Server '%s'...", serverName);
        clientNetInfo = connectToServer (mvlCfg.local_ar_name, serverName);
        if (clientNetInfo == NULL)
            {
            printf ("  failed.");
            if (j < 4)
                {       /* Not last loop. try again.    */
                printf ("  Retry in 5 seconds.");
                sMsSleep (5000);        /* wait 5 seconds & try again   */
                }
            }
        else
            {
            printf ("Connected\n");
            break;    /* connected. Stop looping.     */
            }
        }

    /* Only if IPv6 address is found in the DIB, try to make IPv6 connection (just once). */
    if (find_dib_entry (serverNameIpv6) != NULL)
        {
        printf ("\n Connecting using IPv6 to Server '%s'...", serverNameIpv6);
        clientNetInfoIpv6 = connectToServer (mvlCfg.local_ar_name, serverNameIpv6);
        if (clientNetInfoIpv6 == NULL)
            {
            printf ("Failed\n");
            }
        else
            {
            printf ("Connected\n");
            }
        }

    if ((clientNetInfo != NULL) && (test_mode == TEST_MODE_IDENTIFY))
        {
        printf ("\n\nRunning %d continuous Identify Test\n", identifyIterations);
        sendIdentify (clientNetInfo, identifyIterations);
        }

    if (clientNetInfo != NULL)
        {
        printf ("\n\nRunning tests with IPv4 Server '%s'.\n", serverName);
        runTests (clientNetInfo, test_mode, &rpt_typeids, remote_vmd_ctrl);
        }

    if (clientNetInfoIpv6 != NULL)
        {
        printf ("\n\nRunning tests with IPv6 Server '%s'.\n", serverNameIpv6);
        runTests (clientNetInfoIpv6, test_mode, &rpt_typeids, remote_vmd_ctrl);
        }

    printf ("\n Entering 'server' mode, hit ^c or 'x' to exit ... ");
#if defined(MVL_GOOSE_SUPPORT)
    TimeOld = sGetMsTime ();      /* init old time        */
#endif

    while (doIt)
        {
        doCommService ();
        kbService ();

#if defined(MVL_GOOSE_SUPPORT)
        /* Check for GOOSE timeouts on any subscriptions.   */
        TimeNew = sGetMsTime ();                            /* get current time*/
        TimeElapsed = (ST_LONG)(TimeNew - TimeOld);        /* calc elapsed time*/
        TimeOld = TimeNew;                                  /* update old time*/
        chk_iec_goose_timeout (TimeElapsed);        /* look for expired TAL */
#endif
#if defined(S_SEC_ENABLED)
    /* Periodically check certificates.  */
        if (sGetMsTime () > lastCertCheckTime + 60000)  /* 60000 msec = 60 sec  */
            {
            ulCheckCertExpirationAll ();
            ulUpdateCertRevocationList ();
            lastCertCheckTime = sGetMsTime ();
            }
#endif
        }

    doIt = SD_TRUE;       /* Turn it on again so disconnect can succeed   */

    /* If connect succeeded, do cleanup.    */
    if (clientNetInfo)
        {
        if ((all_rcb_info = clientNetInfo->user_info) != NULL)
            {
            /* Free up rcb_info if necessary. */
            while ((rcb_info = list_get_first (&all_rcb_info->rcb_info_list)) != NULL)
                rcb_info_destroy (rcb_info);
            chk_free (all_rcb_info);
            }
        clientNetInfo->user_info = NULL;

        /* Issue Conclude */
        disconnectFromServer (clientNetInfo);
        }
    /* Do same cleanup for IPv6 connection. */
    if (clientNetInfoIpv6)
        {
        if ((all_rcb_info = clientNetInfoIpv6->user_info) != NULL)
            {
            /* Free up rcb_info if necessary. */
            while ((rcb_info = list_get_first (&all_rcb_info->rcb_info_list)) != NULL)
                rcb_info_destroy (rcb_info);
            chk_free (all_rcb_info);
            }
        clientNetInfoIpv6->user_info = NULL;

        /* Issue Conclude */
        disconnectFromServer (clientNetInfoIpv6);
        }

    /* Done with VMDs. Destroy them.      */
    if (test_mode == TEST_MODE_IEC_REMOTE && remote_vmd_ctrl != NULL)
        {
        mvl_vmd_destroy (remote_vmd_ctrl);  /* only created for this mode*/
        }
    mvl_vmd_destroy (&mvl_vmd);

    /* If cleanup worked, number of types logged here should be the number*/
    /* of types created by Foundry (non-dynamic).                         */
    SLOGALWAYS1 ("DEBUG: Number of types in use after cleanup=%d", mvl_type_count ());

    dyn_mem_ptr_statistics (0);
#if defined(GOOSE_RX_SUPP) || defined(GSSE_RX_SUPP) || defined(GSE_MGMT_RX_SUPP) || defined(SMPVAL_RX_SUPP)
    /* clnp_init called at startup, so must call clnp_end to clean up.*/
    clnp_end ();
#endif
    mvl_end_acse ();

    /* Free buffers allocated by "mvl_init_mms_objs".     */
    mvl_free_mms_objs ();

#if defined(S_SEC_ENABLED)
#if defined(SNAP_LITE_THR)
    snapThrStop (thSnapThr, tidSnapThr, 2000);
#else  /* !defined(SNAP_LITE_THR) */
    ulEndSecurity ();
#endif /* !defined(SNAP_LITE_THR) */
#endif /* defined(S_SEC_ENABLED) */

#if defined(SISCO_STACK_CFG)
    /* free the SISCO Stack Configuration */
    sscFreeStackCfg ();
#else
    /* Free buffers allocated by "osicfgx".       */
    osicfgx_unconfigure ();
#endif

#if defined(USE_DIB_LIST)
    /* Destroy addresses created at startup.      */
    dib_entry_destroy_all ();
#endif

    /* This should log only a few buffers which are freed by slog_end and */
    /* logcfgx_unconfigure below.                                         */
    dyn_mem_ptr_status ();        /* is everything freed up?      */

#if defined(DEBUG_SISCO)
  /* terminate logging services and save current log file position */
    slog_end (sLogCtrl);

    /* Free buffers allocated by "logcfgx".                               */
    /* NOTE: this is AFTER slog_end, so it can't screw up logging.        */
    logcfgx_unconfigure (sLogCtrl);
#endif

    printf ("\n\n");
#if !defined(NO_KEYBOARD) && !defined(_WIN32) && !defined(__QNX__)
    term_rest (); /* Must be called before exit if term_init used.        */
#endif
#if defined(MVL_GOOSE_SUPPORT)
  /* Destroy all subscribers.   */
    if (goosehandle != NULL && iecGooseUnSubscribe (goosehandle) != SD_SUCCESS)
        printf ("\niecGooseUnsubscribe failed");
#endif

    /* Should have 0 buffers allocated now.       */
    if (chk_curr_mem_count != 0)
        printf ("Note: %lu buffers not freed.\n",
        (ST_ULONG)chk_curr_mem_count);

    return (0);
    }

/************************************************************************/
/* Confirm function for mvla_fget request.                              */
/************************************************************************/
ST_VOID fget_cnf (MVL_FGET_REQ_INFO* fget_req)
    {
    printf ("\n mvla_fget completed: result = 0x%X.\n", fget_req->fget_error);
    chk_free (fget_req);
    }

/************************************************************************/
/*                       test_async                                     */
/* This function tests reading and writing variables asynchronously.    */
/************************************************************************/
ST_VOID test_async (MVL_NET_INFO* clientNetInfo)
    {
    ST_CHAR* dom_name;
    ST_CHAR* var_name;   /* name of variable to write and read */
    static ST_UINT32 sboTimeout;  /* "sboTimeout" value to write  */
    /* CRITICAL: data pointer for Read must be valid until response is    */
    /*           processed. Allocation is recommended or you may point to */
    /*           global variables.                                        */
    ST_VOID* write_data;       /* ptr to data written */
    ST_VOID* read_data;        /* ptr to data read */
    MVL_TYPE_CTRL* type_ctrl;
    ST_RET ret;
    MVL_TYPE_ID int32u_type_id;

    /* Create any types we need.  */
    int32u_type_id = mvl_type_id_create_from_tdl (NULL, "Ulong");

    dom_name = "E1Q1SB1C1";
    var_name = "LLN0$CF$Mod$sboTimeout";
    /* NOTE: write_data should point to static or allocated data, just so */
    /*       you know the pointer is still valid in the done function     */
    /*       "named_var_write_async_done".                                */
    write_data = &sboTimeout;

    /* Write 10 values to sboTimeout and read them back. */
    for (sboTimeout = 1000; sboTimeout < 1010; sboTimeout++)
        {
        ret = named_var_write_async (clientNetInfo, var_name, DOM_SPEC, dom_name,
                                     int32u_type_id, write_data,
                                     named_var_write_async_done);  /* called when resp rcvd  */
        if (ret)
            printf ("\n Immediate error writing '%s' = 0x%X", var_name, ret);
        /* Make sure we don't exceed max pending requests.  */
        while (clientNetInfo->numpend_req >= clientNetInfo->maxpend_req)
            doCommService ();

        /* Allocate data ptr. This must be freed in named_var_read_async_done.  */
        /* NOTE: make sure to allocate the right size for the data being read.  */
        type_ctrl = mvl_type_ctrl_find (int32u_type_id);
        read_data = chk_malloc (type_ctrl->data_size);
        ret = named_var_read_async (clientNetInfo, var_name, DOM_SPEC, dom_name,
                                    int32u_type_id, read_data,
                                    named_var_read_async_done);  /* called when resp rcvd  */
        if (ret)
            printf ("\n Immediate error reading '%s' = 0x%X", var_name, ret);
        /* Make sure we don't exceed max pending requests.  */
        while (clientNetInfo->numpend_req >= clientNetInfo->maxpend_req)
            doCommService ();
        }
    }

/************************************************************************/
/*                       test_simple_requests                           */
/* This function sends simple requests to the server. It is designed    */
/* to communicate with the sample "server" program. It is NOT for use   */
/* with an IEC or UCA server.                                           */
/************************************************************************/
ST_VOID test_simple_requests (MVL_NET_INFO* clientNetInfo)
    {
    ST_INT16 remTemperature;
    ST_INT16 write_val;
    ST_RET ret;
    ST_INT i, j;
    IDENT_RESP_INFO* ident_resp;
    MVL_FDIR_RESP_INFO* fdir_resp;
    MVL_REQ_PEND* reqCtrl;

    VARIABLE_LIST* variable_list;
    DEFVLIST_REQ_INFO* defvlist_req;

    GETVLIST_REQ_INFO getvlist_req;
    GETVLIST_RESP_INFO* getvlist_resp;      /* set to reqCtrl->u.getvlist_resp_info*/

    DELVLIST_REQ_INFO delvlist_req;
    JINIT_REQ_INFO jinit_req;
    JSTAT_REQ_INFO jstat_req;
    JREAD_REQ_INFO jread_req;
    MVL_JREAD_RESP_INFO* jread_resp;        /* set to reqCtrl->u.jread_resp_info*/
    MVL_JOURNAL_ENTRY* jou_entry;
    ST_CHAR* src_file_name;
    ST_CHAR* dest_file_name;
    ST_CHAR* file_to_delete;
    ST_CHAR* file_to_rename;
    ST_CHAR* new_file_name;
    GETVAR_REQ_INFO getvar_req;
    GETDOM_REQ_INFO getdom_req;
    NAMELIST_REQ_INFO getnam_req;
    NAMELIST_RESP_INFO* getnam_resp;
    ST_CHAR** nptr;
    STATUS_REQ_INFO status_req;
#if (UNICODE_LOCAL_FORMAT==UNICODE_UTF16)
    MVL_TYPE_ID utf8_type_id;
    ST_UINT16 remUTF8string[13 + 1];
#endif
    MVL_FGET_REQ_INFO* fget_req_info;       /* allocate this        */
    ST_CHAR* localFile, * remoteFile;
    MVL_TYPE_ID int16_type_id;

    /* Create any types we need.  */
    int16_type_id = mvl_type_id_create_from_tdl (NULL, "Short");

    /* send "Identify" request and wait for reply */
    ret = mvla_identify (clientNetInfo, &reqCtrl);
    if (ret == SD_SUCCESS)
        ret = waitReqDone (reqCtrl, timeOut);
    if (ret == SD_SUCCESS)
        {
        ident_resp = reqCtrl->u.ident.resp_info;
        printf ("\n Vendor = %s, Model = %s, Rev = %s", ident_resp->vend,
                ident_resp->model, ident_resp->rev);
        }
    else
        printf ("\n mvl_identify () Error, ret = 0x%X.", ret);
    mvl_free_req_ctrl (reqCtrl);

#if (MMS_STATUS_EN & REQ_EN)
    status_req.extended = SD_FALSE;
    ret = mvla_status (clientNetInfo, &status_req, &reqCtrl);
    if (ret == SD_SUCCESS)
        ret = waitReqDone (reqCtrl, timeOut);
    if (ret != SD_SUCCESS)
        printf ("\n mvl_status () Error, ret = 0x%x.", ret);
    else
        {
        printf ("\n mvl_status OK");
        printf ("\n   logical status  = %d", reqCtrl->u.status.resp_info->logical_stat);
        printf ("\n   physical status = %d", reqCtrl->u.status.resp_info->physical_stat);
        if (reqCtrl->u.status.resp_info->local_detail_pres == SD_FALSE)
            printf ("\n   No local detail");
        else
            {
            printf ("\n   Local detail present");
            printf ("\n     Local detail len = %d bits",
                    reqCtrl->u.status.resp_info->local_detail_len);
            }
        }
    mvl_free_req_ctrl (reqCtrl);
#endif


#if (MMS_GETVAR_EN & REQ_EN)
    getvar_req.req_tag = GETVAR_NAME;
    getvar_req.name.object_tag = VMD_SPEC;
    getvar_req.name.obj_name.vmd_spec = "Temperature";

    ret = mvla_getvar (clientNetInfo, &getvar_req, &reqCtrl);
    if (ret == SD_SUCCESS)
        ret = waitReqDone (reqCtrl, timeOut);
    if (ret != SD_SUCCESS)
        printf ("\n mvl_getvar () Error, ret = 0x%x.", ret);
    else
        {
        printf ("\n mvl_getvar OK");
        printf ("\n   len = %d", reqCtrl->u.getvar.resp_info->type_spec.len);
        }
    mvl_free_req_ctrl (reqCtrl);
#endif

#if (MMS_GET_DOM_EN & REQ_EN)
    strcpy (getdom_req.dname, "mvlLiteDom");
    ret = mvla_getdom (clientNetInfo, &getdom_req, &reqCtrl);
    if (ret == SD_SUCCESS)
        ret = waitReqDone (reqCtrl, timeOut);
    if (ret != SD_SUCCESS)
        printf ("\n mvl_getdom () Error, ret = 0x%x.", ret);
    else
        {
        printf ("\n mvl_getdom OK");
        printf ("\n   num_of_capab = %d", reqCtrl->u.getdom.resp_info->num_of_capab);
        if (reqCtrl->u.getdom.resp_info->mms_deletable == SD_FALSE)
            printf ("\n   MMS Deletable : NO");
        else
            printf ("\n   MMS Deletable : YES");
        if (reqCtrl->u.getdom.resp_info->sharable == SD_FALSE)
            printf ("\n   Sharable : NO");
        else
            printf ("\n   Sharable : YES");
        printf ("\n   num_of_pinames = %d", reqCtrl->u.getdom.resp_info->num_of_pinames);
        printf ("\n   State = %d", reqCtrl->u.getdom.resp_info->state);
        if (reqCtrl->u.getdom.resp_info->upload_in_progress == SD_FALSE)
            printf ("\n   Upload in Progress : NO");
        else
            printf ("\n   Upload in Progress : YES");
        }
    mvl_free_req_ctrl (reqCtrl);
#endif

#if (MMS_GETNAMES_EN & REQ_EN)
    getnam_req.cs_objclass_pres = SD_FALSE;
    getnam_req.obj.mms_class = 0;
    getnam_req.objscope = VMD_SPEC;
    getnam_req.cont_after_pres = SD_FALSE;

    ret = mvla_getnam (clientNetInfo, &getnam_req, &reqCtrl);
    if (ret == SD_SUCCESS)
        ret = waitReqDone (reqCtrl, timeOut);
    if (ret != SD_SUCCESS)
        printf ("\n mvl_getnam () Error, ret = 0x%x.", ret);
    else
        {
        printf ("\n mvl_getnam OK");
        getnam_resp = (NAMELIST_RESP_INFO*)reqCtrl->u.ident.resp_info;
        if (getnam_resp->more_follows)
            printf ("\n   More Follows : TRUE");
        else
            printf ("\n   More Follows : FALSE");
        printf ("\n   %d Names returned : ", getnam_resp->num_names);
        printf ("\n   Name List :");

        nptr = (ST_CHAR * *)(getnam_resp + 1);

        for (i = 0; i < getnam_resp->num_names; ++i)
            printf ("\n     %s ", nptr[i]);
        }
    mvl_free_req_ctrl (reqCtrl);
#endif


    for (i = 0; i < 2; ++i)
        {
        ret = named_var_read (clientNetInfo, "Temperature", VMD_SPEC, NULL, /* no domain name*/
                              int16_type_id, &remTemperature, timeOut);
        if (ret == SD_SUCCESS)
            printf ("\nRemote Temperature: %d, count %d",
            (int)remTemperature, i + 1);
        else
            printf ("\n Read Error");
        }

    /* Test new asynchronous read/write functions.  */
    test_async (clientNetInfo);

#if (UNICODE_LOCAL_FORMAT==UNICODE_UTF16)
    utf8_type_id = mvl_type_id_create_from_tdl (NULL, "UTF8Vstring13");
    ret = named_var_read (clientNetInfo, "UTF8Vstring13test", VMD_SPEC, NULL,     /* no domain name*/
                          utf8_type_id, &remUTF8string, timeOut);
    if (ret == SD_SUCCESS)
        {
        printf ("\nRemote UTF8string Read SUCCESS");
        SLOGALWAYS0 ("Remote UTF8string Read SUCCESS. Converted to UTF16");
        SLOGALWAYSH (sizeof (remUTF8string), remUTF8string);
        }
    else
        printf ("\nRemote UTF8string Read FAILED");
#endif

    write_val = 9;
    ret = named_var_write (clientNetInfo, "Temperature", VMD_SPEC, NULL,  /* no domain name*/
                           int16_type_id, &write_val, timeOut);
    if (ret != SD_SUCCESS)
        printf ("\n Write Error");
    else
        printf ("\n Write OK");

#if (MMS_FRENAME_EN & REQ_EN)
    file_to_rename = "datafile.dat";
    new_file_name = "newfile.dat";
    ret = mvla_frename (clientNetInfo, file_to_rename, new_file_name, &reqCtrl);
    if (ret == SD_SUCCESS)
        ret = waitReqDone (reqCtrl, timeOut);
    if (ret != SD_SUCCESS)
        printf ("\n mvl_frename () Error renaming file '%s', ret = 0x%X.",
                file_to_rename, ret);
    else
        printf ("\n mvl_frename () of '%s' OK.", file_to_rename);
    mvl_free_req_ctrl (reqCtrl);
#endif

#if (MMS_OBTAINFILE_EN & REQ_EN)
    src_file_name = "client.c";
    dest_file_name = "temp.c";
    ret = mvla_obtfile (clientNetInfo, src_file_name, dest_file_name, &reqCtrl);
    if (ret == SD_SUCCESS)
        ret = waitReqDone (reqCtrl, timeOut);
    if (ret != SD_SUCCESS)
        printf ("\n mvl_obtfile () Error obtaining file '%s', ret = 0x%X.",
                src_file_name, ret);
    else
        printf ("\n mvl_obtfile () of '%s' OK.", src_file_name);
    mvl_free_req_ctrl (reqCtrl);
#endif

#if (MMS_FDELETE_EN & REQ_EN)
    file_to_delete = "junk.jnk";
    ret = mvla_fdelete (clientNetInfo, file_to_delete, &reqCtrl);
    if (ret == SD_SUCCESS)
        ret = waitReqDone (reqCtrl, timeOut);
    if (ret != SD_SUCCESS)
        printf ("\n mvl_fdelete () Error deleting file '%s', ret = 0x%X.",
                file_to_delete, ret);
    else
        printf ("\n mvl_fdelete () of '%s' OK.", file_to_delete);
    mvl_free_req_ctrl (reqCtrl);
#endif

#if (MMS_FOPEN_EN & REQ_EN) && (MMS_FREAD_EN & REQ_EN) && (MMS_FCLOSE_EN & REQ_EN)
    remoteFile = "osicfg.xml";
    localFile = "junk.jnk";
    ret = getFile (clientNetInfo, localFile, remoteFile);
    if (ret != SD_SUCCESS)
        printf ("\n getFile () Error getting remote file '%s', ret = 0x%X.",
                remoteFile, ret);
    else
        printf ("\n getFile () of remote file '%s' OK", remoteFile);

    /* Use mvla_fget to transfer the same file.                           */
    /* NOTE: File transfer is not complete until fget_cnf is called.      */
    /* CRITICAL: allocate fget_req_info (used later by callback functions).*/
    fget_req_info = chk_calloc (1, sizeof (MVL_FGET_REQ_INFO));
    fget_req_info->fget_cnf_ptr = fget_cnf;       /* CRITICAL: must set this ptr*/
    ret = mvla_fget (clientNetInfo, remoteFile, localFile, fget_req_info);
    if (ret != SD_SUCCESS)
        printf ("\n mvla_fget () Error getting remote file '%s', ret = 0x%X.",
                remoteFile, ret);
    else
        printf ("\n mvla_fget () of remote file '%s': request sent OK. WAIT for completion result.", remoteFile);
#endif

#if (MMS_FDIR_EN & REQ_EN)
    ret = mvla_fdir (clientNetInfo,
                     "*",  /* fs_name (IEC 61850 expects "*" or ""  */
                     NULL, /* ca_name      */
                     &reqCtrl);
    if (ret == SD_SUCCESS)
        ret = waitReqDone (reqCtrl, timeOut);
    if (ret != SD_SUCCESS)
        printf ("\n mvl_fdir () Error, ret = 0x%X.", ret);
    else
        {
        fdir_resp = reqCtrl->u.fdir.resp_info;
        printf ("\n mvl_fdir results:");
        for (i = 0; i < fdir_resp->num_dir_ent; i++)
            printf ("\n   File #%d: %s", i, fdir_resp->dir_ent[i].filename);
        }
    mvl_free_req_ctrl (reqCtrl);  /* CRITICAL:            */
#endif

#if (MMS_DEFVLIST_EN & REQ_EN)
    defvlist_req =
        (DEFVLIST_REQ_INFO*)chk_calloc (1, sizeof (DEFVLIST_REQ_INFO) +
                                        2 * sizeof (VARIABLE_LIST));
    defvlist_req->vl_name.object_tag = VMD_SPEC;
    defvlist_req->vl_name.obj_name.vmd_spec = "NewVlist";
    defvlist_req->num_of_variables = 2;
    variable_list = (VARIABLE_LIST*)(defvlist_req + 1);

    /* Just define 2 copies of Temperature (almost all SISCO apps have it)*/
    variable_list->var_spec.vs.name.object_tag = VMD_SPEC;
    variable_list->var_spec.vs.name.obj_name.vmd_spec = "Temperature";
    variable_list++;
    variable_list->var_spec.vs.name.object_tag = VMD_SPEC;
    variable_list->var_spec.vs.name.obj_name.vmd_spec = "Temperature";
    variable_list++;

    ret = mvla_defvlist (clientNetInfo, defvlist_req, &reqCtrl);
    if (ret == SD_SUCCESS)
        ret = waitReqDone (reqCtrl, timeOut);
    if (ret != SD_SUCCESS)
        printf ("\n mvl_defvlist () Error, ret = 0x%X.", ret);
    else
        printf ("\n mvl_defvlist OK");
    mvl_free_req_ctrl (reqCtrl);
    chk_free (defvlist_req);
#endif

#if (MMS_GETVLIST_EN & REQ_EN)
    getvlist_req.vl_name.object_tag = VMD_SPEC;
    getvlist_req.vl_name.obj_name.vmd_spec = "NewVlist";

    ret = mvla_getvlist (clientNetInfo, &getvlist_req, &reqCtrl);
    if (ret == SD_SUCCESS)
        ret = waitReqDone (reqCtrl, timeOut);
    if (ret != SD_SUCCESS)
        printf ("\n mvl_getvlist () Error, ret = 0x%X.", ret);
    else
        {
        getvlist_resp = reqCtrl->u.getvlist.resp_info;
        printf ("\n mvl_getvlist results:");
        if (getvlist_resp->mms_deletable)
            printf ("    Deletable");
        else
            printf ("    NOT Deletable");
        variable_list = (VARIABLE_LIST*)(getvlist_resp + 1);
        for (i = 0; i < getvlist_resp->num_of_variables; i++, variable_list++)
            printf ("\n   Var #%d: %s", i, variable_list->var_spec.vs.name.obj_name.vmd_spec);
        }
    mvl_free_req_ctrl (reqCtrl);  /* CRITICAL:            */
#endif

#if (MMS_DELVLIST_EN & REQ_EN)
    delvlist_req.scope = DELVL_VMD;
    delvlist_req.dname_pres = SD_FALSE;
    delvlist_req.vnames_pres = SD_FALSE;

    ret = mvla_delvlist (clientNetInfo, &delvlist_req, &reqCtrl);
    if (ret == SD_SUCCESS)
        ret = waitReqDone (reqCtrl, timeOut);
    if (ret != SD_SUCCESS)
        printf ("\n mvl_delvlist () Error, ret = 0x%X.", ret);
    else
        {
        printf ("\n mvl_delvlist OK");
        printf ("\n   num_matched = %lu", (ST_ULONG)reqCtrl->u.delvlist.resp_info->num_matched);
        printf ("\n   num_deleted = %lu", (ST_ULONG)reqCtrl->u.delvlist.resp_info->num_deleted);
        }
    mvl_free_req_ctrl (reqCtrl);
#endif

#if (MMS_JINIT_EN & REQ_EN)
    printf ("\n\n Testing VMD_SPEC JournalInit");
    jinit_req.jou_name.object_tag = VMD_SPEC;
    jinit_req.jou_name.obj_name.vmd_spec = "SampleJournal";
    jinit_req.limit_spec_pres = 0;
    jinit_req.limit_entry_pres = 0;
    ret = mvla_jinit (clientNetInfo, &jinit_req, &reqCtrl);
    if (ret == SD_SUCCESS)
        ret = waitReqDone (reqCtrl, timeOut);
    if (ret != SD_SUCCESS)
        printf ("\n mvl_jinit () Error, ret = 0x%X.", ret);
    else
        {
        printf ("\n mvl_jinit OK");
        printf ("\n   del_entries = %lu", (ST_ULONG)reqCtrl->u.jinit.resp_info->del_entries);
        }
    mvl_free_req_ctrl (reqCtrl);
#endif

#if (MMS_JSTAT_EN & REQ_EN)
    printf ("\n\n Testing VMD_SPEC JournalStat");
    jstat_req.jou_name.object_tag = VMD_SPEC;
    jstat_req.jou_name.obj_name.vmd_spec = "SampleJournal";
    ret = mvla_jstat (clientNetInfo, &jstat_req, &reqCtrl);
    if (ret == SD_SUCCESS)
        ret = waitReqDone (reqCtrl, timeOut);
    if (ret != SD_SUCCESS)
        printf ("\n mvl_jstat () Error, ret = 0x%X.", ret);
    else
        {
        printf ("\n mvl_jstat OK");
        printf ("\n   cur_entries = %lu", (ST_ULONG)reqCtrl->u.jstat.resp_info->cur_entries);
        printf ("\n   mms_deletable = %d", reqCtrl->u.jstat.resp_info->mms_deletable);
        }
    mvl_free_req_ctrl (reqCtrl);
#endif

#if (MMS_JREAD_EN & REQ_EN)
    printf ("\n\n Testing VMD_SPEC JournalRead");
    jread_req.jou_name.object_tag = VMD_SPEC;
    jread_req.jou_name.obj_name.vmd_spec = "SampleJournal";
    jread_req.range_start_pres = 0;
    jread_req.range_stop_pres = 0;
    jread_req.list_of_var_pres = 0;
    jread_req.sa_entry_pres = 0;
    ret = mvla_jread (clientNetInfo, &jread_req, &reqCtrl);
    if (ret == SD_SUCCESS)
        ret = waitReqDone (reqCtrl, timeOut);
    if (ret != SD_SUCCESS)
        printf ("\n mvl_jread () Error, ret = 0x%X.", ret);
    else
        {
        printf ("\n mvl_jread OK");
        jread_resp = reqCtrl->u.jread.resp_info;
        printf ("\n   num_entry = %d", jread_resp->num_of_jou_entry);
        printf ("\n   more_follows = %d", jread_resp->more_follows);
        for (i = 0; i < jread_resp->num_of_jou_entry; i++)
            {
            jou_entry = &jread_resp->jou_entry[i];
            printf ("\n   Journal Entry # %d:", i);
            printf ("\n     entry_id_len = %d", jou_entry->entry_id_len);
            printf ("\n     occur_time.form = %s", (jou_entry->occur_time.form == MMS_BTOD6 ? "MMS_BTOD6" : "MMS_BTOD4"));
            printf ("\n     occur_time.ms  = %ld", (ST_LONG)jou_entry->occur_time.ms);
            printf ("\n     occur_time.day = %ld", (ST_LONG)jou_entry->occur_time.day);
            printf ("\n     entry_form_tag = %d", jou_entry->entry_form_tag);
            if (jou_entry->entry_form_tag == 2)
                {
                if (jou_entry->ef.data.list_of_var_pres)
                    {
                    for (j = 0; j < jou_entry->ef.data.num_of_var; j++)
                        {
                        printf ("\n       Var # %d: var_tag = %s", j,
                                jou_entry->ef.data.list_of_var[j].var_tag);
                        printf ("\n       Var # %d: value_spec.len = %d", j,
                                jou_entry->ef.data.list_of_var[j].value_spec.len);
                        }
                    }
                }
            else
                {
                printf ("\n       annotation = %s",
                        jou_entry->ef.annotation);
                }
            } /* end "loop"   */
        }
    mvl_free_req_ctrl (reqCtrl);  /* CRITICAL:            */
#endif


#if (MMS_JINIT_EN & REQ_EN)
    printf ("\n\n Testing DOM_SPEC JournalInit");
    jinit_req.jou_name.object_tag = DOM_SPEC;
    jinit_req.jou_name.domain_id = "mvlLiteDom1";
    jinit_req.jou_name.obj_name.vmd_spec = "MyJournal";
    jinit_req.limit_spec_pres = 0;
    jinit_req.limit_entry_pres = 0;
    ret = mvla_jinit (clientNetInfo, &jinit_req, &reqCtrl);
    if (ret == SD_SUCCESS)
        ret = waitReqDone (reqCtrl, timeOut);
    if (ret != SD_SUCCESS)
        printf ("\n mvl_jinit () Error, ret = 0x%X.", ret);
    else
        {
        printf ("\n mvl_jinit OK");
        printf ("\n   del_entries = %lu", (ST_ULONG)reqCtrl->u.jinit.resp_info->del_entries);
        }
    mvl_free_req_ctrl (reqCtrl);
#endif

#if (MMS_JSTAT_EN & REQ_EN)
    printf ("\n\n Testing DOM_SPEC JournalStat");
    jstat_req.jou_name.object_tag = DOM_SPEC;
    jstat_req.jou_name.domain_id = "mvlLiteDom1";
    jstat_req.jou_name.obj_name.vmd_spec = "MyJournal";
    ret = mvla_jstat (clientNetInfo, &jstat_req, &reqCtrl);
    if (ret == SD_SUCCESS)
        ret = waitReqDone (reqCtrl, timeOut);
    if (ret != SD_SUCCESS)
        printf ("\n mvl_jstat () Error, ret = 0x%X.", ret);
    else
        {
        printf ("\n mvl_jstat OK");
        printf ("\n   cur_entries = %lu", (ST_ULONG)reqCtrl->u.jstat.resp_info->cur_entries);
        printf ("\n   mms_deletable = %d", reqCtrl->u.jstat.resp_info->mms_deletable);
        }
    mvl_free_req_ctrl (reqCtrl);
#endif

#if (MMS_JREAD_EN & REQ_EN)
    printf ("\n\n Testing DOM_SPEC JournalRead");
    jread_req.jou_name.object_tag = DOM_SPEC;
    jread_req.jou_name.domain_id = "mvlLiteDom1";
    jread_req.jou_name.obj_name.vmd_spec = "MyJournal";
    jread_req.range_start_pres = 0;
    jread_req.range_stop_pres = 0;
    jread_req.list_of_var_pres = 0;
    jread_req.sa_entry_pres = 0;
    ret = mvla_jread (clientNetInfo, &jread_req, &reqCtrl);
    if (ret == SD_SUCCESS)
        ret = waitReqDone (reqCtrl, timeOut);
    if (ret != SD_SUCCESS)
        printf ("\n mvl_jread () Error, ret = 0x%X.", ret);
    else
        {
        printf ("\n mvl_jread OK");
        jread_resp = reqCtrl->u.jread.resp_info;
        printf ("\n   num_entry = %d", jread_resp->num_of_jou_entry);
        printf ("\n   more_follows = %d", jread_resp->more_follows);
        for (i = 0; i < jread_resp->num_of_jou_entry; i++)
            {
            jou_entry = &jread_resp->jou_entry[i];
            printf ("\n   Journal Entry # %d:", i);
            printf ("\n     entry_id_len = %d", jou_entry->entry_id_len);
            printf ("\n     occur_time.form = %s", (jou_entry->occur_time.form == MMS_BTOD6 ? "MMS_BTOD6" : "MMS_BTOD4"));
            printf ("\n     occur_time.ms  = %ld", (ST_LONG)jou_entry->occur_time.ms);
            printf ("\n     occur_time.day = %ld", (ST_LONG)jou_entry->occur_time.day);
            printf ("\n     entry_form_tag = %d", jou_entry->entry_form_tag);
            if (jou_entry->entry_form_tag == 2)
                {
                if (jou_entry->ef.data.list_of_var_pres)
                    {
                    for (j = 0; j < jou_entry->ef.data.num_of_var; j++)
                        {
                        printf ("\n       Var # %d: var_tag = %s", j,
                                jou_entry->ef.data.list_of_var[j].var_tag);
                        printf ("\n       Var # %d: value_spec.len = %d", j,
                                jou_entry->ef.data.list_of_var[j].value_spec.len);
                        }
                    }
                }
            else
                {
                printf ("\n       annotation = %s",
                        jou_entry->ef.annotation);
                }
            } /* end "loop"   */
        }
    mvl_free_req_ctrl (reqCtrl);  /* CRITICAL:            */
#endif
    return;
    }
/************************************************************************/
/*                       test_iec_rpt                                   */
/* This function sets up to receive IEC-61850 or UCA reports on this connection*/
/************************************************************************/
RCB_INFO* test_iec_rpt (MVL_NET_INFO* clientNetInfo, RPT_TYPEIDS* rpt_typeids,
                        ST_CHAR* dom_name,  /* domain which contains the RCB*/
                        ST_CHAR* rcb_name)  /* RCB (e.g. "LLN0$BR$PosReport")*/
    {
    ST_RET ret;
    RCB_INFO* rcb_info;             /* UCA/IEC Report Control Block info    */
    ST_UCHAR OptFlds[2];           /* 10 bit bitstring but only allow write of 9 bits*/
    ST_UCHAR TrgOps[1];            /* 8 bit bitstring                      */
    ALL_RCB_INFO* all_rcb_info;

    all_rcb_info = (ALL_RCB_INFO*)clientNetInfo->user_info;

    rcb_info = rcb_info_create (clientNetInfo, dom_name, rcb_name, rpt_typeids, 10);
    if (rcb_info)
        {
        /* Add RCB to list before enabling, so we're ready to receive RPTS immediately.*/
        list_add_last (&all_rcb_info->rcb_info_list, rcb_info);

        printf ("rcb_info->numDsVar = %d\n", rcb_info->numDsVar);
        /* Enable options we want to see in report. */
        OptFlds[0] = 0;
        OptFlds[1] = 0;
        /* These options valid for IEC BRCB, IEC URCB, or UCA.      */
        BSTR_BIT_SET_ON (OptFlds, OPTFLD_BITNUM_SQNUM);
        BSTR_BIT_SET_ON (OptFlds, OPTFLD_BITNUM_TIMESTAMP);
        BSTR_BIT_SET_ON (OptFlds, OPTFLD_BITNUM_REASON);
        BSTR_BIT_SET_ON (OptFlds, OPTFLD_BITNUM_DATSETNAME);
        /* NOTE: these options ONLY available for IEC-61850 (not UCA)
         * SUBSEQNUM is only set by the server, so don't try to set it.
         */
        if (rcb_info->rcb_type == RCB_TYPE_IEC_BRCB)
            { /* These only valid for IEC BRCB        */
            BSTR_BIT_SET_ON (OptFlds, OPTFLD_BITNUM_BUFOVFL);
            BSTR_BIT_SET_ON (OptFlds, OPTFLD_BITNUM_ENTRYID);
            }
        if (rcb_info->rcb_type != RCB_TYPE_UCA)
            { /* These only valid for IEC BRCB or IEC URCB    */
            BSTR_BIT_SET_ON (OptFlds, OPTFLD_BITNUM_DATAREF);
            BSTR_BIT_SET_ON (OptFlds, OPTFLD_BITNUM_CONFREV);
            }

        TrgOps[0] = 0xff;  /* enable ALL triggers  */
        ret = rcb_enable (clientNetInfo, dom_name, rcb_name, OptFlds, TrgOps,
                          5000,             /* Integrity Period (ms)        */
                          rpt_typeids,
                          10);              /* timeout (s)  */
        if (ret)
            {
            printf ("rcb_enable error\n");
            /* Remove this RCB from list and destroy it.      */
            list_unlink (&all_rcb_info->rcb_info_list, rcb_info);
            rcb_info_destroy (rcb_info);
            rcb_info = NULL;
            }
        }
    else
        printf ("rcb_info_create error for dom='%s', rcb='%s'\n", dom_name, rcb_name);
    return (rcb_info);
    }
/************************************************************************/
/*                              doCommService                           */
/************************************************************************/

ST_VOID doCommService ()
    {
    ST_BOOLEAN event;
    wait_any_event (1000);      /* Wait 1000 milliseconds               */
    do
        {
        /* CRITICAL: do like this so both functions called each time through loop.*/
        event = mvl_comm_serve ();       /* Perform communications service       */
#if  defined(GOOSE_RX_SUPP) || defined(GSSE_RX_SUPP) || defined(GSE_MGMT_RX_SUPP) || defined(SMPVAL_RX_SUPP)
    /* NOTE: this is called only if an appropriate protocol is enabled.*/
        event |= subnet_serve ();
#endif
        } while (event);            /* Loop until BOTH functions return 0*/

#if defined(DEBUG_SISCO)
        if (sLogCtrl->logCtrl & LOG_IPC_EN)
            slogIpcEvent ();  /* required for IPC Logging if gensock2.c is    */
          /* not compiled with GENSOCK_THREAD_SUPPORT   */

          /* At runtime, periodically need to service SLOG commands and calling connections. */
          /* The timing of this service is not critical, but to be responsive a default of    */
          /* 100ms works well.                                                                */
        slog_ipc_std_cmd_service ("logcfg.xml", NULL, NULL, SD_TRUE, NULL, NULL);
#endif

#if defined(SUBNET_THREADS)
        /* This code is purely for demonstration purposes. No threads are       */
        /* created, but this shows what could be done in a separate thread.     */
        /* You could also have a separate thread to process each list.          */
        /* This just processes all packets on 4 separate lists.                 */
        {
        SN_UNITDATA* sn_udt;
        N_UNITDATA* n_udt;
#if defined(GOOSE_RX_SUPP)
        while ((sn_udt = goose_ind_get ()) != NULL)
            {
            goose_ind_process (sn_udt);       /* process it   */
            chk_free (sn_udt);                /* free it      */
            }
#endif
#if defined(SMPVAL_RX_SUPP)
        while ((sn_udt = smpval_ind_get ()) != NULL)
            {
            smpval_ind_process (sn_udt);      /* process it   */
            chk_free (sn_udt);                /* free it      */
            }
#endif
#if defined(GSE_MGMT_RX_SUPP)
        while ((sn_udt = gse_mgmt_ind_get ()) != NULL)
            {
            gse_mgmt_ind_process (sn_udt);    /* process it   */
            chk_free (sn_udt);                /* free it      */
            }
#endif
#if defined(GSSE_RX_SUPP)
        while ((n_udt = cltp_ind_get ()) != NULL)
            {
            cltp_ind_process (n_udt);         /* process it   */
            clnp_free (n_udt);                /* free it      */
            }
#endif
        }
#endif  /* SUBNET_THREADS       */
    }

/************************************************************************/
/*                              ctrlCfun                                */
/************************************************************************/
/* This function handles the ^c, and allows us to cleanup on exit       */

void ctrlCfun (int i)
    {
    doIt = SD_FALSE;
    }

/************************************************************************/
/*                      kbService ()                                    */
/************************************************************************/

/* NOTE for some UNIX systems:                                          */
/*      This function, if enebled, needs ukey.c to be linked into the   */
/*      project (for the kbhit function).                               */
static ST_VOID kbService (ST_VOID)
    {
#if !defined(NO_KEYBOARD)
    ST_CHAR c;

    if (kbhit ())         /* Report test keyboard input */
        {
#if defined(_WIN32) || (defined(__QNX__) && !defined(__QNXNTO__))
        c = getch ();
#else
        c = (ST_CHAR)getchar ();   /* works only if term_init called first */
#endif

        if (c == 'x')
            doIt = SD_FALSE;  /* This triggers graceful exit  */

        if (c == '?')
            {
#if defined(S_SEC_ENABLED)
            printf ("\n u : Update Security Configuration");
#endif
            printf ("\n x : Exit");
            }
        }
#endif  /* !defined(NO_KEYBOARD)        */
    }

/************************************************************************/
/*                      connectToServer                                 */
/************************************************************************/
MVL_NET_INFO* connectToServer (ST_CHAR* clientARName, ST_CHAR* serverARName)
#if defined(S_SEC_ENABLED)
    {
    ST_RET ret;
    INIT_INFO callingInitInfo;
    INIT_INFO initRespInfo;
    MVL_REQ_PEND* reqCtrl;
    MVL_NET_INFO* clientNetInfo = NULL;
    ACSE_AUTH_INFO      authInfoStr = { 0 };
    S_SEC_ENCRYPT_CTRL  encryptCtrlStr = { 0 };
    S_SEC_LOC_REM_AR* loc_ar_sec = NULL;
    S_SEC_LOC_REM_AR* rem_ar_sec = NULL;
    S_CERT_CTRL* locMaceCert = NULL;

    ACSE_AUTH_INFO* authInfo = NULL;    /* conn authentication info     */
    S_SEC_ENCRYPT_CTRL* encryptCtrl = NULL; /* conn enctryption info        */

    callingInitInfo.mms_p_context_pres = SD_TRUE;
    callingInitInfo.max_segsize_pres = SD_TRUE;
    callingInitInfo.max_segsize = mvl_cfg_info->max_msg_size;
    callingInitInfo.maxreq_calling = 1;
    /* NOTE: maxreq_called must be > 1 to allow mvla_fget to work.*/
    callingInitInfo.maxreq_called = 2;
    callingInitInfo.max_nest_pres = SD_TRUE;
    callingInitInfo.max_nest = 5;
    callingInitInfo.mms_detail_pres = SD_TRUE;
    callingInitInfo.version = 1;
    callingInitInfo.num_cs_init = 0;
    callingInitInfo.core_position = 0;
    callingInitInfo.param_supp[0] = m_param[0];
    callingInitInfo.param_supp[1] = m_param[1];
    memcpy (callingInitInfo.serv_supp, m_service_resp, 11);

    authInfo = &authInfoStr;
    encryptCtrl = &encryptCtrlStr;
    /* set authentication and encryption infor for this connection */
    /* NOTE: use ulSetSecurityCallingEx if we like to save certStatus */
    ret = ulSetSecurityCalling (clientARName, serverARName,
                                &loc_ar_sec, &rem_ar_sec,
                                authInfo, &locMaceCert, encryptCtrl);
    if (ret != SD_SUCCESS)
        {
        printf ("\n Initialization of security info failed");
        return (NULL);      /* error        */
        }
    ret = mvla_initiate_req_exx (clientARName, serverARName, &callingInitInfo,
                                 &initRespInfo, &clientNetInfo, &reqCtrl,
                                 authInfo, encryptCtrl, locMaceCert);

    if (ret == SD_SUCCESS)
        {
        /* Save the AR Security pointers, for convenience when the confirm is rxd   */
        clientNetInfo->loc_ar_sec = loc_ar_sec;
        clientNetInfo->rem_ar_sec = rem_ar_sec;
        /* Note: content of auth_info saved in ACSE_CONN, do not free here  */
        }
    else
        {
        /* failed to send Initiate Req, must free the data in auth_info here  */
        if (authInfo->auth_pres)
            ulFreeAssocSecurity (authInfo);
        }

    if (ret == SD_SUCCESS)
        {
        ret = waitReqDone (reqCtrl, 10);
        /* CRITICAL: on timeout, must Abort or resources may not be freed.*/
        if (ret == MVL_ERR_USR_TIMEOUT)
            mvl_abort_req (clientNetInfo);
        }

    if (ret != SD_SUCCESS)
        clientNetInfo = NULL;

    mvl_free_req_ctrl (reqCtrl);
    return (clientNetInfo);
    }

#else /* default code */

{
ST_RET ret;
INIT_INFO callingInitInfo;
INIT_INFO initRespInfo;
MVL_REQ_PEND* reqCtrl;
MVL_NET_INFO* clientNetInfo = NULL;
#if defined(ACSE_AUTH_ENABLED)
ACSE_AUTH_INFO      authInfoStr = { 0 };
#endif

ACSE_AUTH_INFO* authInfo = NULL;    /* conn authentication info     */
S_SEC_ENCRYPT_CTRL* encryptCtrl = NULL; /* conn enctryption info        */

callingInitInfo.mms_p_context_pres = SD_TRUE;
callingInitInfo.max_segsize_pres = SD_TRUE;
callingInitInfo.max_segsize = mvl_cfg_info->max_msg_size;
callingInitInfo.maxreq_calling = MAX_CNT_SHORT /*1*/; /*TODO: Arun*/
/* NOTE: maxreq_called must be > 1 to allow mvla_fget to work.*/
callingInitInfo.maxreq_called = MAX_CNT_SHORT /*2*/; /*TODO: Arun*/
callingInitInfo.max_nest_pres = SD_TRUE;
callingInitInfo.max_nest = 5; /*TODO: Arun test how this plays a role*/
callingInitInfo.mms_detail_pres = SD_TRUE;
callingInitInfo.version = 1;
callingInitInfo.num_cs_init = 0;
callingInitInfo.core_position = 0;
callingInitInfo.param_supp[0] = m_param[0];
callingInitInfo.param_supp[1] = m_param[1];
memcpy (callingInitInfo.serv_supp, m_service_resp, 11);

#if defined(ACSE_AUTH_ENABLED)
/* Fill out an authentication structure */
authInfo = &authInfoStr;
authInfo->auth_pres = SD_TRUE;
authInfo->mech_type = ACSE_AUTH_MECH_PASSWORD;
strcpy (authInfo->u.pw_auth.password, "mypassword");  /* this is the password */

ret = mvla_initiate_req_ex (clientARName, serverARName, &callingInitInfo,
                            &initRespInfo, &clientNetInfo, &reqCtrl,
                            authInfo, encryptCtrl);
#else   /* default */
ret = mvla_initiate_req_ex (clientARName, serverARName, &callingInitInfo,
                            &initRespInfo, &clientNetInfo, &reqCtrl,
                            authInfo, encryptCtrl);
#endif

if (ret == SD_SUCCESS)
    {
    ret = waitReqDone (reqCtrl, 10);
    /* CRITICAL: on timeout, must Abort or resources may not be freed.*/
    if (ret == MVL_ERR_USR_TIMEOUT)
        mvl_abort_req (clientNetInfo);
    }

if (ret != SD_SUCCESS)
clientNetInfo = NULL;

mvl_free_req_ctrl (reqCtrl);
return (clientNetInfo);
}
#endif  /* default code */

/************************************************************************/
/*                      disconnectFromServer                            */
/* Send "Conclude" request and check result.                            */
/************************************************************************/

ST_RET disconnectFromServer (MVL_NET_INFO* clientNetInfo)
    {
    MVL_REQ_PEND* reqCtrl;
    ST_RET retcode;

    mvla_concl (clientNetInfo, &reqCtrl); /* Send "Conclude" request      */
    waitReqDone (reqCtrl, timeOut);
    retcode = reqCtrl->result;    /* save result before freeing reqCtrl   */
    mvl_free_req_ctrl (reqCtrl);
    if (retcode)
        printf ("Conclude request failed: error=0x%04X\n", retcode);

    return (retcode);
    }

/************************************************************************/
/*                      u_mvl_info_rpt_ind                              */
/* Processes a CommandTermination report, or LastApplError report, or   */
/* "normal" IEC 61850 Report (based on RCB).                            */
/* Saves LastApplError data in my_control_info.LastApplError.           */
/* If report is CommandTermination, also sets my_control_info.cmd_term_num_va.*/
/************************************************************************/
ST_VOID u_mvl_info_rpt_ind (MVL_COMM_EVENT* event)
    {
    INFO_REQ_INFO* info_ptr;
    ST_INT j;
    OBJECT_NAME* vobj;
    VARIABLE_LIST* vl;
    VAR_ACC_SPEC* va_spec;
    ST_INT num_va;
    ST_CHAR* name;
    ACCESS_RESULT* ar_ptr;
    RUNTIME_TYPE* rt_first;
    ST_INT rt_num;
    ST_RET retcode;

    /* First, check for IEC 61850 CommandTermination message.  */
    info_ptr = (INFO_REQ_INFO*)event->u.mms.dec_rslt.data_ptr;
    va_spec = &info_ptr->va_spec;
    num_va = info_ptr->num_of_acc_result;
    ar_ptr = info_ptr->acc_rslt_list;

    if (va_spec->var_acc_tag == VAR_ACC_VARLIST)
        {   /* Report contains List of Variables  */
        vl = (VARIABLE_LIST*)(info_ptr + 1);
        ar_ptr = info_ptr->acc_rslt_list;
        /* Loop through variables and look for "my_control_info.oper_name" or "LastApplError".*/
        for (j = 0; j < num_va; ++j, ++vl, ++ar_ptr)
            {
            if (vl->var_spec.var_spec_tag == VA_SPEC_NAMED)
                {
                vobj = &vl->var_spec.vs.name;
                name = vobj->obj_name.vmd_spec;
                if (!strcmp (name, my_control_info.oper_name) && vobj->object_tag == DOM_SPEC)
                    {
                    /* This is IEC 61850 CommandTermination. Save num of vars.	*/
                    my_control_info.cmd_term_num_va = num_va;
                    }
                else if (!strcmp (name, "LastApplError") && vobj->object_tag == VMD_SPEC)
                    {
                    /* If 1 var in rpt, this is just LastApplError report. */
                    /* If 2 var in rpt, this is CommandTermination.        */
                    /* Either way, decode and save data in "my_control_info.LastApplError".*/
                    retcode = SD_FAILURE;  /* assume decode fails  */
                    if (ar_ptr->acc_rslt_tag == ACC_RSLT_SUCCESS)
                        {
                        retcode = mvl_get_runtime (LastApplErrorTypeId, &rt_first, &rt_num);
                        if (retcode == SD_SUCCESS)
                            {
                            retcode = ms_asn1_to_local (rt_first,
                                                        rt_num,
                                                        ar_ptr->va_data.data,
                                                        ar_ptr->va_data.len,
                                                        (ST_CHAR*)& my_control_info.LastApplError);
                            if (retcode == SD_SUCCESS)
                                {
                                my_control_info.cmd_term_num_va = num_va;
                                }
                            }
                        }
                    if (retcode)
                        { /* Could not decode. Log error and clear "LastApplError". */
                        MVL_LOG_ERR0 ("LastApplError could not be decoded");
                        memset (&my_control_info.LastApplError, 0, sizeof (MVL61850_LAST_APPL_ERROR));
                        }
                    }
                }
            else
                {
                MVL_LOG_NERR0 ("InformationReport.Ind : Variable Spec not named. Report ignored.");
                }
            }
        }
    else
        {   /* Report contains Named Variable List */
        /* Assume it is IEC 61850 report (controlled by RCB) and process as such.*/
        if (u_iec_rpt_ind (event) != SD_SUCCESS)
            SLOGALWAYS0 ("ERROR: received report is not a valid IEC 61850 report.");
        }
    }


/************************************************************************/
/*                      u_mvl_connect_ind_ex                            */
/*----------------------------------------------------------------------*/
/* This function is called when a remote node has connected to us. We   */
/* can look at the assReqInfo to see who it is (assuming that the       */
/* AP-Titles are used), or at the cc->rem_init_info to see initiate     */
/* request parameters.                                                  */
/************************************************************************/
extern ST_ACSE_AUTH u_mvl_connect_ind_ex (MVL_NET_INFO* cc, INIT_INFO* init_info,
                                          ACSE_AUTH_INFO* req_auth_info, ACSE_AUTH_INFO* rsp_auth_info)
    {
    ST_ACSE_AUTH ret = ACSE_AUTH_SUCCESS;

#if defined(S_SEC_ENABLED)
    /* this Client will reject the connection since there is no security  */
    /* checking code in place (see Server for sample)                     */
    ret = ACSE_DIAG_NO_REASON;
#elif defined(ACSE_AUTH_ENABLED)
    /* this Client will reject the connection since there is no password  */
    /* checking code in place (see Server for sample)                     */
    ret = ACSE_DIAG_NO_REASON;
#endif

    return (ret);
    }

/************************************************************************/
/*                      u_mvl_connect_cnf_ex                            */
/*----------------------------------------------------------------------*/
/* This function is called when we have received an initiate response.  */
/* Depending on the server's authentication scheme, we may have been    */
/* sent responding authentication that we can pull out of the response  */
/* PDU.                                                                 */
/************************************************************************/

ST_ACSE_AUTH u_mvl_connect_cnf_ex (MVL_NET_INFO* cc, AARE_APDU* ass_rsp_info)
    {
    ST_RET ret = ACSE_AUTH_SUCCESS;

#if defined(S_SEC_ENABLED)
    S_SEC_AUTHCHK_RSLT sSecAuthChk;

    /* check security parameters of the Associate Confirm                 */
    ret = ulCheckSecureAssocConf (cc, &sSecAuthChk);
    /* user can examine the sSecAuthChk */
#elif defined(ACSE_AUTH_ENABLED)
    {
    ACSE_AUTH_INFO* rsp_auth_info;

    rsp_auth_info = &ass_rsp_info->auth_info;


    if (rsp_auth_info->auth_pres == SD_TRUE)
        {
        /* Looks like we have some authentication to look at, simply print  */
        /* the password and continue as normal.                             */
        if (rsp_auth_info->mech_type == ACSE_AUTH_MECH_PASSWORD)
            {
            printf ("\nPassword recieved from peer: %s\n", rsp_auth_info->u.pw_auth.password);
            }
        /* We were sent a mechanism we don't support, let's reject the      */
        /* the connection with the appropriate diagnostic.                  */
        else
            {
            ret = ACSE_DIAG_AUTH_MECH_NAME_NOT_RECOGNIZED;
            }
        }
    else
        {
        /* Hmm... looks like we weren't sent any authentication even though */
        /* we require it. Let's reject with the appropriate diagnostic.     */
        ret = ACSE_DIAG_AUTH_REQUIRED;
        }
    }
#endif /* defined(ACSE_AUTH_ENABLED) */

    /* to accept the confirm ACSE_AUTH_SUCCESS need to be returned        */
    return ((ST_ACSE_AUTH)ret);
    }


/************************************************************************/
/*                      disc_ind_fun                                    */
/************************************************************************/
/* This function is called when connection is terminated.               */

static ST_VOID disc_ind_fun (MVL_NET_INFO* cc, ST_INT discType)
    {
    ALL_RCB_INFO* all_rcb_info;
    RCB_INFO* rcb_info;

    printf ("disconnect indication received.\n");
    /* Free up rcb_info if necessary.     */
    if (cc->user_info)
        {
        all_rcb_info = cc->user_info;
        while ((rcb_info = list_get_first (&all_rcb_info->rcb_info_list)) != NULL)
            rcb_info_destroy (rcb_info);
        chk_free (all_rcb_info);
        cc->user_info = NULL;
        }

    cc->rem_vmd = NULL;
    }

#if (MMS_IDENT_EN & RESP_EN)
/************************************************************************/
/*                      u_mvl_ident_ind                                 */
/************************************************************************/
ST_VOID u_mvl_ident_ind (MVL_IND_PEND* indCtrl)
    {
#if 0   /* enable this to print when Ind received               */
    printf ("\n Identify Indication received.");
#endif
    indCtrl->u.ident.resp_info = &identify_response_info;
    mplas_ident_resp (indCtrl);
    }
#endif  /* #if (MMS_IDENT_EN & RESP_EN) */

/************************************************************************/
/*                      getFile                                         */
/************************************************************************/
static ST_RET getFile (MVL_NET_INFO* clientNetInfo, ST_CHAR* loc_file, ST_CHAR* rem_file)
    {
    FREAD_REQ_INFO   fread_req_info;
    FCLOSE_REQ_INFO  fclose_req_info;
    ST_RET ret;
    FILE* loc_fp;
    MVL_REQ_PEND* reqCtrl;
    ST_INT32 frsmid;
    ST_BOOLEAN more_follows;

    ret = mvla_fopen (clientNetInfo,
                      rem_file,
                      0,           /* init_pos: start at beginning of file */
                      &reqCtrl);
    if (ret == SD_SUCCESS)
        ret = waitReqDone (reqCtrl, timeOut);

    if (ret == SD_SUCCESS)
        frsmid = reqCtrl->u.fopen.resp_info->frsmid;        /* save frsmid  */
    mvl_free_req_ctrl (reqCtrl);
    if (ret != SD_SUCCESS)
        return (ret);

    loc_fp = fopen (loc_file, "wb");
    if (loc_fp == NULL)
        return (SD_FAILURE);        /* Can't open local file.       */

    fread_req_info.frsmid = frsmid;       /* "fread" request doesn't chg. */
    do
        {
        ret = mvla_fread (clientNetInfo, &fread_req_info, &reqCtrl);
        if (ret == SD_SUCCESS)
            ret = waitReqDone (reqCtrl, timeOut);
        if (ret == SD_SUCCESS)
            {
            more_follows = reqCtrl->u.fread.resp_info->more_follows;
            fwrite (reqCtrl->u.fread.resp_info->filedata, 1,
                    reqCtrl->u.fread.resp_info->fd_len, loc_fp);
            }
        mvl_free_req_ctrl (reqCtrl);
        if (ret != SD_SUCCESS)
            {
            fclose (loc_fp);
            return (ret);
            }
        } while (more_follows);

        fclose_req_info.frsmid = frsmid;
        ret = mvla_fclose (clientNetInfo, &fclose_req_info, &reqCtrl);
        if (ret == SD_SUCCESS)
            ret = waitReqDone (reqCtrl, timeOut);
        if (ret != SD_SUCCESS)
            printf ("\n mvl_fclose failed, ret = 0x%X", ret);
        mvl_free_req_ctrl (reqCtrl);

        fclose (loc_fp);
        return (SD_SUCCESS);
    }

/************************************************************************/
/*                              init_log_cfg                            */
/************************************************************************/

ST_VOID init_log_cfg (ST_VOID)
    {
#if defined(DEBUG_SISCO)

#if defined (HARD_CODED_CFG)
    /* Use File logging                                                     */
    sLogCtrl->logCtrl = LOG_FILE_EN;

    /* Use time/date time log                                               */
    sLogCtrl->logCtrl |= LOG_TIME_EN;

    /* File Logging Control defaults                                        */
    sLogCtrl->fc.fileName = "client.log";
    sLogCtrl->fc.maxSize = 1000000L;
    sLogCtrl->fc.ctrl = (FIL_CTRL_WIPE_EN |
                         FIL_CTRL_WRAP_EN |
                         FIL_CTRL_NO_APPEND |
                         FIL_CTRL_MSG_HDR_EN);

    mms_debug_sel |= MMS_LOG_NERR;

    mvl_debug_sel |= MVLLOG_ERR;
    mvl_debug_sel |= MVLLOG_NERR;
#if 0
    mvl_debug_sel |= MVLLOG_ACSE;
    mvl_debug_sel |= MVLLOG_ACSEDATA;
    mvl_debug_sel |= MVLLOG_TIMING;
#endif

    acse_debug_sel |= ACSE_LOG_ERR;
#if 0
    acse_debug_sel |= ACSE_LOG_ENC;
    acse_debug_sel |= ACSE_LOG_DEC;
    acse_debug_sel |= COPP_LOG_ERR;
    acse_debug_sel |= COPP_LOG_DEC;
    acse_debug_sel |= COPP_LOG_DEC_HEX;
    acse_debug_sel |= COPP_LOG_ENC;
    acse_debug_sel |= COPP_LOG_ENC_HEX;
    acse_debug_sel |= COSP_LOG_ERR;
    acse_debug_sel |= COSP_LOG_DEC;
    acse_debug_sel |= COSP_LOG_DEC_HEX;
    acse_debug_sel |= COSP_LOG_ENC;
    acse_debug_sel |= COSP_LOG_ENC_HEX;
#endif


    tp4_debug_sel |= TP4_LOG_ERR;
#if 0
    tp4_debug_sel |= TP4_LOG_FLOWUP;
    tp4_debug_sel |= TP4_LOG_FLOWDOWN;
#endif

#if !defined(REDUCED_STACK)
    clnp_debug_sel |= CLNP_LOG_ERR;
    clnp_debug_sel |= CLNP_LOG_NERR;
#if 0
    clnp_debug_sel |= CLNP_LOG_REQ;
    clnp_debug_sel |= CLNP_LOG_IND;
    clnp_debug_sel |= CLNP_LOG_ENC_DEC;
    clnp_debug_sel |= CLNP_LOG_LLC_ENC_DEC;
    clnp_debug_sel |= CLSNS_LOG_REQ;
    clnp_debug_sel |= CLSNS_LOG_IND;
#endif
#endif

#else   /* !defined (HARD_CODED_CFG)    */

#if defined(SISCO_STACK_CFG)
    logCfgAddMaskGroup (&sscLogMaskMapCtrl);
#endif
#if defined(S_SEC_ENABLED)
    logCfgAddMaskGroup (&secLogMaskMapCtrl);
    logCfgAddMaskGroup (&ssleLogMaskMapCtrl);
    logCfgAddMaskGroup (&ssleASN1LogMaskMapCtrl); /* in SSL Engine */
#endif
    logCfgAddMaskGroup (&mvlLogMaskMapCtrl);
    logCfgAddMaskGroup (&mmsLogMaskMapCtrl);
    logCfgAddMaskGroup (&acseLogMaskMapCtrl);
    logCfgAddMaskGroup (&tp4LogMaskMapCtrl);
    logCfgAddMaskGroup (&clnpLogMaskMapCtrl);
    logCfgAddMaskGroup (&asn1LogMaskMapCtrl);
    logCfgAddMaskGroup (&sxLogMaskMapCtrl);
#if defined(S_MT_SUPPORT)
    logCfgAddMaskGroup (&gsLogMaskMapCtrl);
#endif
    logCfgAddMaskGroup (&sockLogMaskMapCtrl);
#if defined(SNAP_LITE_THR)
    logCfgAddMaskGroup (&snapLogMaskMapCtrl);
#endif
    logCfgAddMaskGroup (&ipcLogMaskMapCtrl);
    logCfgAddMaskGroup (&memLogMaskMapCtrl);
    logCfgAddMaskGroup (&memDebugMapCtrl);

    /* At initialization, install a SLOGIPC command handler. The      */
    /* build in SLOGIPC handler just receives the command and put's   */
    /* on a list to be handled by the application at it's leisure ... */
    sLogCtrl->ipc.slog_ipc_cmd_fun = slog_ipc_std_cmd_fun;

    if (logcfgx_ex (sLogCtrl, "logcfg.xml", NULL, SD_FALSE, SD_FALSE) != SD_SUCCESS)
        {
        printf ("\n Parsing of 'logging' configuration file failed.");
        if (sLogCtrl->fc.fileName)
            printf ("\n Check log file '%s'.", sLogCtrl->fc.fileName);
        exit (5);
        }

    mms_debug_sel = 0;
    tp4_debug_sel = 0;
    clnp_debug_sel = 0;
#endif  /* !defined (HARD_CODED_CFG)    */

    slog_start (sLogCtrl, MAX_LOG_SIZE);  /* call after logging parameters are configured */

#endif  /* DEBUG_SISCO */
    }

/************************************************************************/
/*                              init_mem                                */
/************************************************************************/

static ST_VOID mem_chk_error_detected (ST_VOID);
static ST_VOID* my_malloc_err (ST_UINT size);
static ST_VOID* my_calloc_err (ST_UINT num, ST_UINT size);
static ST_VOID* my_realloc_err (ST_VOID* old, ST_UINT size);

static ST_CHAR* spareMem;

ST_VOID init_mem ()
    {
    /* Allocate spare memory to allow logging/printing memory errors        */
    spareMem = (ST_CHAR*)malloc (500);

    /* trap mem_chk errors                                                  */
    mem_chk_err = mem_chk_error_detected;
    m_memerr_fun = my_malloc_err;
    c_memerr_fun = my_calloc_err;
    r_memerr_fun = my_realloc_err;

#if 0
    m_heap_check_enable = SD_TRUE;
    m_check_list_enable = SD_TRUE;
    m_no_realloc_smaller = SD_TRUE;
    m_fill_en = SD_TRUE;
#endif
    m_mem_debug = SD_TRUE;
    }

/************************************************************************/
/* This function is called from the DEBUG version of the mem library    */
/* when an error of any type is detected.                               */
/************************************************************************/

static ST_INT memErrDetected;
extern void pClientLog (char* text, char* caption, int code);/*TODO: Arun*/

static ST_VOID mem_chk_error_detected (ST_VOID)
    {
    if (!memErrDetected)
        {
        free (spareMem);
        memErrDetected = SD_TRUE;
        printf ("\n Memory Error Detected!");
        pClientLog ("MEM CHK ERROR", "ERROR", -1); /*TODO: Arun*/
        dyn_mem_ptr_status ();
        }
    }

/************************************************************************/
/* Memory Allocation Error Handling Functions.                          */
/* These functions are called from mem_chk when it is unable to         */
/* perform the requested operation. These functions must either return  */
/* a valid buffer or not return at all.                                 */
/************************************************************************/

static ST_VOID* my_malloc_err (ST_UINT size)
    {
    mem_chk_error_detected ();
    printf ("\n Malloc");
    exit (2);
    return (NULL);
    }

static ST_VOID* my_calloc_err (ST_UINT num, ST_UINT size)
    {
    mem_chk_error_detected ();
    exit (3);
    return (NULL);
    }

static ST_VOID* my_realloc_err (ST_VOID* old, ST_UINT size)
    {
    mem_chk_error_detected ();
    exit (4);
    return (NULL);
    }

/************************************************************************/
/*                      waitReqDone                                     */
/*----------------------------------------------------------------------*/
/* Wait for request to complete. Service communications while waiting.  */
/* Check for timeout or Ctrl-C (i.e. doIt=SD_FALSE).                    */
/* NOTE: if other processing should be done while waiting, add it to    */
/*       this function.                                                 */
/* Parameters:                                                          */
/*      req             request to wait for.                            */
/*      timeout         time to wait (seconds). 0 means wait forever.   */
/* RETURNS: SD_SUCCESS, MVL_ERR_USR_TIMEOUT or other MVL error code.    */
/************************************************************************/
ST_RET waitReqDone (MVL_REQ_PEND* req, ST_INT timeout)
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
/************************************************************************/
/*                       named_var_read                                 */
/* Read a single named variable.                                        */
/*   scope = VMD_SPEC, DOM_SPEC, or AA_SPEC                             */
/* CRITICAL: the local variable "parse_info" is written indirectly      */
/*           from waitReqDone when the response is received. The call   */
/*           to "waitReqDone" MUST NOT be moved outside this function.  */
/************************************************************************/
ST_RET named_var_read (MVL_NET_INFO* net_info, ST_CHAR* varName,
                       ST_INT scope, ST_CHAR* domName,
                       MVL_TYPE_ID type_id, ST_VOID* dataDest, ST_INT timeOut)
    {
    ST_INT num_data;
    MVL_READ_RESP_PARSE_INFO parse_info;
    READ_REQ_INFO* req_info;
    VARIABLE_LIST* vl;
    ST_INT ret;
    MVL_REQ_PEND* reqCtrl;

    num_data = 1;   /* this function always reads exactly 1 variable   */

    /* CRITICAL: Must always allocate space for READ_REQ_INFO +   */
    /*           array of VARIABLE_LIST                           */
    /*           (see "read_req_info" definition in the Reference Manual).*/
    /*           Memory corruption may occur if this allocation is too small.*/
    req_info = (READ_REQ_INFO*)chk_calloc (1, sizeof (READ_REQ_INFO) +
                                           num_data * sizeof (VARIABLE_LIST));
    req_info->spec_in_result = SD_FALSE;
    req_info->va_spec.var_acc_tag = VAR_ACC_VARLIST;
    req_info->va_spec.num_of_variables = 1;
    vl = (VARIABLE_LIST*)(req_info + 1);

    vl->alt_access_pres = SD_FALSE;
    vl->var_spec.var_spec_tag = VA_SPEC_NAMED;
    vl->var_spec.vs.name.object_tag = scope;      /* set scope    */
    if (scope == DOM_SPEC)
        vl->var_spec.vs.name.domain_id = domName;   /* set domain name      */
    vl->var_spec.vs.name.obj_name.vmd_spec = varName;

    /* IMPORTANT: start with clean structure, then set appropriate elements.*/
    memset (&parse_info, 0, sizeof (parse_info));
    parse_info.dest = dataDest;
    parse_info.type_id = type_id;
    parse_info.descr_arr = SD_FALSE;

    /* Send read request. */
    ret = mvla_read_variables (net_info, req_info, num_data,
                               &parse_info, &reqCtrl);

    /* If request sent successfully, wait for reply.      */
    if (ret == SD_SUCCESS)
        ret = waitReqDone (reqCtrl, timeOut);

    chk_free (req_info);
    mvl_free_req_ctrl (reqCtrl);
    if (ret != SD_SUCCESS)
        return (ret);

    return (parse_info.result);   /* Return the single variable result    */
    }
/************************************************************************/
/*                      named_var_write                                 */
/* Write a single named variable.                                       */
/*   scope = VMD_SPEC, DOM_SPEC, or AA_SPEC                             */
/* CRITICAL: the local variable "wr_info" is written indirectly         */
/*           from waitReqDone when the response is received. The call   */
/*           to "waitReqDone" MUST NOT be moved outside this function.  */
/************************************************************************/
ST_RET named_var_write (MVL_NET_INFO* netInfo, ST_CHAR* varName,
                        ST_INT scope, ST_CHAR* domName,
                        MVL_TYPE_ID type_id, ST_VOID* dataSrc, ST_INT timeOut)
    {
    MVL_WRITE_REQ_INFO* wr_info;
    WRITE_REQ_INFO* req_info;
    VARIABLE_LIST* vl;
    ST_RET ret;
    MVL_REQ_PEND* reqCtrl;
    ST_INT num_data;

    num_data = 1;   /* this function always writes exactly 1 variable   */

    /* CRITICAL: Must always allocate space for WRITE_REQ_INFO +  */
    /*           array of VARIABLE_LIST + array of VAR_ACC_DATA   */
    /*           (see "write_req_info" definition in the Reference Manual).*/
    /*           Memory corruption may occur if this allocation is too small.*/
    req_info = (WRITE_REQ_INFO*)chk_calloc (1, sizeof (WRITE_REQ_INFO) +
                                            num_data * sizeof (VARIABLE_LIST) +
                                            num_data * sizeof (VAR_ACC_DATA));
    req_info->num_of_data = 1;
    req_info->va_spec.var_acc_tag = VAR_ACC_VARLIST;
    req_info->va_spec.num_of_variables = 1;
    vl = (VARIABLE_LIST*)(req_info + 1);

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
        ret = wr_info->result;      /* return single variable result        */
    chk_free (wr_info);

    return (ret);
    }

/************************************************************************/
/* This function processes the variable data according to the type      */
/* defined in var->type_id. The function pointer table                  */
/* "&mlog_arb_log_ctrl" passed to sxd_process_arb_data causes it        */
/* to just log the data (see mlogavar.c).                               */
/* NOTE: User may pass a different function pointer table to            */
/*       "sxd_process..." to perform different processing.              */
/************************************************************************/
static ST_VOID process_var_data (MVL_VAR_ASSOC* var, OBJECT_NAME* varObjName)
    {
    MVL_TYPE_CTRL* type_ctrl;

    if (varObjName->object_tag == DOM_SPEC)
        SLOGALWAYS2 ("DATA for variable '%s' in domain '%s':",
                     var->name, varObjName->domain_id);
    else
        SLOGALWAYS1 ("DATA for variable '%s':", var->name);

    type_ctrl = mvl_type_ctrl_find (var->type_id);
    if (type_ctrl)
        {
        sxd_process_arb_data (var->data, type_ctrl->rt, type_ctrl->num_rt,
                              NULL,  /* pointer to any user data     */
                              &mlog_arb_log_ctrl,    /* function ptr table   */
                              NULL); /* pointer to "elPres" array. Always NULL.*/
        }
    else
        SLOGCALWAYS2 ("  ERROR: type_id is invalid for variable '%s' in domain '%s'",
                      var->name, varObjName->domain_id);
    }
/************************************************************************/
/*                      getVarNames                                     */
/* This funct based on getGnlVarNames in mvl_uca.c, but much simpler    */
/* because it doesn't need to handle "ContinueAfter".                   */
/************************************************************************/
#define MAX_NEST_LEVEL  10
ST_VOID getVarNames (MVL_VAR_ASSOC * va,
                     ST_CHAR * allNameBuf,  /* buffer for all names                 */
                     ST_CHAR * *dest,               /* array of ptrs to names (max=numUcaRt)*/
                     RUNTIME_TYPE * ucaRt,  /* array of RUNTIME_TYPE                */
                     ST_INT numUcaRt,      /* num entries in array of RUNTIME_TYPE */
                     ST_INT * numNames)
    {
    ST_INT i;
    ST_INT nameCount;
    ST_INT sortedNum;
    /* prefix len should never reach MAX_IDENT_LEN but could get very close*/
    ST_CHAR namePrefix[MAX_NEST_LEVEL][MAX_IDENT_LEN + 1];
    ST_INT nestLevel;
    ST_INT strLen;
    ST_INT get;
    ST_INT put;
    ST_INT numNewNames;
    ST_BOOLEAN compress;
    ST_CHAR* comp_name;             /* component name               */
    ST_CHAR* currNamePos;           /* current position in buffer   */

    currNamePos = allNameBuf;     /* start out pointing to head of buffer */

    if ((va->flags & MVL_VAR_FLAG_UCA) == 0)
        {
        ++(*numNames);
        dest[0] = va->name;
        return;
        }

    memset (namePrefix, 0, sizeof (namePrefix));
    strcpy (namePrefix[0], va->name);

    /* Put base variable name in first entry.       */
    nameCount = *numNames;
    dest[0] = va->name;
    ++nameCount;

    /* OK, now we start doing the real thing. Derive names for this         */
    /* type, put them into the dest array.                                  */

    compress = SD_FALSE;

    /* This code assumes all UCA vars are structs, so first tag must be RT_STR_START*/
    assert (ucaRt->el_tag == RT_STR_START);

    nestLevel = 0;
    for (i = 0; i < numUcaRt; ++i, ++ucaRt)
        {
        sortedNum = ucaRt->mvluTypeInfo.sortedNum;
        assert (sortedNum < numUcaRt);      /* this should never fail.      */
        comp_name = ms_comp_name_find (ucaRt);
        if (strlen (comp_name))
            {
            /* Chk len is legal BEFORE writing (need room for 2 strings + '$'.*/
            if (strlen (namePrefix[nestLevel]) + strlen (comp_name) + 1 <= MAX_IDENT_LEN)
                {
                sprintf (currNamePos, "%s$%s", namePrefix[nestLevel], comp_name);
                strLen = strlen (currNamePos);
                assert (strLen <= MAX_IDENT_LEN);       /* if this fails, len chk in "if" is wrong*/
                /* NOTE: names not stored in order. "dest" array puts "pointers" in order*/
                dest[sortedNum] = currNamePos;
                currNamePos += strLen + 1;      /* point after name just added  */
                ++nameCount;
                }
            else
                {
                SLOGALWAYS2 ("ERROR: Derived variable name '%s$%s' too long to be stored",
                             namePrefix[nestLevel], comp_name);
                compress = SD_TRUE;
                }
            }

        if (ucaRt->el_tag == RT_STR_START)
            {
            comp_name = ms_comp_name_find (ucaRt);
            if (strlen (comp_name))
                {
                ++nestLevel;
                assert (nestLevel < MAX_NEST_LEVEL);
                assert (nestLevel > 0);
                strcpy (namePrefix[nestLevel], namePrefix[nestLevel - 1]);
                strncat_maxstrlen (namePrefix[nestLevel], "$", MAX_IDENT_LEN);
                strncat_maxstrlen (namePrefix[nestLevel], comp_name, MAX_IDENT_LEN);
                }
            }
        else if (ucaRt->el_tag == RT_STR_END)
            --nestLevel;
        else if (ucaRt->el_tag == RT_ARR_START)
            {                                          /* Skip the array elements */
            /* There is no way to create UCA names for objects inside an      */
            /* array, so skip over the array.                                 */
            i += (ucaRt->u.arr.num_rt_blks + 1);
            ucaRt += (ucaRt->u.arr.num_rt_blks + 1);
            }
        }

    /* DEBUG: at this point, 'nestLevel' should equal (-1). This may      */
    /* sound strange, but nestLevel NOT incremented on first RT_STR_START,*/
    /* so last RT_STR_END makes it -1.                                    */
    assert (nestLevel == -1);

    /* If we had to skip one or more names, we need to eliminate the      */
    /* holes in the name pointer table.                                   */
    if (compress == SD_TRUE)
        {
        get = 0;
        put = 0;
        numNewNames = nameCount - *numNames;
        while (put < numNewNames)
            {
            if (dest[get] != NULL)
                dest[put++] = dest[get];
            ++get;
            }
        }

    *numNames = nameCount;
    }

/************************************************************************/
/*                      named_var_find_read_and_log                     */
/* Find a named variable in the VMD, send a "Read" request to the       */
/* Server. If successful, log the variable's data.                      */
/************************************************************************/
ST_RET named_var_find_read_and_log (MVL_NET_INFO* net_info,
                                    MVL_VMD_CTRL* vmd_ctrl,
                                    OBJECT_NAME* varObjName)
    {
    ST_RET retcode = SD_FAILURE;
    MVL_VAR_ASSOC* var;

    /* Create a temporary variable        */
    var = u_mvl_get_va_aa (vmd_ctrl,
                           MMSOP_INFO_RPT,
                           varObjName,
                           net_info,
                           SD_FALSE,           /* alt_access_pres      */
                           NULL,               /* alt_acc              */
                           NULL);              /* alt_access_done_out  */
    if (var == NULL)
        {   /* Cannot find variable. Log it.        */
        if (varObjName->object_tag == DOM_SPEC)
            SLOGALWAYS2 ("Error: Cannot find variable '%s' in domain '%s'",
                         varObjName->obj_name.item_id, varObjName->domain_id);
        else
            SLOGALWAYS1 ("Error: Cannot find variable '%s'", varObjName->obj_name.item_id);
        }
    else
        {   /* Found the variable. Now read it.     */
        retcode = named_var_read (net_info,
                                  varObjName->obj_name.item_id,
                                  varObjName->object_tag,
                                  varObjName->domain_id,
                                  var->type_id, var->data, timeOut);
        if (varObjName->object_tag == DOM_SPEC)
            SLOGALWAYS2 ("Reading variable '%s' in domain '%s'",
                         varObjName->obj_name.item_id, varObjName->domain_id);
        else
            SLOGALWAYS1 ("Reading variable '%s'",
                         varObjName->obj_name.item_id);
        if (retcode)
            SLOGCALWAYS1 ("RESULT: Error 0x%X", retcode);
        else
            {
            SLOGCALWAYS0 ("RESULT: Success");
            /* log the variable's data        */
            process_var_data (var, varObjName);
            }

        /* Free the tmp var created by u_mvl_get_va_aa      */
        u_mvl_free_va (MMSOP_INFO_RPT, var, net_info);
        }

    return (retcode);
    }
/************************************************************************/
/*                      log_iec_remote_vmd_var_names                    */
/* Log all DOM_SPEC variable names for this VMD.                        */
/* NOTE: This function is for demonstration purposes only.              */
/************************************************************************/
ST_VOID log_iec_remote_vmd_var_names (MVL_VMD_CTRL* vmd_ctrl)
    {
    ST_INT dom_idx, var_idx;

    if (vmd_ctrl == NULL)
        {
        printf ("ERROR: Remote VMD not found.\n");
        return; /* cannot continue  */
        }

    for (dom_idx = 0; dom_idx < vmd_ctrl->num_dom; dom_idx++)
        {
        MVL_DOM_CTRL* dom = vmd_ctrl->dom_tbl[dom_idx];
        SLOGALWAYS2 ("Domain[%d] = %s", dom_idx, dom->name);
        for (var_idx = 0; var_idx < dom->num_var_assoc; var_idx++)
            {
            MVL_VAR_ASSOC* var = dom->var_assoc_tbl[var_idx];
            ST_CHAR** dest;
            ST_RET rc;
            RUNTIME_TYPE* ucaRt;
            ST_INT numUcaRt;
            ST_CHAR* allNameBuf;
            ST_INT numNames = 0;
            ST_INT j;         /* loop counter */
            rc = mvl_get_runtime (var->type_id, &ucaRt, &numUcaRt);
            if (rc == SD_SUCCESS)
                {
                /* Allocate a name buffer and array of pointers to names.       */
                allNameBuf = (ST_CHAR*)chk_calloc (1, numUcaRt * (MAX_IDENT_LEN + 1));
                dest = (ST_CHAR * *)chk_calloc (1, numUcaRt * sizeof (ST_CHAR*));
                getVarNames (var, allNameBuf, dest, ucaRt, numUcaRt, &numNames);
                SLOGCALWAYS2 ("  Variable[%d] = %s (flattened names):", var_idx, var->name);
                for (j = 0; j < numNames; j++)
                    SLOGCALWAYS1 ("    %s", dest[j]);     /* log flattened name   */
                  /* Free the name buffer and array of pointers to names. */
                chk_free (allNameBuf);
                chk_free (dest);
                }
            }
        }
    }

/************************************************************************/
/*                      remote_obj_create                               */
/* Read all object configuration files (especially SCL) and create all  */
/* objects.                                                             */
/************************************************************************/
ST_RET remote_obj_create (MVL_VMD_CTRL* vmd_ctrl,
                          ST_CHAR* scl_filename,
                          ST_CHAR* ied_name,
                          ST_CHAR* access_point_name,
                          SCL_INFO* scl_info)
    {
    ST_RET ret;
    ST_INT brcb_bufsize = 0; /* never used for remote objects        */
    ST_UINT report_scan_rate = 0;   /* never used for remote objects*/

    ret = scl_parse (scl_filename,
                     ied_name,                      /* IED name     */
                     access_point_name,             /* AccessPoint  */
                     scl_info);

    if (ret == SX_ERR_CONVERT)
        printf ("WARNING: SCL parser detected a data conversion error. Ignored.\n");

    /* Create types only if everything successful up to now.      */
    if (ret == SD_SUCCESS || ret == SX_ERR_CONVERT)
        {
        /* This creates types & saves the type_id in each SCL_LN & SCL_LNTYPE.*/
        /* DO NOT call it again with same scl_info.                         */
        /* If it is called again, it will fail, but will not corrupt scl_info.*/
        /* NOTE: arg #4 always SD_FALSE so types not named.         */
        /* NOTE: arg #3 (max_rt_num) should be 0 (i.e. unlimited) to support MHAI.*/
        ret = scl2_datatype_create_all (vmd_ctrl, scl_info, 0, SD_FALSE, NULL);
        }

    /* Create variables only if everything successful up to now.  */
    if (ret == SD_SUCCESS)
        ret = scl2_ld_create_all (vmd_ctrl, scl_info, report_scan_rate, brcb_bufsize,
                                  SD_TRUE); /* create "client" model (no RCB, etc.)*/
    return (ret);
    }
/************************************************************************/
/*                      config_iec_remote_vmd                           */
/************************************************************************/
MVL_VMD_CTRL* config_iec_remote_vmd (
    ST_CHAR* scl_filename,
    ST_CHAR* ied_name,
    ST_CHAR* access_point_name,
    SCL_INFO* remote_scl_info)    /* filled in by this function   */
    {
    MVL_VMD_CTRL* vmd_ctrl;
    ST_RET ret;
    vmd_ctrl = mvl_vmd_create (mvl_max_dyn.doms, mvl_max_dyn.vmd_vars,
                               mvl_max_dyn.vmd_nvls, mvl_max_dyn.journals);

    ret = remote_obj_create (vmd_ctrl,
                             scl_filename,
                             ied_name,
                             access_point_name,
                             remote_scl_info);
    if (ret != SD_SUCCESS)
        {
        printf ("\n DEBUG: ERROR: remote_obj_create FAILED.");
        mvl_vmd_destroy (vmd_ctrl); /* Destroy VMD created above.   */
        vmd_ctrl = NULL;
        }
    return (vmd_ctrl);
    }
/************************************************************************/
/*                      test_iec_remote_vmd                             */
/* Read and log "some" selected remote VMD data (see item_id).          */
/* NOTE: this is for demonstration purposes only. It simply shows how   */
/*       different variables can be read and processed. It looks for    */
/*       common variables in every domain of the VMD, tries to read     */
/*       the variables, and then logs the data.                         */
/************************************************************************/
ST_VOID test_iec_remote_vmd (
    MVL_NET_INFO* net_info,
    MVL_VMD_CTRL* vmd_ctrl)
    {
    ST_INT dom_idx;
    OBJECT_NAME varObjName;
    ST_RET retcode;
    MVL_NVLIST_CTRL* nvl;
    ST_INT nvl_idx;

    if (vmd_ctrl == NULL)
        {
        printf ("ERROR: Remote VMD not found.\n");
        return; /* cannot continue  */
        }

    /* Loop through all Domains in this VMD.      */
    for (dom_idx = 0; dom_idx < vmd_ctrl->num_dom; dom_idx++)
        {
        MVL_DOM_CTRL* dom = vmd_ctrl->dom_tbl[dom_idx];

        /* Try reading top level variable from this domain  */
        varObjName.object_tag = DOM_SPEC;
        varObjName.domain_id = dom->name;
        varObjName.obj_name.item_id = "LLN0";
        /* This funct logs on error so may not need to check return.*/
        retcode = named_var_find_read_and_log (net_info, vmd_ctrl, &varObjName);

        /* Try reading lower level variable from this domain        */
        varObjName.object_tag = DOM_SPEC;
        varObjName.domain_id = dom->name;
        varObjName.obj_name.item_id = "LLN0$DC$NamPlt$vendor";
        retcode = named_var_find_read_and_log (net_info, vmd_ctrl, &varObjName);

        /* Try reading another lower level variable from this domain*/
        varObjName.object_tag = DOM_SPEC;
        varObjName.domain_id = dom->name;
        varObjName.obj_name.item_id = "LLN0$DC$NamPlt$configRev";
        retcode = named_var_find_read_and_log (net_info, vmd_ctrl, &varObjName);

        /* If an expected "writable" NVL is found, try to write it. */
        for (nvl_idx = 0; nvl_idx < dom->num_nvlist; nvl_idx++)
            {
            nvl = dom->nvlist_tbl[nvl_idx];
            if (strcmp (nvl->name, "LLN0$writableDataSet") == 0)
                {
                ST_UINT32 var1 = 11000;
                ST_UINT32 var2 = 12000;
                ST_UINT32 var3 = 13000;
                ST_UINT32 var4 = 14000;
                ST_VOID* ptr_array [] = { &var1, &var2, &var3, &var4 };
                ST_INT  size_array [] = { sizeof (var1), sizeof (var2), sizeof (var3), sizeof (var3) };
                retcode = nvl_write (net_info, vmd_ctrl, nvl->name, DOM_SPEC, dom->name, ptr_array, size_array, 4, 10);
                if (retcode)
                    SLOGALWAYS2 ("Error 0x%X writing NVL='%s'", retcode, nvl->name);
                }
            }

        /* Try reading each NVL in this domain of the remote VMD. */
        for (nvl_idx = 0; nvl_idx < dom->num_nvlist; nvl_idx++)
            {
            nvl = dom->nvlist_tbl[nvl_idx];
            retcode = nvl_read (net_info, vmd_ctrl, nvl->name, DOM_SPEC, dom->name, 10);
            if (retcode)
                SLOGALWAYS2 ("Error 0x%X reading NVL='%s'", retcode, nvl->name);
            }
        }
    }

/************************************************************************/
/*                      u_mvl_scl_set_initial_value                     */
/* Called during SCL processing to convert initial value text to data.  */
/* NOTE: only called if MVL library conversion fails.                   */
/************************************************************************/
ST_RET u_mvl_scl_set_initial_value (SCL2_IV_TRANSLATE_CTRL* translate)
    {
    /* NOTE: do nothing but return SD_SUCCESS so SCL processing continues.*/
    /* Initial values not usable in client application.                   */
    return (SD_SUCCESS);
    }
/************************************************************************/
/*                      u_set_all_leaf_functions                        */
/* Called during SCL processing to set up leaf functions. The client    */
/* does not need any leaf functions, so this function does nothing, but */
/* it must always return SD_SUCCESS, so SCL processing will continue.   */
/************************************************************************/
ST_RET u_set_all_leaf_functions (RUNTIME_CTRL* rt_ctrl, SCL_LNTYPE* scl_lntype)
    {
    return (SD_SUCCESS);  /* always return SD_SUCCESS     */
    }
/************************************************************************/
/* NOTE: the following global variables and functions are needed to     */
/* satisfy the linker when the "mvlu" library is used, but they should  */
/* NEVER be used.                                                       */
/************************************************************************/
MVLU_RD_FUN_INFO mvluRdFunInfoTbl [] =
    {
      {"", NULL}
    };
ST_INT mvluNumRdFunEntries = sizeof (mvluRdFunInfoTbl) / sizeof (MVLU_RD_FUN_INFO);

MVLU_WR_FUN_INFO mvluWrFunInfoTbl [] =
    {
      {"", NULL}
    };
ST_INT mvluNumWrFunEntries = sizeof (mvluWrFunInfoTbl) / sizeof (MVLU_WR_FUN_INFO);

ST_VOID  u_mvl_sbo_operate (MVL_SBO_CTRL* sboSelect,
                            MVLU_WR_VA_CTRL* mvluWrVaCtrl)
    {
    assert (0);   /* should never be called       */
    }

ST_VOID u_mvlu_rpt_time_get (MMS_BTIME6* TimeOfEntry)
    {
    assert (0);   /* should never be called       */
    }

ST_RET u_mvlu_resolve_leaf_ref (ST_CHAR* leafName, ST_INT* setFlagsIo,
                                ST_CHAR* refText, ST_RTREF* refOut)
    {
    assert (0);   /* should never be called       */
    return (SD_FAILURE);
    }

/************************************************************************/
/*                      runTests                                        */
/************************************************************************/
static ST_VOID runTests (MVL_NET_INFO* clientNetInfo,
                         ST_INT test_mode,
                         RPT_TYPEIDS* rpt_typeids,
                         MVL_VMD_CTRL* remote_vmd_ctrl)
    {
    ALL_RCB_INFO* all_rcb_info;
    RCB_INFO* rcb_info;
    MVL_TYPE_ID ModOperTypeId, PosOperTypeId;
    ST_CHAR* LastApplErrorTdl = "{\
    (CntrlObj)Vstring129,\
    (Error)Byte,\
    (Origin){(orCat)Byte,(orIdent)OVstring64},\
    (ctlNum)Ubyte,\
    (AddCause)Byte}";

    /* Alloc, init "all_rcb_info" & save it in "user_info" member of MVL_NET_INFO
     * (needed for TEST_MODE_IEC_RPT; ignored for other tests).
     * When a disconnect occurs, RCB resources must be freed (see "disc_ind_fun").
     */
    all_rcb_info = chk_calloc (1, sizeof (ALL_RCB_INFO));
    all_rcb_info->rpt_typeids = rpt_typeids;
    assert (clientNetInfo->user_info == NULL);  /* make sure user_info not already used*/
    clientNetInfo->user_info = all_rcb_info;    /* save ptr in conn struct      */

    /* Execute Client Services ... */
    switch (test_mode)
        {
            case TEST_MODE_IEC_RPT:
                /* NOTE: Could use mvla_getnam to get domain & RCB name from server.*/
                rcb_info = test_iec_rpt (clientNetInfo, rpt_typeids, "E1Q1SB1C1", "LLN0$BR$PosReport01");

                /* Create 2nd RCB to test multiples.    */
                rcb_info = test_iec_rpt (clientNetInfo, rpt_typeids, "E1Q1SB1C1", "LLN0$RP$MeaReport01");
                break;
            case TEST_MODE_IEC_CONTROL:
                /* Create type for "LastApplError" to use when decoding CommandTermination.*/
                LastApplErrorTypeId = mvl_type_id_create_from_tdl (NULL, LastApplErrorTdl);
                if (LastApplErrorTypeId == MVL_TYPE_ID_INVALID)
                    {
                    printf ("Error creating type for LastApplError. Cannot do controls.\n");
                    break;  /* skip these control tests */
                    }

                /* NOTE: Could use mvla_getnam to get these names from the server.*/
                ModOperTypeId = domvar_type_id_create (clientNetInfo, "E1Q1SB1C1", "CSWI1$CO$Mod$Oper");
                PosOperTypeId = domvar_type_id_create (clientNetInfo, "E1Q1SB1C1", "CSWI1$CO$Pos$Oper");
                /* Run tests with "CSWI1.CO.Mod". */
                printf ("\nCONTROL TEST #1\n");
                test_iec_control (clientNetInfo, rpt_typeids,
                                  "E1Q1SB1C1", "CSWI1$CO$Mod$SBO", "CSWI1$CO$Mod$Oper", ModOperTypeId);
                printf ("\nCONTROL TEST #2\n");
                test_iec_control (clientNetInfo, rpt_typeids,
                                  "E1Q1SB1C1", "CSWI1$CO$Mod$SBO", "CSWI1$CO$Mod$Oper", ModOperTypeId);
                printf ("\nCONTROL TEST #3\n");
                test_iec_control (clientNetInfo, rpt_typeids,
                                  "E1Q1SB1C1", "CSWI1$CO$Mod$SBO", "CSWI1$CO$Mod$Oper", ModOperTypeId);
                printf ("\nCONTROL TEST #4\n");
                test_iec_control (clientNetInfo, rpt_typeids,
                                  "E1Q1SB1C1", "CSWI1$CO$Mod$SBO", "CSWI1$CO$Mod$Oper", ModOperTypeId);
                /* Run tests with "CSWI1.CO.Pos". */
                printf ("\nCONTROL TEST #5\n");
                test_iec_control (clientNetInfo, rpt_typeids,
                                  "E1Q1SB1C1", "CSWI1$CO$Pos$SBO", "CSWI1$CO$Pos$Oper", PosOperTypeId);
                printf ("\nCONTROL TEST #6\n");
                test_iec_control (clientNetInfo, rpt_typeids,
                                  "E1Q1SB1C1", "CSWI1$CO$Pos$SBO", "CSWI1$CO$Pos$Oper", PosOperTypeId);
                printf ("\nCONTROL TEST #7\n");
                test_iec_control (clientNetInfo, rpt_typeids,
                                  "E1Q1SB1C1", "CSWI1$CO$Pos$SBO", "CSWI1$CO$Pos$Oper", PosOperTypeId);
                printf ("\nCONTROL TEST #8\n");
                test_iec_control (clientNetInfo, rpt_typeids,
                                  "E1Q1SB1C1", "CSWI1$CO$Pos$SBO", "CSWI1$CO$Pos$Oper", PosOperTypeId);
                mvl_type_id_destroy (ModOperTypeId);
                mvl_type_id_destroy (PosOperTypeId);
                break;
            case TEST_MODE_IEC_REMOTE:
                printf ("\nREMOTE VMD TEST\n");
                test_iec_remote_vmd (clientNetInfo, remote_vmd_ctrl);
                break;

            default:
                test_simple_requests (clientNetInfo);
                break;
        }
    return;
    }

/************************************************************************/
/*                      sendIdentify                                    */
/************************************************************************/
static ST_VOID sendIdentify (MVL_NET_INFO* clientNetInfo,
                             ST_INT identifyIterations)
    {
    ST_INT i;
    MVL_REQ_PEND* reqCtrl;
    ST_RET ret;

    for (i = 0; i < identifyIterations; i++)
        {
        ret = mvla_identify (clientNetInfo, &reqCtrl);
        if (ret == SD_SUCCESS)
            ret = waitReqDone (reqCtrl, timeOut);
        if (ret == SD_SUCCESS)
            {
            printf (" %d", i);
            }
        else
            printf ("\n mvl_identify () Error, ret = 0x%X.", ret);
        mvl_free_req_ctrl (reqCtrl);
        }

    return;
    }

