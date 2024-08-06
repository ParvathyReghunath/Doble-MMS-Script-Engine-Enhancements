/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,       */
/*              1994 - 2018, All Rights Reserved                        */
/*                                                                      */
/* MODULE NAME : mvl_uca.h                                              */
/* PRODUCT(S)  : MMSEASE-LITE                                           */
/*                                                                      */
/* MODULE DESCRIPTION :                                                 */
/*                                                                      */
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :                            */
/*      NONE                                                            */
/*                                                                      */
/* MODIFICATION LOG :                                                   */
/*  Date     Who   Rev                  Comments                        */
/* --------  ---  ------   -------------------------------------------  */
/* 10/09/18  JRB  MVL61850_BRCB_CTRL: add back "resync" (still needed). */
/* 07/23/18  JRB  Add LTRK support: ltrkInitAll, ltrkUpdate*.           */
/* 03/08/18  JRB  Change u_mvl61850_ctl_oper_begin, u_mvl61850_ctl_oper_end*/
/*                to function pointers and add argument.                */
/*                Add MVL61850_CTL_DIRECT_ENHANCED & related functions. */
/* 01/05/18  JRB  Add mvl61850_rpt_log_service.                         */
/* 12/14/17  JRB  MVL61850_BRCB_CTRL: del "resync" (no longer needed).  */
/* 05/03/17  JRB  Add mvl61850_ctl_sbow_attr_cmp & MVL_SBO_CTRL:sbow_flatname.*/
/* 04/25/17  JRB  Add mvl61850_set_all_t.                               */
/* 04/18/17  JRB  Add mvl61850_change_all_q, "u_mvl61850_q_*".          */
/* 01/09/17  JRB  Add OwnerLen param & "mvl61850_owner_*" functions for IPv6.*/
/* 07/15/16  JRB  Add USE_TYPE_PTR option.                              */
/* 01/18/16  JRB  Add firstScanDone flag to pass Conformance test Rp3.  */
/* 03/07/14  JRB  Add mvl61850_enumval_check function.                  */
/*                MVL61850_BRCB_CTRL: add resync.                       */
/* 01/15/14  JRB  Increase RptID size for Edition 2.                    */
/* 11/13/13  JRB  Add mvl61850_update_all_mod_beh, mvl61850_calc_beh.   */
/*                Add defines MVL61850_MOD_*, LAST_APPL_*, & ADD_CAUSE_*.*/
/* 10/04/13  JRB           Replace lcb_data with lcb_attr (separate ptrs).*/
/*                         mvl61850_log_ctrl_create: del lcb_data arg.  */
/* 08/16/13  JRB           Merge MVLU_RPT_CLIENT into MVLU_RPT_CTRL.    */
/*                         Add clientln_* to MVLU_RPT_CTRL.             */
/* 01/11/12  JRB           MVLU_BASRCB: add ResvTms, Owner.             */
/*                         mvlu_find_component: chg prim_num to (ST_UINT*)*/
/*                         Add "MVL61850_LOG_*" & MVL61850_LCB_DATA.    */
/*                         Add "mvl61850_log_*" functions.              */
/*                         Del mvl61850_mk_dataref, use mvl61850_objref_create*/
/* 05/25/11  JRB           BUFLIST_ENTRY: add mem_size.                 */
/* 03/18/11  JRB           Add mvlu_find_component.                     */
/*                         Add "mvl61850_mkname_sbo*".                  */
/*                         Add MVL61850_SBO_SELECT_TIMEOUT.             */
/*                         Del unneeded ifdefs in MVLU_??_VA_CTRL.      */
/*                         Del obsolete funct.                          */
/* 12/08/10  JRB           Add mvlu_scan_read_lov_start/cleanup         */
/*                         Add mvlu_flat_var_create/destroy             */
/* 01/27/09  JRB     39    Add funct ptrs u_mvl_wr_ind_var_start/end.   */
/* 07/23/08  JRB     38    Move some defs to mvl_defs.h (need earlier). */
/*                         Add mvl61850_objref_create.                  */
/* 06/30/08  JRB     37    MVL_SBO_CTRL: repl cmd_executing w/ ctlState.*/
/*                         Add MVL61850_CTLSTATE_* for setting ctlState.*/
/*                         Increase size of sbo_var for IEC 61850.      */
/*                         MVL61850_BRCB_CTRL: Add lastSentTimeOfEntry. */
/* 05/15/08  JRB     36    Add mvl61850_integrity_timeout for 61850 only*/
/* 05/08/08  JRB     35    Add ctlModel to MVL_SBO_CTRL.                */
/* 05/06/08  JRB     34    Add cmd_executing to MVL_SBO_CTRL.           */
/* 03/03/08  JRB     33    Add mvl61850_ctl_req_done.                   */
/*                         Add sboClass to MVL_SBO_CTRL.                */
/*                         Add MVL61850_SBOCLASS_* defines.             */
/* 11/12/07  MDE     32    Moved MMSOP_RDWR_USR_HANDLED to mms_def2.h   */
/* 03/07/07  JRB     31    Add mvlu_get_leaf_val_int_any.               */
/* 02/26/07  JRB     30    initSboCtrl: add use_ms_timer arg.           */
/*                         MVL_SBO_CTRL: add millisecond timer support. */
/* 02/13/07  JRB     29    mvl61850_get_rcb: del unused net_info arg.   */
/*                         Add scan timers to MVL_RPT_CTRL. Add helper fct*/
/* 11/30/06  JRB     28    Fix *_brcb_entryid_init so user can call it. */
/* 11/21/06  JRB     27    Add mvl61850_ctl_lastapplerror_send.         */
/*                         Move ..LAST_APPL_ERROR def to mvl_defs.h.    */
/* 10/10/06  JRB     26    Add mvlu_set_leaf_param_name2.               */
/* 09/27/06  MDE     25    Added MMSOP_RDWR_USR_HANDLED for IOS         */
/* 08/09/06  JRB     24    MVL61850_BRCB_CTRL, BUFLIST_ENTRY: big changes.*/
/*                         Add mvl61850_create(free)_rpt_ctrl,          */
/*                          mvl61850_rpt_service,                       */
/*                          mvl61850_rpt_dataset_create(destroy).       */
/*                         Chg EntryID storage from ST_INT to Ostring.  */
/*                         Del obsolete MVLU_RPT_VA_CTRL.               */
/*                         Chg/add/del several internal report functions.*/
/*                         Add vstring129 to MVLU_RPT_TYPEIDS.          */
/* 08/09/06  JRB     23    Fix MVL61850_MAX_OBJREF_LEN per Tissue 141.  */
/* 03/27/06  JRB     22    Add more mvlu_get_leaf_* functions.          */
/* 09/12/05  JRB     21    Add mvl61850_ctl_chk_sbow.                   */
/* 07/11/05  JRB     20    Add more "mvl61850_ctl_*" functions.         */
/* 06/27/05  JRB     19    Del mvlu_rpt_nvl_add/destroy.                */
/* 06/27/05  JRB     18    _mvlu_get_rd_rcb: add (MVLU_RPT_CLIENT **) arg.*/
/*                         MVL61850_BRCB_CTRL: Add lastSentEntryID.     */
/* 05/27/05  CRM     17    Add mvlu_rpt_destroy_scan_ctrl.              */
/* 05/09/05  JRB     16    Define MVLU_LEAF_FUN_LOOKUP_ENABLE by default*/
/* 12/09/04  JRB     15    Add "MVL61850_CTLMODEL_*" defines.           */
/*                         Add MVL61850_LAST_APPL.. struct.             */
/*                         Add mvlu_trim_branch_name, mvlu_find_comp_type,*/
/*                          mvlu_get_leaf_val_*.                        */
/*                         Add mvl61850_ctl_chk_sbo, mvl61850_ctl_chk_state.*/
/*                         Add initSboCtrl.                             */
/* 09/20/04  JRB     14    More SBO changes for IEC 61850.              */
/* 08/30/04  JRB     13    Add mvlu_sbo_chk_* prototypes.               */
/* 07/06/04  JRB     12    Add mvlu_rpt_create_scan_ctrl2 proto.        */
/* 06/30/04  JRB     11    Add "prim_num", "prim_offset_base" to        */
/*                         MVLU_RD_VA_CTRL, MVLU_WR_VA_CTRL.            */
/*                         Del global var mvluUseStaticData.            */
/*                         Del SCL parsing functions & related structs: */
/*                          mvl61850_ln_create_start, mvl61850_do_create,*/
/*                          mvl61850_da_create, mvl61850_ln_create_finish,*/
/*                          & mvl61850_ln_destroy. New SCL standard     */
/*                          makes these obsolete.                       */
/* 05/13/04  JRB     10    Add SqNumInt16u to MVLU_BASRCB.              */
/*                         Add define MVL61850_MAX_OBJREF_LEN.  */
/* 04/20/04  JRB     09    Add TRGOPS_BITNUM_* defines.                 */
/* 12/17/03  JRB     08    61850-8-1 FDIS changes:                      */
/*                         MVLU_BASRCB: Add TimeofEntry for 61850 BRCB. */
/*                         Add OPTFLD_BITNUM_CONFREV = 8.               */
/*                         Chg OPTFLD_BITNUM_SUBSEQNUM from 8 to 9.     */
/*                         Increase MVLU_MAX_RPT_OPTS to 11.            */
/* 04/22/03  JRB     07    Add comments to dynamic 61850 type functs.   */
/* 04/17/03  JRB     06    Add mvlu_setup_scan_read.                    */
/* 04/04/03  JRB     05    Fix integrity/GI scan code so multiple       */
/*                         concurrent scans don't corrupt one another.  */
/*                         u_mvlu_rpt_scan_done deleted.                */
/* 03/13/03  JRB     04    Add funcs for 61850 dynamic type creation.   */
/*                         Add 61850 URCB support.                      */
/*                         MVLU_RPT_TYPEIDS: Use clearer member names.  */
/*                         MVLU_RPT_CTRL: Del inclusion_data & all      */
/*                           MVL_VAR_ASSOC vars.                        */
/*                         MVLU_BASRCB: Add "Resv".                     */
/*                         ucaName: Chg from ptr to array.              */
/*                         mvlu_proc_rt_type: Chg to use RUNTIME_CTRL.  */
/*                         TrgOps: Chg to Bvstring (variable len).      */
/*                         MVLU_RPT_CLIENT: add/delete members.         */
/*                         Add mvlu_integrity_timeout funct.            */
/* 12/17/02  JRB     03    Chg prefix to "MVLU_STR:"                    */
/*                         Del all *typeId globals, use MVLU_RPT_TYPEIDS*/
/* 12/14/02  JRB     02    Chg mvlu_create_rpt_ctrl, mvlu_send_report args*/
/*                         Add mvlu_integrity_scan_* functions.         */
/*                         Add callback funct u_mvlu_rpt_time_get.      */
/*                         Chg OptFlds to BVstring9, add "bitnum" defines*/
/*                         Add defs, protos, etc. fof IEC-61850.        */
/*                         Add ConfRev, EntryID to MVLU_BASRCB          */
/*                         Add scan_read_ind to MVLU_RPT_CLIENT         */
/*                         Add MVLU_RPT_TYPEIDS                         */
/*                         Del rptTim_data, rptTim_va, rptTim_typeId    */
/*                         Del outDat_data, outDat_va, outDat_typeId    */
/* 12/10/02  JRB     01    Created from defs moved here from mvl_defs.h */
/************************************************************************/
#ifndef MVL_UCA_INCLUDED
#define MVL_UCA_INCLUDED

#include "mvl_defs.h"   /* This uses many common MVL definitions.       */
#include "goose.h"      /* for GSE_IEC_CTRL     */
#include "smpval.h"     /* for SMPVAL_PUBLISHER */

#ifdef __cplusplus
extern "C" {
#endif

/* Max number of mandatory and optional variables in IEC/UCA report.
 * IEC & UCA share RPT code, so max must accomodate both.
 * Includes RptID, OptFlds, SqNum, RptTim, OutDat (DataSetNa),
 * & InclusionBitstring (plus BufOvfl, SubSeqNum, MoreSegmentsFollow,
 * EntryId, & ConfRev for IEC only).
 */
#define MVLU_MAX_RPT_OPTS       11

/* Defines for IEC 61850 control model. */
#define MVL61850_CTLMODEL_STATUS_ONLY           0
#define MVL61850_CTLMODEL_DIRECT_NORMAL         1
#define MVL61850_CTLMODEL_SBO_NORMAL            2
#define MVL61850_CTLMODEL_DIRECT_ENHANCED       3
#define MVL61850_CTLMODEL_SBO_ENHANCED          4

#define MVL61850_SBOCLASS_OPERATE_ONCE          0
#define MVL61850_SBOCLASS_OPERATE_MANY          1

#define MVL61850_CTLSTATE_UNSELECTED            0       /* SBO only     */
#define MVL61850_CTLSTATE_READY                 1
#define MVL61850_CTLSTATE_WAIT_ACTIVATION_TIME  2
#define MVL61850_CTLSTATE_WAIT_CHANGE           3       /* enhanced only*/

/* Values for Mod$stVal or Beh$stVal. */
#define MVL61850_MOD_ON             1
#define MVL61850_MOD_BLOCKED        2
#define MVL61850_MOD_TEST           3
#define MVL61850_MOD_TEST_BLOCKED   4
#define MVL61850_MOD_OFF            5

/* Defines for LastApplError.Error */
#define LAST_APPL_ERROR_NO_ERROR              0
#define LAST_APPL_ERROR_UNKNOWN               1
#define LAST_APPL_ERROR_TIMEOUT_TEST_NOT_OK   2
#define LAST_APPL_ERROR_OPERATOR_TEST_NOT_OK  3

/* Defines for LastApplError.AddCause (from IEC 61850-8-1 Table 97). */
#define ADD_CAUSE_UNKNOWN                        0
#define ADD_CAUSE_NOT_SUPPORTED                  1
#define ADD_CAUSE_BLOCKED_BY_SWITCHING_HIERARCHY 2
#define ADD_CAUSE_SELECT_FAILED                  3
#define ADD_CAUSE_INVALID_POSITION               4
#define ADD_CAUSE_POSITION_REACHED               5
#define ADD_CAUSE_PARAMETER_CHANGE_IN_EXECUTION  6
#define ADD_CAUSE_STEP_LIMIT                     7
#define ADD_CAUSE_BLOCKED_BY_MODE                8
#define ADD_CAUSE_BLOCKED_BY_PROCESS             9
#define ADD_CAUSE_BLOCKED_BY_INTERLOCKING        10
#define ADD_CAUSE_BLOCKED_BY_SYNCHROCHECK        11
#define ADD_CAUSE_COMMAND_ALREADY_IN_EXECUTION   12
#define ADD_CAUSE_BLOCKED_BY_HEALTH              13
#define ADD_CAUSE_1_OF_N_CONTROL                 14
#define ADD_CAUSE_ABORTION_BY_CANCEL             15
#define ADD_CAUSE_TIME_LIMIT_OVER                16
#define ADD_CAUSE_ABORTION_BY_TRIP               17
#define ADD_CAUSE_OBJECT_NOT_SELECTED            18
#define ADD_CAUSE_OBJECT_ALREADY_SELECTED        19
#define ADD_CAUSE_NO_ACCESS_AUTHORITY            20
#define ADD_CAUSE_ENDED_WITH_OVERSHOOT           21
#define ADD_CAUSE_ABORTION_DUE_TO_DEVIATION      22
#define ADD_CAUSE_ABORTION_BY_COMMUNICATION_LOSS 23
#define ADD_CAUSE_BLOCKED_BY_COMMAND             24
#define ADD_CAUSE_NONE                           25
#define ADD_CAUSE_INCONSISTENT_PARAMETERS        26
#define ADD_CAUSE_LOCKED_BY_OTHER_CLIENT         27

/************************************************************************/
/* IEC 61850 LTRK definitions.                                          */
/************************************************************************/
/* ServiceType values from Table 51 in IEC 61850-8-1.  */
#define SERVICE_TYPE_UNKNOWN                   0
#define SERVICE_TYPE_ASSOCIATE                 1
#define SERVICE_TYPE_ABORT                     2
#define SERVICE_TYPE_RELEASE                   3
#define SERVICE_TYPE_GETSERVERDIRECTORY        4
#define SERVICE_TYPE_GETLOGICALDEVICEDIRECTORY 5
#define SERVICE_TYPE_GETALLDATAVAUES           6
#define SERVICE_TYPE_GETDATAVALUES             7
#define SERVICE_TYPE_SETDATAVALUES             8
#define SERVICE_TYPE_GETDATADIRECTORY          9
#define SERVICE_TYPE_GETDATADEFINITION         10
#define SERVICE_TYPE_GETDATASETVALUES          11
#define SERVICE_TYPE_SETDATASETVALUES          12
#define SERVICE_TYPE_CREATEDATASET             13
#define SERVICE_TYPE_DELETEDATASET             14
#define SERVICE_TYPE_GETDATASETDIRECTORY       15
#define SERVICE_TYPE_SELECTACTIVESG            16
#define SERVICE_TYPE_SELECTEDITSG              17
#define SERVICE_TYPE_SETEDITSGVALUE            18
#define SERVICE_TYPE_CONFIRMEDITSGVALUES       19
#define SERVICE_TYPE_GETEDITSGVALUE            20
#define SERVICE_TYPE_GETSGCBVALUES             21
#define SERVICE_TYPE_REPORT                    22
#define SERVICE_TYPE_GETBRCBVALUES             23
#define SERVICE_TYPE_SETBRCBVALUES             24
#define SERVICE_TYPE_GETURCBVALUES             25
#define SERVICE_TYPE_SETURCBVALUES             26
#define SERVICE_TYPE_GETLCBVALUES              27
#define SERVICE_TYPE_SETLCBVALUES              28
#define SERVICE_TYPE_QUERYLOGBYTIME            29
#define SERVICE_TYPE_QUERYLOGAFTER             30
#define SERVICE_TYPE_GETLOGSTATUSVALUES        31
#define SERVICE_TYPE_SENDGOOSEMESSAGE          32
#define SERVICE_TYPE_GETGOCBVALUES             33
#define SERVICE_TYPE_SETGOCBVALUES             34
#define SERVICE_TYPE_GETGOREFERENCE            35
#define SERVICE_TYPE_GETGOOSEELEMENTNUMBER     36
#define SERVICE_TYPE_SENDMSVMESSAGE            37
#define SERVICE_TYPE_GETMSVCBVALUES            38
#define SERVICE_TYPE_SETMSVCBVALUES            39
#define SERVICE_TYPE_SENDUSVMESSAGE            40
#define SERVICE_TYPE_GETUSVCBVALUES            41
#define SERVICE_TYPE_SETUSVCBVALUES            42
#define SERVICE_TYPE_SELECT                    43
#define SERVICE_TYPE_SELECTWITHVALUE           44
#define SERVICE_TYPE_CANCEL                    45
#define SERVICE_TYPE_OPERATE                   46
#define SERVICE_TYPE_COMMANDTERMINATION        47
#define SERVICE_TYPE_TIMEACTIVATEDOPERATE      48
#define SERVICE_TYPE_GETFILE                   49
#define SERVICE_TYPE_SETFILE                   50
#define SERVICE_TYPE_DELETEFILE                51
#define SERVICE_TYPE_GETFILEATTRIBUTEVALUES    52
#define SERVICE_TYPE_TIMESYNCHRONIZATION       53
#define SERVICE_TYPE_INTERNALCHANGE            54

/* IEC 61850-7-2 says errorCode type is ServiceError.*/
/* IEC 61850-8-1 Table 52 maps ServiceError for CST to MMS like this.  */
#define TRK_ERR_NO_ERROR                                  0
#define TRK_ERR_INSTANCE_NOT_AVAILABLE                    1
#define TRK_ERR_INSTANCE_IN_USE                           2
#define TRK_ERR_ACCESS_VIOLATION                          3
#define TRK_ERR_ACCESS_NOT_ALLOWED_IN_CURRENT_STATE       4
#define TRK_ERR_PARAMETER_VALUE_INAPPROPRIATE             5
#define TRK_ERR_PARAMETER_VALUE_INCONSISTENT              6
#define TRK_ERR_CLASS_NOT_SUPPORTED                       7
#define TRK_ERR_INSTANCE_LOCKED_BY_OTHER_CLIENT           8
#define TRK_ERR_CONTROL_MUST_BE_SELECTED                  9
#define TRK_ERR_TYPE_CONFLICT                             10
#define TRK_ERR_FAILED_DUE_TO_COMMUNICATIONS_CONSTRAINT   11
#define TRK_ERR_FAILED_DUE_TO_SERVER_CONSTRAINT           12

/************************************************************************/
/* UCA SPECIFIC DATA ELEMENTS */
/************************************************************************/

typedef struct mvlu_rd_va_ctrl
  {
  MVL_IND_PEND *indCtrl;
  MVLAS_RD_VA_CTRL *rdVaCtrl;
  RUNTIME_TYPE *rt;
#if defined(MVL_XNAME)
  ST_CHAR xName[MAX_IDENT_LEN+1];
#endif
  ST_CHAR *primData;
  ST_RTREF primRef;
  ST_UINT prim_num;             /* index to data        */
  ST_UINT prim_offset_base;     /* mem offset from start of "base" var  */
  } MVLU_RD_VA_CTRL;

typedef struct mvlu_wr_va_ctrl
  {
  MVL_IND_PEND *indCtrl;
  MVLAS_WR_VA_CTRL *wrVaCtrl;
  RUNTIME_TYPE *rt;
#if defined(MVL_XNAME)
  ST_CHAR xName[MAX_IDENT_LEN+1];
#endif
  ST_CHAR *primData;
  ST_RTREF primRef;
  ST_UINT prim_num;             /* index to data        */
  ST_UINT prim_offset_base;     /* mem offset from start of "base" var  */
  } MVLU_WR_VA_CTRL;

ST_VOID mvlu_wr_prim_done (MVLU_WR_VA_CTRL *mvluWrVaCtrl, ST_RET rc);
ST_VOID mvlu_rd_prim_done (MVLU_RD_VA_CTRL *mvluRdVaCtrl, ST_RET rc);

/* These function pointers are invoked to initialize the va->data       */
/* element for each VariableAssociation required to handle the MMS      */
/* indication.                                                          */

extern ST_VOID(*mvluAsyncWrIndFun)(struct mvlu_wr_va_ctrl *mvluWrVaCtrl);

/* Alternate leaf handler function pointers                             */
extern ST_VOID (*u_mvlu_leaf_rd_ind_fun)(MVLU_RD_VA_CTRL *mvluRdVaCtrl);
extern ST_VOID (*u_mvlu_leaf_wr_ind_fun)(MVLU_WR_VA_CTRL *mvluWrVaCtrl);

/* Finding and setting leaf access parameters */
RUNTIME_TYPE *mvlu_find_rt_leaf (MVL_TYPE_ID type_id, ST_CHAR *leafName);

#define MVLU_SET_RD_FUN 0x01
#define MVLU_SET_WR_FUN 0x02
#define MVLU_SET_REF    0x04
#define MVLU_SET_ALL    (MVLU_SET_RD_FUN | MVLU_SET_WR_FUN | MVLU_SET_REF)

ST_RET mvlu_set_leaf_param (ST_INT setFlags, ST_CHAR *leafName,
                             ST_RTINT rdIndFunIndex, ST_RTINT wrIndFunIndex,
                             ST_RTREF ref);


/************************************************************************/
/************************************************************************/
/* Leaf Access Function Tables. To be supplied by the user.             */

/* Comment the following line out to compile out the Foundry supplied   */
/* Leaf Access function pointer tables, so custom tables can be used    */
/*#define MVLU_USR_FUN_TBLS             */

/* Remove this MVLU_LEAF_FUN_LOOKUP_ENABLE define to save memory,
 * but ONLY IF NOT mapping leaf functions at run-time (i.e. not calling
 * mvlu_set_leaf_param_name or mvlu_find_rd(wr)_ind_fun_index).
 * NOTE: this define is required if SCL configuration is used.
 */
#define MVLU_LEAF_FUN_LOOKUP_ENABLE

typedef struct
  {
#if defined MVLU_LEAF_FUN_LOOKUP_ENABLE
 ST_CHAR *fun_name;
#endif
  ST_VOID (*fun_ptr) (MVLU_RD_VA_CTRL *rdCtrl);
  } MVLU_RD_FUN_INFO;
extern MVLU_RD_FUN_INFO mvluRdFunInfoTbl[];
extern ST_INT mvluNumRdFunEntries;

typedef struct
  {
#if defined MVLU_LEAF_FUN_LOOKUP_ENABLE
  ST_CHAR *fun_name;
#endif
  ST_VOID (*fun_ptr) (MVLU_WR_VA_CTRL *wrCtrl);
  } MVLU_WR_FUN_INFO;
extern MVLU_WR_FUN_INFO mvluWrFunInfoTbl[];
extern ST_INT mvluNumWrFunEntries;

/* Useful for flexible initialization of leaf function tables */
#if defined MVLU_LEAF_FUN_LOOKUP_ENABLE
#define _MVLU_LEAF_FUN_INIT(_funName) #_funName, _funName
#else
#define _MVLU_LEAF_FUN_INIT(_funName) _funName
#endif
ST_RTINT mvlu_find_rd_ind_fun_index (ST_CHAR *funName);
ST_RTINT mvlu_find_wr_ind_fun_index (ST_CHAR *funName);


/************************************************************************/

ST_RET mvlu_set_leaf_param_name (ST_INT setFlags, ST_CHAR *leafName,
                                 ST_CHAR *rdIndFunName, ST_CHAR *wrIndFunName,
                                 ST_CHAR *refString);
ST_RET mvlu_set_leaf_param_name2 (MVL_TYPE_ID base_var_type_id,
        ST_INT setFlags,
        ST_CHAR *leafName,
        ST_CHAR *rdIndFunName,
        ST_CHAR *wrIndFunName,
        ST_CHAR *refString);
ST_RET mvlu_load_xml_leaf_file (ST_CHAR *fileName);
ST_RET u_mvlu_resolve_leaf_ref (ST_CHAR *leafName, ST_INT *setFlags,
                                ST_CHAR *refText, ST_RTREF *refOut);

/* Prefix to use in XML LAP file, or when calling "mvlu_set_leaf_param_name"
 * if you want a quoted string to be saved as the Reference.
 * Following this is the string to be saved.
 * This is used for SBO select, in which case the saved string must be
 * the name of the protected element. Foundry and the sample "reference
 * resolve leaf" function look for this prefix.
 */
#define MVLU_STRING_REF_PREFIX "MVLU_STR:"

/* Internal */
ST_VOID _mvlu_null_read_ind (MVLU_RD_VA_CTRL *mvluRdVaCtrl);
ST_VOID _mvlu_null_write_ind (MVLU_WR_VA_CTRL *mvluWrVaCtrl);



typedef struct
  {
  ST_CHAR ucaName [MAX_IDENT_LEN+1];
  RUNTIME_TYPE *rt;
  ST_INT sortedNum;
  ST_INT rtIndex;
  } MVLU_UCA_NAME_CTRL;

ST_RET mvlu_proc_rt_type (ST_CHAR *typeName, RUNTIME_CTRL *rtCtrl,
                          MVLU_UCA_NAME_CTRL **ucaNamesOut);

/************************************************************************/
/************************************************************************/
/* UCA REPORT CONTROL */
/************************************************************************/

#define MVLU_RPT_NUM_VA_DATA_BUF  3

/* TrgOpsvalues, also used as Reason values */
#define MVLU_TRGOPS_DATA        0x40
#define MVLU_TRGOPS_QUALITY     0x20
#define MVLU_TRGOPS_FREEZE      0x10
/* MVLU_TRGOPS_DUPD same as MVLU_TRGOPS_FREEZE but clearer for IEC 61850.*/
#define MVLU_TRGOPS_DUPD        0x10
#define MVLU_TRGOPS_INTEGRITY   0x08    /* Bit 4        */
#define MVLU_TRGOPS_GI          0x04    /* Bit 5        */

/* OptFlds bit masks */
#define MVLU_SQNUM_MASK         0x40
#define MVLU_RPTTIM_MASK        0x20
#define MVLU_REASONS_MASK       0x10
#define MVLU_OUTDAT_MASK        0x08

/* TrgOps bit numbers for IEC-61850.                                    */
/* Use "bit" macros (BSTR_BIT_*) to access each individual bit.         */
#define TRGOPS_BITNUM_RESERVED                  0
#define TRGOPS_BITNUM_DATA_CHANGE               1       /* "dchg" in some specs */
#define TRGOPS_BITNUM_QUALITY_CHANGE            2       /* "qchg" in some specs */
#define TRGOPS_BITNUM_DATA_UPDATE               3       /* "dupd" in some specs */
#define TRGOPS_BITNUM_INTEGRITY                 4       /* "period" in 61850-6  */
#define TRGOPS_BITNUM_GENERAL_INTERROGATION     5

/* OptFlds bit numbers for IEC-61850.                                   */
/* Use "bit" macros (BSTR_BIT_*) to access each individual bit.         */
/* NOTE: DATSETNAME in IEC-61850 same as OUTDAT in UCA.                 */
/* NOTE: bit numbers 1 thru 4 correspond to the masks MVLU_SQNUM_MASK,  */
/*       MVLU_RPTTIM_MASK, MVLU_REASONS_MASK, MVLU_OUTDAT_MASK above.   */
/*       Bit masks don't work well beyond 8 bits.                       */
#define OPTFLD_BITNUM_RESERVED          0
#define OPTFLD_BITNUM_SQNUM             1
#define OPTFLD_BITNUM_TIMESTAMP         2
#define OPTFLD_BITNUM_REASON            3
#define OPTFLD_BITNUM_DATSETNAME        4
#define OPTFLD_BITNUM_DATAREF           5
#define OPTFLD_BITNUM_BUFOVFL           6
#define OPTFLD_BITNUM_ENTRYID           7
#define OPTFLD_BITNUM_CONFREV           8
#define OPTFLD_BITNUM_SUBSEQNUM         9       /* segmentation in 61850-8-1*/

/* Flags to pass as "rcb_type" arg to mvlu_create_rpt_ctrl.     */
/* Saved in "rcb_type" parameter of MVLU_RPT_CTRL.              */
#define RCB_TYPE_UCA                    0
#define RCB_TYPE_IEC_BRCB               1
#define RCB_TYPE_IEC_URCB               2

/* Flags to pass as "buftim_action" arg to mvlu_create_rpt_ctrl.        */
/* Saved in "buftim_action" parameter of MVLU_RPT_CTRL.                 */
#define MVLU_RPT_BUFTIM_REPLACE         0
#define MVLU_RPT_BUFTIM_SEND_NOW        1

/* Basic Report Control Block data */
typedef struct
  {
  ST_BOOLEAN RptEna;
  ST_BOOLEAN Resv;              /* Used only for 61850 URCB.    */
  ST_CHAR    RptID[MVL61850_MAX_OBJREF_LEN+1];
  /* NOTE: DatSetNa is shorter for UCA, but bigger buffer shouldn't hurt.*/
  ST_CHAR    DatSetNa[MVL61850_MAX_OBJREF_LEN+1];
  struct     /* BVstring */
    {
    ST_INT16 len_1;
    ST_UCHAR data_1[2];         /* BVstring9 - need 2 bytes     */
    } OptFlds;
  ST_UINT32 BufTim;
  ST_UINT16 Trgs;
  ST_UINT8  SqNum;              /* Used for UCA & 61850 URCB    */
  ST_UINT16 SqNumInt16u;        /* Used for 61850 BRCB          */
  struct     /* BVstring */
    {
    ST_INT16 len;
    ST_UCHAR data[1];           /* BVstring8 - need 1 byte      */
    } TrgOps;
  /* NOTE: RBEPd used only when OBSOLETE_UCA defined. May be deleted later.*/
  ST_UINT32 RBEPd;
  ST_UINT32 IntgPd;
  ST_UINT32 ConfRev;            /* Used only for 61850 BRCB/URCB.       */
  ST_UCHAR EntryID[8];          /* Used only for 61850 BRCB (Ostring8). */
  MMS_BTIME6 TimeofEntry;       /* Used only for 61850 BRCB.    */

  ST_INT16 ResvTms;             /* Used only for 61850 Edition 2 BRCB.  */
  /* CRITICAL: Owner is used to indicate Reservation.   */
  /* If all 0, RCB is not reserved. If not all 0, RCB is reserved.      */
  ST_UCHAR Owner[64];           /* Used only for 61850 Edition 2 BRCB/URCB (Ostring64).*/
                                /* First 4 bytes contain IP Address of client*/
  ST_UINT OwnerLen;             /* len of Owner: 4 for IPv4, 16 for IPv6*/
  } MVLU_BASRCB;

/************************************************************************/
/* Structures for saving Buffered report data.                          */
/************************************************************************/
/* VAR_DATA - data for one variable of a Report Dataset.                */
typedef struct var_data
  {
  ST_VOID *data_ptr;            /* ptr to raw MMS Variable data         */
                                /* (allocated)                          */
  ST_INT data_len;              /* length of raw MMS Variable data      */
  ST_UINT8 reason_for_incl;     /* reason for inclusion                 */
  } VAR_DATA;

/* BUFLIST_ENTRY - data for one buffered rpt (stored on linked list).   */
/* Data in this struct usually changes with each Report, so must be     */
/* saved in the buffer. Other data may be taken directly from the RCB   */
/* when a Report is sent.                                               */
typedef struct buflist_entry
  {
  /* CRITICAL: First 2 parameters used to add this struct to linked     */
  /* lists using list_add_last, etc.                                    */
  struct buflist_entry *next;           /* CRITICAL: DON'T MOVE.        */
  struct buflist_entry *prev;           /* CRITICAL: DON'T MOVE.        */
  ST_UCHAR EntryID[8];          /* Rpt EntryID (Ostring8)               */
  MMS_BTIME6 TimeOfEntry;       /* Rpt TimeOfEntry                      */
  ST_INT num_var;               /* Num of var in var_data array         */
  VAR_DATA *var_data;           /* Ptr to array of structs, one for each*/
                                /* var (allocated)                      */
  ST_INT *asn1_len_array;       /* Array of ASN.1 lengths, one for each */
                                /* var in Dataset (allocated)           */
  ST_INT cur_va_index;          /* Index to next var in Dataset to send */
                                /* (used when segmenting)               */
  ST_UINT mem_size;             /* total size of memory allocated for   */
                                /* this entry                           */
  ST_UINT16 SubSeqNum;          /* SubSeqNum of next segment to send    */
  } BUFLIST_ENTRY;

/*      MVL61850_BRCB_CTRL      */
typedef struct
  {
  /* Optional parameters to send in rpt (if appropriate OptFlds bit set).*/
  ST_BOOLEAN BufOvfl;
  /* Internal parameters                                */
  BUFLIST_ENTRY *rpt_list;      /* Linked list of buffered reports      */
  BUFLIST_ENTRY *rpt_list_next; /* Next rpt to send from "rpt_list"     */
                                /* (NULL if list empty or all sent)     */
  ST_INT rpt_count;             /* count of entries in "rpt_list"       */
                                /* once buffer's full, shouldn't change much*/
  ST_UINT rpts_lost_count;      /* # rpts lost because of overflow      */
  ST_INT brcb_bufsize;
  ST_INT cur_bufsize;
  ST_BOOLEAN enabled_once;      /* TRUE if BRCB was enabled at least once*/
  ST_UCHAR lastSentEntryID[8];
  MMS_BTIME6 lastSentTimeOfEntry;
  ST_BOOLEAN resync;            /* TRUE only after successful resync    */
                                /* (valid EntryID was written)          */
  } MVL61850_BRCB_CTRL;

/* TypeIDs for special report types created by mvlu_rpt_init_types.     */
typedef struct
  {
  MVL_TYPE_ID mmsbool;       /* named so not confused with C++ 'bool' type   */
  MVL_TYPE_ID int8u;         /* for SqNum                    */
  MVL_TYPE_ID int16u;
  MVL_TYPE_ID int32u;
  MVL_TYPE_ID vstring65;     /* for RptID                    */
  MVL_TYPE_ID btime6;        /* for EntryTime                */
  MVL_TYPE_ID ostring8;      /* for EntryID                  */
  MVL_TYPE_ID vstring32;     /* for RptId                    */
  MVL_TYPE_ID bvstring6;     /* for TrgOps (IEC)             */
  MVL_TYPE_ID bvstring8;     /* for OptFlds,TrgOps (UCA)     */
  MVL_TYPE_ID bvstring10;    /* for OptFlds (IEC)            */
  MVL_TYPE_ID bstr6;         /* for Reason (IEC)             */
  MVL_TYPE_ID bstr8;         /* for Reason (UCA)             */
  MVL_TYPE_ID vstring129;    /* for ObjectReference (IEC)    */
  } MVLU_RPT_TYPEIDS;

/* For backward compatibility ... do not use */
#define MVLU_SEQNUM_MASK  MVLU_SQNUM_MASK

#define MVLU_MAX_RPTID_LEN  65
#define MVLU_MAX_OUTDAT_LEN  65


/*      MVLU_RPT_CTRL   */
typedef struct mvlu_rpt_ctrl
  {
  DBL_LNK l;                    /* Internal use */

  ST_CHAR *basrcb_name;

/* Used in read/write indication functions in finding the report ctrl   */
  RUNTIME_TYPE *rcbRtHead;
  MVL_VAR_ASSOC *base_va;

/* Used to support different report schemes     */
  ST_INT rcb_type;      /* RCB_TYPE_UCA, RCB_TYPE_IEC_BRCB, etc.        */

/* Action to be taken if var changes twice before buftim expires        */
  ST_INT buftim_action;         /* MVLU_RPT_BUFTIM_REPLACE/SEND_NOW     */

/* The information below is used internally by MVLU                     */
  MVL_NVLIST_CTRL *dsNvl;       /* The base dataSet for the report      */
  MVL_NVLIST_CTRL rptNvl;       /* The NVL used to send the InfoRpt     */
  RUNTIME_TYPE incRt;           /* Used in building the inclusion_va    */
  ST_INT maxNumRptVars;         /* Max vars allowed in report.          */
  MVLU_RPT_TYPEIDS rpt_typeids; /* Types needed for reports.            */
  MVL_TYPE_ID inclusion_typeid; /* One more special type.               */
  MVL61850_BRCB_CTRL brcbCtrl;  /* Used only for 61850 BRCB.            */
  ST_DOUBLE scan_rate;          /* Scan rate in milliseconds            */
  ST_DOUBLE next_scan_start;    /* Time for next scan                   */

  MVL_NET_INFO *netInfo;        /* Client who has reserved this RCB     */
  MVLU_BASRCB basrcb;           /* RCB data                             */

/* Transmit timing control                                              */
  ST_DOUBLE next_integ_rpt_time;
  ST_DOUBLE buf_time_done;
  ST_BOOLEAN integ_scan_in_progress;

/* Data state control */
  ST_BOOLEAN firstScanDone;     /* TRUE after first change scan completes*/
  ST_UINT16 numTrgs;
  ST_UINT8 *reasons_data;       /* array of reasons     */
  ST_UINT8 *changed_flags;      /* bitstring            */
  ST_UINT8 *segmented_inclusion;/* inclusion bitstring for segmented rpt*/
  ST_DOUBLE ResvTmsExpireTime;  /* Time to release BRCB reservation     */
                                /* Init when connection lost.           */
                                /* Unreserve when this time reached.    */

/* After SCL parse, address info copied from SCL_ADDRESS to these structs.*/
  ST_BOOLEAN clientln_pres;     /* true if ClientLN specified in SCL    */
  AE_TITLE   clientln_ae_title; /* AE-Title of reserved client          */
  PRES_ADDR  clientln_paddr;    /* Presentation Addr of reserved client */

  ST_CHAR objRefAcsi [MVL61850_MAX_OBJREF_LEN+1]; /* init at startup    */
  } MVLU_RPT_CTRL;

extern MVLU_RPT_CTRL *mvlu_rpt_ctrl_list;       /* List of only UCA RCBs.*/
extern MVLU_RPT_CTRL *mvl61850_rpt_ctrl_list;   /* List of only 61850 RCBs.*/

/* UCA VA Scan Control */
typedef struct
  {
  DBL_LNK l;                            /* Internal use                 */
  ST_BOOLEAN enable;
  ST_DOUBLE scan_period;
  ST_DOUBLE next_scan_start;
  ST_BOOLEAN saturated;
  ST_INT num_va_changes;

  ST_INT num_scan_va;
  MVL_VAR_ASSOC **scan_va;
  ST_INT num_va_read_pend;

  MVL_IND_PEND indCtrl;
  MVL_COMM_EVENT commEvent;
  MVL_SCOPE *scan_va_scope;             /* ptr to array of structs.     */
  ST_VOID *usr;         /* user assignable pointer to use in scan callbacks*/
  } MVLU_RPT_SCAN_CTRL;
extern MVLU_RPT_SCAN_CTRL *mvlu_rpt_scan_list;


/* Report Service */
ST_VOID mvlu_rpt_service (ST_VOID);

/* Report Variable Scanning */
MVLU_RPT_SCAN_CTRL *mvlu_rpt_create_scan_ctrl (ST_INT numScanVa);
MVLU_RPT_SCAN_CTRL *mvlu_rpt_create_scan_ctrl2 (
        MVL_NVLIST_CTRL *nvl,
        ST_RET (*scan_done_fun) (struct mvl_ind_pend *ind_pend),
        ST_UINT report_scan_rate);      /* report scan rate (millisec)  */
ST_VOID mvlu_rpt_destroy_scan_ctrl (MVLU_RPT_SCAN_CTRL *scanCtrl);
ST_VOID mvlu_rpt_va_scan (ST_VOID);
ST_VOID mvlu_rpt_scan_read (MVLU_RPT_SCAN_CTRL *scanCtrl);

/* Asynchronous Change Reporting */
ST_VOID mvlu_rpt_va_change (MVL_VAR_ASSOC *va,
                            ST_UCHAR reason,
                            ST_VOID *new_data);

/* Creating Report Dataset NVL's */
MVL_NVLIST_CTRL *mvlu_derive_rpt_ds (ST_CHAR *domName, ST_CHAR *nvlName,
                                     ST_INT numNodes, ST_CHAR **nodeNames);

/* Structure Element name derivation */
ST_RET mvlu_derive_ds_va_names (ST_CHAR *domName,
                                ST_INT numNodes, ST_CHAR **nodeNames,
                                ST_INT *numObjNamesOut,
                                OBJECT_NAME **objNameTblOut,
                                ST_CHAR **nameBufOut);


/* Report Control Create/Free  */
/* NOTE: The "buftim_action" arg may be set to
 * MVLU_RPT_BUFTIM_REPLACE or MVLU_RPT_BUFTIM_SEND_NOW.
 * This arg is only used in the case when buffer
 * time (BufTim) is used, and a second data change is detected for the
 * same "va", before the BufTim expires. To satisfy the requirements of
 * IEC61850-7-2, the caller must set this flag to indicate whether to
 * replace the buffered value with the new value and continue timing
 * (MVLU_RPT_BUFTIM_REPLACE), OR to send a report immediately with the
 * buffered value, save the new value in the buffer, and restart the timer
 * (MVLU_RPT_BUFTIM_SEND_NOW).
 */
MVLU_RPT_CTRL *mvlu_create_rpt_ctrl (ST_CHAR *basrcbName,
                        MVL_NVLIST_CTRL *dsNvl,
                        MVL_VAR_ASSOC *base_va,
                        ST_INT rcb_type,
                        ST_INT buftim_action,   /* MVLU_RPT_BUFTIM_*    */
                        ST_INT brcb_bufsize,    /* for BRCB only        */
                        ST_UINT32 ConfRev);     /* for BRCB/URCB only   */
ST_VOID mvlu_free_rpt_ctrl (MVLU_RPT_CTRL *rptCtrl);

/* Main reporting functions for IEC 61850 only.         */
MVLU_RPT_CTRL *mvl61850_create_rpt_ctrl (ST_CHAR *basrcbName,
                        MVL_NVLIST_CTRL *dsNvl,
                        MVL_VAR_ASSOC *base_va,
                        ST_INT rcb_type,
                        ST_INT buftim_action,   /* MVLU_RPT_BUFTIM_*    */
                        ST_INT brcb_bufsize,    /* for BRCB only        */
                        ST_UINT32 ConfRev);     /* for BRCB/URCB only   */
ST_VOID mvl61850_free_rpt_ctrl (MVLU_RPT_CTRL *rptCtrl);
ST_VOID mvl61850_rpt_service (ST_VOID);
ST_RET mvl61850_rpt_dataset_create (MVLU_RPT_CTRL *rptCtrl,
                        MVL_NVLIST_CTRL *dsNvl);
ST_VOID mvl61850_rpt_dataset_destroy (MVLU_RPT_CTRL *rptCtrl);

/************************************************************************/
/*                      mvlu_integrity_scan_destroy                     */
/* Free temporary MVL_IND_PEND allocated by mvlu_integrity_scan_read.   */
/************************************************************************/
ST_VOID mvlu_integrity_scan_destroy (MVL_IND_PEND *indCtrl);

/*
 *                      mvlu_integrity_scan_read
 * This function "BEGINS" the scan of ALL data for one Report DataSet (NVL).
 * It is called automatically when an Integrity Report or a
 * General Interrogation Report needs to be sent. This is much more
 * efficient than constantly scanning.
 * - It allocates & initializes a temporary MVL_IND_PEND structure.
 * - It calls "u_mvl_read_ind" to begin the scan.
 * - The scan may complete synchronously or asynchronously.
 * - When the scan completes, the funct pointed to by "scan_done_fun" arg
 *   is called to build the report and cleanup.
 * CRITICAL: Function pointed to by "scan_done_fun" must call
 * "mvlu_integrity_scan_destroy" to free temporary MVL_IND_PEND struct.
 */

ST_VOID mvlu_integrity_scan_read (MVLU_RPT_CTRL *rptCtrl,
                        ST_VOID (*scan_va_done_fun)(MVL_IND_PEND *indCtrl,
                                        MVL_VAR_ASSOC *va),
                        ST_RET (*scan_done_fun)(MVL_IND_PEND *indCtrl)
                        );

MVL_IND_PEND *mvlu_setup_scan_read (MVL_NVLIST_CTRL *nvl,
                        ST_VOID (*scan_va_done_fun)(MVL_IND_PEND *indCtrl,
                                        MVL_VAR_ASSOC *va),
                        ST_RET (*scan_done_fun)(MVL_IND_PEND *indCtrl)
                        );

ST_VOID mvlu_scan_read_lov_start (
        ST_INT num_of_entries,          /* num of vars in array         */
        MVL_VAR_ASSOC **varlist,        /* array of variables           */
        MVL_SCOPE **scopelist,          /* array of scopes              */
        ST_VOID *usr_info,              /* ptr to any user info         */
                                        /* saved in indCtrl->usr        */
        ST_RET (*scan_done_fun)(MVL_IND_PEND *indCtrl)
        );
ST_RET mvlu_scan_read_lov_cleanup (MVL_IND_PEND *indCtrl);

/************************************************************************/
/*                      mvlu_integrity_timeout                          */
/* Check for Integrity period timeout. If timeout occurred, start       */
/* integrity scan.                                                      */
/* RETURNS: SD_TRUE if IntgPd is set and timeout occurred               */
/*          SD_FALSE otherwise                                          */
/************************************************************************/
ST_RET mvlu_integrity_timeout (MVLU_RPT_CTRL *rptCtrl, ST_DOUBLE timeNow);

/* Pointer to one of these functions passed to "mvlu_integrity_scan_read",
 * saved in "usr_done_fun" in MVL_IND_PEND struct.  Called when scan completes.
 */
ST_RET mvlu_integrity_scan_done (MVL_IND_PEND *indCtrl);
ST_RET mvlu_gi_scan_done (MVL_IND_PEND *indCtrl);

/* Pointer to one of these functions passed to "mvlu_integrity_scan_read",
 * stored in global var. Called when each va read completes.
 */
ST_VOID mvlu_integrity_scan_va_done (MVL_IND_PEND *indCtrl, MVL_VAR_ASSOC *va);
ST_VOID mvlu_gi_scan_va_done (MVL_IND_PEND *indCtrl, MVL_VAR_ASSOC *va);

extern ST_VOID (*_mvlu_rpt_disconnect_rcvd_fun)(MVL_NET_INFO *netInfo);

MVL_NVLIST_CTRL *mvlu_resolve_uca_nvl (MVL_NVLIST_CTRL *ucaNvl,
                                       OBJECT_NAME *scopeSel);

/* Defines for UCA/IEC report type, "rpt_type" arg to "mvlu_send_report"*/
#define MVLU_RPT_TYPE_INTEGRITY_OR_GI   0
#define MVLU_RPT_TYPE_RBE               1

ST_RET mvlu_send_report (MVLU_RPT_CTRL *rptCtrl, ST_INT rpt_type);

/* u_mvlu_rpt_time_get - User callback function to set accurate time
 * that UCA/IEC report is built, called by "mvlu_send_report" or
 * "mvl61850_rcb_build" when report is being built.
 */
ST_VOID u_mvlu_rpt_time_get (MMS_BTIME6 *TimeOfEntry);

/* Misc. Helper Functions */
ST_INT mvlu_rpt_get_va_index (MVLU_RPT_CTRL *rptCtrl, MVL_VAR_ASSOC *va);
MVLU_BASRCB *_mvlu_get_rd_rcb (MVLU_RD_VA_CTRL *mvluRdVaCtrl,
                                MVLU_RPT_CTRL **rptCtrlOut);
MVLU_BASRCB *_mvlu_get_wr_rcb (MVLU_WR_VA_CTRL *mvluWrVaCtrl,
                                      MVLU_RPT_CTRL **rptCtrlOut);
MVLU_BASRCB *mvl61850_get_rcb (MVL_VAR_ASSOC *baseVa,
        RUNTIME_TYPE *rt,
        MVLU_RPT_CTRL **rptCtrlOut);

ST_VOID _mvlu_rpt_disconnect_rcvd (MVL_NET_INFO *netInfo);
ST_RET mvlu_rpt_find_typeids (MVLU_RPT_TYPEIDS *rpt_typeids);
ST_RET mvlu_rpt_rcb_type_find (MVL_TYPE_ID type_id, ST_CHAR *basrcbName,
        RUNTIME_TYPE **rcbHeadOut, ST_INT *numRtOut);


ST_RET mvlu_find_uca_var (RUNTIME_TYPE **rtIo, ST_INT *numRtIo,
                         ST_CHAR *varName);

ST_RET mvl61850_enumval_check (MVLU_WR_VA_CTRL *mvluWrVaCtrl);

/************************************************************************/
/************************************************************************/
/* SBO HANDLING ELEMENTS                                                */

#define MVL61850_SBO_SELECT_TIMEOUT     30000   /* milliseconds         */

#define SBO_SELECT_TIMEOUT      30      /* seconds (for UCA)            */
#define MAX_NUM_SBO_PEND        10      /* Number of SBOs to be pending */
#define MAX_SBO_NAME_SIZE       66

typedef struct mvl_sbo_ctrl
  {
  ST_BOOLEAN in_use;                    /* control element management   */
  ST_INT ctlState;              /* Control state (MVL61850_CTLSTATE_*)  */
  ST_BOOLEAN use_ms_timer;              /* if TRUE, use expire_time_ms below*/
  ST_CHAR sbo_var[MVL61850_MAX_OBJREF_LEN+1];   /* ObjRef of "Oper" attr*/
  MVL_NET_INFO *net_info;               /* Connection ID                */
  time_t expire_time;                   /* SELECT expiration time       */
  ST_DOUBLE expire_time_ms;             /* SELECT expiration time in ms */
  ST_INT8 ctlModel;                     /* sbo-with-normal-security, etc*/
  ST_INT8 sboClass;                     /* operate-once or operate-many */

  /* Use to save info for IEC 61850 CommandTermination.                 */
  MVL61850_LAST_APPL_ERROR LastApplError;
  ST_CHAR sbow_flatname [MAX_IDENT_LEN + 1];  /* used for SBOw data checks*/
  } MVL_SBO_CTRL;


ST_VOID u_mvl_sbo_operate (MVL_SBO_CTRL *sboSelect,
                           MVLU_WR_VA_CTRL *mvluWrVaCtrl);

ST_VOID mvlu_clr_pend_sbo (MVL_NET_INFO *net_info);

MVL_SBO_CTRL *mvlu_sbo_chk_state (ST_CHAR *sboName,
                                     MVL_NET_INFO *net_info);
ST_VOID mvlu_sbo_chk_timers (ST_VOID);
ST_VOID mvlu_sbo_ctrl_free (MVL_SBO_CTRL *sboCtrl);
MVL_SBO_CTRL *initSboCtrl (MVL_NET_INFO *net_info,
        ST_CHAR *sbo_name,
        ST_UINT32 sboTimeout,
        ST_BOOLEAN use_ms_timer);


/************************************************************************/
/************************************************************************/
/* IEC-61850 functions                                                  */
/************************************************************************/

/*
 *                      mvl61850_brcb_client_service
 *
 * Check the BRCB state and, if necessary,
 * encode, queue, AND/OR send IEC "Buffered" reports.
 */
ST_VOID mvl61850_brcb_client_service (MVLU_RPT_CTRL *rptCtrl,
                        ST_DOUBLE timeNow);

/**
 *                      mvl61850_urcb_client_service
 *
 * Check the URCB state and, if necessary,
 * encode AND send IEC "Unbuffered" reports.
 */
ST_VOID mvl61850_urcb_client_service (MVLU_RPT_CTRL *rptCtrl,
                        ST_DOUBLE timeNow);

ST_RET mvlu_rpt_ready (MVLU_RPT_CTRL *rptCtrl, ST_INT rpt_type);
ST_RET mvl61850_urcb_rpt_send (MVLU_RPT_CTRL *rptCtrl,
        ST_INT rpt_type);
ST_RET mvl61850_brcb_rpt_save (MVLU_RPT_CTRL *rptCtrl);
ST_RET mvl61850_brcb_rpt_send (MVLU_RPT_CTRL *rptCtrl,
        BUFLIST_ENTRY *buflist_entry,
        ST_BOOLEAN *all_seg_done);
ST_VOID mvl61850_brcb_entryid_init (MVLU_RPT_CTRL *rptCtrl, ST_UINT8 *EntryID);

ST_VOID mvl61850_brcb_rpt_lists_clean (MVL61850_BRCB_CTRL *brcbCtrl);

/* Internal IEC 61850 reporting functions (not called by user). */
ST_RET mvl61850_mk_rptid (MVLU_RPT_CTRL *rptCtrl, ST_CHAR *RptID, size_t max_len);
ST_VOID mvl61850_rcb_cleanup (MVLU_RPT_CTRL *rptCtrl);
ST_RET mvl61850_brcb_rpt_set_entryid (MVLU_RPT_CTRL *rptCtrl, ST_UCHAR *EntryID);
ST_VOID mvl61850_rcb_chk_state (MVLU_RPT_CTRL *rptCtrl,
        ST_DOUBLE timeNow);
ST_RET mvl61850_integrity_timeout (MVLU_RPT_CTRL *rptCtrl, ST_DOUBLE timeNow);
ST_BOOLEAN chk_seg_needed (MVLU_RPT_CTRL *rptCtrl,
                        ST_INT *asn1_len_array,
                        ST_INT cur_va_index,    /* input arg    */
                        ST_INT *next_va_index); /* output arg   */

/************************************************************************/
/* IEC 61850 "Control Model" structures.                                */
/************************************************************************/

/************************************************************************/
/* struct for creating list of direct-with-enhanced-security controls   */
/* that are currently in progress.                                      */
/************************************************************************/
typedef struct mvl61850_ctl_direct_enhanced
  {
  /* CRITICAL: First 2 parameters used to add this struct to linked     */
  /* lists using list_add_last, etc.                                    */
  struct mvl61850_ctl_direct_enhanced *next;    /* CRITICAL: DON'T MOVE.*/
  struct mvl61850_ctl_direct_enhanced *prev;    /* CRITICAL: DON'T MOVE.*/
  ST_CHAR oper_ref [MVL61850_MAX_OBJREF_LEN + 1];
  MVL_NET_INFO *net_info;       /* connection used for this control     */
  ST_INT ctlState;              /* Control state (MVL61850_CTLSTATE_*)  */
  } MVL61850_CTL_DIRECT_ENHANCED;

/************************************************************************/
/* IEC 61850 "Control Model" functions.                                 */
/************************************************************************/
ST_VOID mvl61850_sbo_create_sboname (MVL_VAR_ASSOC *va, MVL_SCOPE *va_scope,
                      ST_CHAR *sboName);

MVL_SBO_CTRL *mvl61850_ctl_chk_sbo (MVLU_RD_VA_CTRL *mvluRdVaCtrl);
MVL_SBO_CTRL *mvl61850_ctl_chk_sbow (MVLU_WR_VA_CTRL *mvluWrVaCtrl);

ST_RET mvl61850_ctl_chk_state (MVLU_WR_VA_CTRL *mvluWrVaCtrl);

ST_RET mvl61850_ctl_lastapplerror_create (ST_VOID);
ST_RET mvl61850_ctl_lastapplerror_send (MVL_NET_INFO *net_info,
        MVL61850_LAST_APPL_ERROR *last_appl_error);
ST_RET mvl61850_ctl_command_termination (MVL_NET_INFO *net_info, ST_CHAR *oper_ref,
              ST_RET status, MVL61850_LAST_APPL_ERROR *last_appl_error);
ST_VOID mvl61850_ctl_req_done (MVLAS_WR_VA_CTRL *wrVaCtrl, MVL_NET_INFO *net_info);

ST_RET mvl61850_mkname_ctlmodel (ST_CHAR *var_name, ST_CHAR *flatname, size_t flatname_len);
ST_RET mvl61850_mkname_sbotimeout (ST_CHAR *var_name, ST_CHAR *flatname, size_t flatname_len);
ST_RET mvl61850_mkname_sboclass   (ST_CHAR *var_name, ST_CHAR *flatname, size_t flatname_len);

ST_RET mvl61850_ctl_sbow_attr_cmp (MVLU_WR_VA_CTRL *mvluWrVaCtrl);

MVL61850_CTL_DIRECT_ENHANCED *mvl61850_ctl_direct_enhanced_get (
        MVL_NET_INFO *net_info,   /* connection used for this control   */
        ST_CHAR *oper_ref);       /* ObjectReference of "Oper" attr     */
ST_VOID mvl61850_ctl_direct_enhanced_free (MVL61850_CTL_DIRECT_ENHANCED *direct_enh);
ST_RET mvl61850_ctl_direct_enhanced_check (
        ST_CHAR *oper_ref);       /* ObjectReference of "Oper" attr     */

/************************************************************************/
/* Miscellaneous helper functions                                       */
/************************************************************************/
ST_VOID mvlu_trim_branch_name (ST_CHAR *branch_name);
ST_RET mvlu_find_comp_type (MVL_TYPE_ID base_type_id, ST_CHAR *flat_name,
                        RUNTIME_TYPE **sub_rt_type,             /* out  */
                        ST_INT *sub_rt_num);                    /* out  */

ST_RET mvlu_get_leaf_val_int8 (MVL_VAR_ASSOC *base_va, ST_CHAR *flatname, ST_INT8 *data);
ST_RET mvlu_get_leaf_val_int32 (MVL_VAR_ASSOC *base_va, ST_CHAR *flatname, ST_INT32 *data);
ST_RET mvlu_get_leaf_val_int_any (MVL_VAR_ASSOC *base_va, ST_CHAR *flatname, ST_INT32 *data);
ST_RET mvlu_get_leaf_val_uint32 (MVL_VAR_ASSOC *base_va, ST_CHAR *flatname, ST_UINT32 *data);
ST_RET mvlu_get_leaf_val_boolean (MVL_VAR_ASSOC *base_va, ST_CHAR *flatname, ST_BOOLEAN *data);
ST_RET mvlu_get_leaf_val_bvstring (MVL_VAR_ASSOC *base_va, ST_CHAR *flatname,
                                   MMS_BVSTRING *data, ST_INT max_num_bits);
ST_VOID *mvlu_get_leaf_data_ptr (MVL_VAR_ASSOC *base_va, ST_CHAR *flatname, RUNTIME_TYPE **rt_type);
ST_RET _rcb_writable (MVLU_BASRCB *rcb, MVLU_RPT_CTRL *rptCtrl,
        MVLU_WR_VA_CTRL *mvluWrVaCtrl);
ST_RET mvl61850_objref_create (ST_CHAR *objName,
        MVL_SCOPE *objScope,
        ST_CHAR *objRef);       /* ptr to ObjectReference               */
ST_RET mvl61850_objref_acsi_create (ST_CHAR *objName,
        ST_CHAR *domName,
        ST_CHAR *objRefAcsi);   /* ptr to ObjectReference (ACSI syntax) */
OBJECT_NAME *mvl61850_objref_to_objname (
        ST_CHAR *objref);       /* ObjectReference      */

/* Create and destroy "flattened" variable for UCA/IEC 61850.   */
MVL_VAR_ASSOC *mvlu_flat_var_create (
        MVL_VAR_ASSOC *base_va, /* base variable (Logical Node)         */
        ST_CHAR *flatname);     /* name of attribute inside base var    */
ST_VOID mvlu_flat_var_destroy (
        MVL_VAR_ASSOC *var);    /* var created by mvlu_flat_var_create  */

ST_RET mvlu_find_component (
        MVL_VAR_ASSOC *var,     /* base variable (Logical Node)         */
        ST_CHAR *flatname,      /* flattened name (e.g. ST$Mod$stVal)   */
        ALT_ACCESS *alt_acc,    /* NULL if Alt Access Spec not present  */
        RUNTIME_TYPE **rt_out,  /* if successful, *rt_out is set        */
        ST_INT *num_rt_out,     /* if successful, *num_rt_out is set    */
        ST_INT *offset_out,     /* if successful, *offset_out is set    */
        ST_UINT *prim_num_out); /* if successful, *prim_num_out is set  */

/************************************************************************/
/* Global variables.                                                    */
/************************************************************************/
extern MVL_SBO_CTRL sbo_pool[]; /* array of SBO control structures.     */

/* This function pointer called BEFORE all write leaf functions.        */
/* NOTE: called only for complex variables (struct or array).           */
extern ST_RET (*u_mvl_wr_ind_var_start)(MVL_IND_PEND *indCtrl,
        MVLAS_WR_VA_CTRL *wrVaCtrl);    /* current var in list of var   */

/* This function pointer called AFTER all write leaf functions.         */
/* NOTE: called only for complex variables (struct or array).           */
extern ST_RET (*u_mvl_wr_ind_var_end)(MVL_IND_PEND *indCtrl,
        MVLAS_WR_VA_CTRL *wrVaCtrl);    /* current var in list of var   */

/************************************************************************/
/* Structure containing pointers to data attributes of                  */
/* IEC 61850 Log Control Block (LCB).                                   */
/************************************************************************/
typedef struct
  {
  ST_UINT8 *LogEna;             /* Bool */
  ST_CHAR *LogRef;              /* ObjectReference      */
  ST_INT   LogRefMaxLen;        /* Max length of LogRef */
  ST_CHAR *DatSet;              /* ObjectReference      */
  ST_INT   DatSetMaxLen;        /* Max length of DatSet */
  MMS_BTIME6 *OldEntrTm;        /* Btime6       */
  MMS_BTIME6 *NewEntrTm;        /* Btime6       */
  ST_UINT8 *OldEnt;             /* Ostring8     */
  ST_UINT8 *NewEnt;             /* Ostring8     */
  MMS_BVSTRING *TrgOps; /* BVstring6 - struct includes 1 byte data. Enough.*/
  ST_UINT32 *IntgPd;            /* Ulong        */
  } MVL61850_LCB_ATTR;

/************************************************************************/
/* IEC 61850 Log info for one variable of a Log DataSet.                */
/************************************************************************/
typedef struct
  {
  ST_CHAR var_tag [MVL61850_MAX_OBJREF_LEN+1];  /* Vstring129 (ObjectReference) */
  ST_UINT8 reason;                                      /* reason       */
  } MVL61850_LOG_VAR_INFO;

/************************************************************************/
/* IEC 61850 Log control info.                                          */
/************************************************************************/
typedef struct mvl61850_log_ctrl
  {
  DBL_LNK l;                    /* Internal use */

  ST_CHAR *dom_name;            /* domain containing this LCB   */
  ST_CHAR *lcb_name;            /* LCB name                     */

  MVL_JOURNAL_CTRL *journal;    /* MMS Journal created for this Log.    */

  MVL61850_LCB_ATTR lcb_attr;   /* ptrs to LCB attributes in the LN     */
  ST_CHAR *lcb_data_start;      /* ptr to start of LCB data             */
  ST_CHAR *lcb_data_end;        /* ptr to end   of LCB data             */

  MVL_VAR_ASSOC *base_va;       /* Base variable (LN) containing LCB    */

  MVL_NVLIST_CTRL *dsNvl;       /* The dataset for the Log              */
  MVLU_RPT_TYPEIDS rpt_typeids; /* Types needed for Reports. Use for Logs too.*/

  ST_DOUBLE scanRateMs;                 /* Scan rate in milliseconds    */
  ST_DOUBLE next_change_scan_time;      /* Time for next "change" scan  */

  ST_DOUBLE next_integ_scan_time;       /* Time for next Integrity scan.*/
                                        /* LCB IntgPd used to set this. */

  ST_BOOLEAN Triggered;                 /* SD_TRUE if any trigger       */
                                        /* detected (dchg, qchg, etc).  */

  MVL61850_LOG_VAR_INFO *var_info;      /* array of struct with info    */
                                        /* about each var in Log dataset*/

  APP_REF orig_app;     /* Application Reference to send in each Journal entry*/
                        /* Init at startup.                             */
  ST_INT maxEntries;            /* Max number of Log entries            */
  ST_INT16 cur_state;           /* Event Condition State to store in    */
                                /* each Log entry.                      */
  ST_UCHAR EntryID [8];         /* EntryID to put in next Log entry     */

  ST_CHAR objRefAcsi [MVL61850_MAX_OBJREF_LEN+1]; /* init at startup    */
  } MVL61850_LOG_CTRL;

/************************************************************************/
/* IEC 61850 Log related functions.                                     */
/************************************************************************/
MVL61850_LOG_CTRL *mvl61850_log_ctrl_create (
        ST_CHAR *domName,               /* Domain name                  */
        ST_CHAR *lcbName,               /* LCB name                     */
        MVL_JOURNAL_CTRL *journal,      /* MMS journal for this Log     */
        APP_REF *orig_app,              /* Appl Ref of Originator       */
        MVL_NVLIST_CTRL *dsNvl,         /* DataSet (may be NULL)        */
        MVL_VAR_ASSOC *base_va,         /* base variable (the LN).      */
        ST_INT maxEntries,              /* Max number of Log entries    */
        ST_UINT scanRateMs);            /* Log scan rate (millisec)     */
ST_VOID mvl61850_log_ctrl_destroy (MVL61850_LOG_CTRL *logCtrl);
ST_VOID mvl61850_log_ctrl_destroy_all (ST_VOID);
MVL61850_LOG_CTRL *mvl61850_log_ctrl_find (
        MVL_VAR_ASSOC *base_va,         /* base variable (the LN).      */
        ST_VOID *primData);             /* ptr to attribute in LCB of LN*/
ST_VOID mvl61850_log_service (ST_VOID);
ST_VOID mvl61850_log_dataset_create (MVL61850_LOG_CTRL *logCtrl,
        MVL_NVLIST_CTRL *dsNvl);
ST_VOID mvl61850_log_dataset_destroy (MVL61850_LOG_CTRL *logCtrl);
ST_RET mvl61850_log_entry_add (MVL61850_LOG_CTRL *logCtrl);
ST_VOID mvl61850_log_entry_remove (MVL61850_LOG_CTRL *logCtrl, MVL_JOURNAL_ENTRY *entry);
ST_VOID mvl61850_log_jinit_resp (MVL_IND_PEND *indCtrl);
ST_RET mvl61850_log_integrity_chk_timeout (MVL61850_LOG_CTRL *logCtrl, ST_DOUBLE timeNow);

/************************************************************************/
/* Functions needed to combine IEC 61850 Report and Log processing.     */
/************************************************************************/
ST_VOID mvl61850_rpt_log_service (ST_DOUBLE timeNow);

/************************************************************************/
/* IEC 61850 Miscellaneous functions.                                   */
/************************************************************************/
ST_INT32 mvl61850_calc_beh (
        ST_INT32 xxxx_mod,   /* XXXX$ST$Mod$stVal (same LN as Beh) */
        ST_INT32 lln0_mod);  /* LLN0$ST$Mod$stVal.                 */
ST_VOID mvl61850_update_all_mod_beh (ST_VOID);

ST_VOID mvl61850_owner_set (MVL_NET_INFO *net_info, MVLU_BASRCB *rcb);
ST_INT  mvl61850_owner_compare (MVL_NET_INFO *net_info, MVLU_BASRCB *rcb);
ST_VOID mvl61850_owner_clear (MVLU_BASRCB *rcb);

ST_RET mvl61850_change_all_q (
  MVL_VAR_ASSOC *var,     /* variable in which to change all "q" values */
  ST_VOID (*fptr)(ST_UINT8 *bstrData, ST_UINT bitnum), /* user funct to call*/
  ST_UINT bitnum);        /* bit number to pass to user function (may be ignored)*/

/* Basic "user" functions to pass to mvl61850_change_all_q. */
ST_VOID u_mvl61850_q_bit_set_off (
  ST_UINT8 *bstrData,   /* ptr to "q" data to change */
  ST_UINT bitnum);      /* bit number to change      */
ST_VOID u_mvl61850_q_bit_set_on (
  ST_UINT8 *bstrData,   /* ptr to "q" data to change */
  ST_UINT bitnum);      /* bit number to change      */
ST_VOID u_mvl61850_q_test_blocked (
  ST_UINT8 *bstrData,   /* ptr to "q" data to change */
  ST_UINT bitnum);      /* ignored    */
ST_VOID u_mvl61850_q_invalid (
  ST_UINT8 *bstrData,   /* ptr to "q" data to change */
  ST_UINT bitnum);      /* ignored    */

ST_RET mvl61850_set_all_t (
  MVL_VAR_ASSOC *var,     /* variable in which to change all "q" values */
  MMS_UTC_TIME *utcTime); /* time value to set  */

/************************************************************************/
/* IEC 61850 LTRK functions.                                            */
/************************************************************************/
ST_RET ltrkInitAll (MVL_DOM_CTRL *dom);

ST_VOID ltrkUpdateUrcbTrk (MVL_NET_INFO *netInfo,
  MVL_DOM_CTRL *domCtrl,
  MVLU_RPT_CTRL *rptCtrl,
  ST_INT8 errorCode,
  ST_INT8 serviceType);

ST_VOID ltrkUpdateBrcbTrk (MVL_NET_INFO *netInfo,
  MVL_DOM_CTRL *domCtrl,
  MVLU_RPT_CTRL *rptCtrl,
  ST_INT8 errorCode,
  ST_INT8 serviceType);

ST_VOID ltrkUpdateLocbTrk (MVL_NET_INFO *netInfo,
  MVL_DOM_CTRL *domCtrl,
  MVL61850_LOG_CTRL *logCtrl,
  ST_INT8 errorCode,
  ST_INT8 serviceType);

ST_VOID ltrkUpdateGocbTrk (MVL_NET_INFO *netInfo,
  MVL_DOM_CTRL *domCtrl,
  GSE_IEC_CTRL *gooseCtrl,
  ST_BOOLEAN GoEna,
  ST_UINT8 *dstMAC,
  ST_UINT16 etype_tci,
  ST_UINT16 etype_appID,
  ST_INT8 errorCode,
  ST_INT8 serviceType);

/* ltrkUpdateMsvcbTrk updates MsvcbTrk (or UsvcbTrk if multicast="false").  */
ST_VOID ltrkUpdateMsvcbTrk (MVL_NET_INFO *netInfo,
  MVL_DOM_CTRL *domCtrl,
  SMPVAL_PUBLISHER *smpvalPublisher,
  ST_INT8 errorCode,
  ST_INT8 serviceType);

ST_VOID ltrkUpdateCtlTrk (MVL_NET_INFO *netInfo,
  MVL_DOM_CTRL *domCtrl,
  ST_CHAR *varName,
  LTRK_CTL_TRK_VALUES *CtlTrkValues);

/************************************************************************/
/* IEC 61850 Log related global variables.                              */
/************************************************************************/
extern MVL61850_LOG_CTRL *mvl61850_log_ctrl_list;       /* Linked list of 61850 LCBs.*/

/************************************************************************/
/* IEC 61850 Control related global variables.                          */
/************************************************************************/
/* Linked list of direct-with-enhanced-security controls in progress    */
extern MVL61850_CTL_DIRECT_ENHANCED *mvl61850_ctl_direct_enhanced_list;

/* User must set these function pointers to implement IEC 61850 Controls in a Server. */
extern ST_VOID (*u_mvl61850_ctl_oper_begin) (ST_CHAR *oper_ref);
extern ST_VOID (*u_mvl61850_ctl_oper_end) (MVL_NET_INFO *net_info, ST_CHAR *oper_ref, MVL_VAR_ASSOC *base_var, MVLAS_WR_VA_CTRL *wrVaCtrl);


/************************************************************************/
#ifdef __cplusplus
}
#endif

#endif /* MVL_UCA_INCLUDED */

