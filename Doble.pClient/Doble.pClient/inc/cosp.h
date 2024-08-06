/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*      (c) Copyright Systems Integration Specialists Company, Inc.,    */
/*      1997, All Rights Reserved                                       */
/*                                                                      */
/*                      PROPRIETARY AND CONFIDENTIAL                    */
/*                                                                      */
/* MODULE NAME : cosp.h                                                 */
/* PRODUCT(S)  : MOSI Stack (over TP4)                                  */
/*                                                                      */
/* MODULE DESCRIPTION :                                                 */
/*              This header file defines variable and functions         */
/*              internal to the COSP (decoding and encoding).           */
/*                                                                      */
/*                                                                      */
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :                            */
/*                                                                      */
/*                                                                      */
/* MODIFICATION LOG :                                                   */
/*  Date     Who   Rev                  Comments                        */
/* --------  ---  ------   -------------------------------------------  */
/* 11/17/09  JRB    04     Break cosp_enc_cn_ac into ..enc_cn & ..enc_ac*/
/*                         Del unneeded proto cosp_envelope_len.        */
/* 09/12/01  JRB    03     Reverse cosp_enc_cn_ac change (not needed).  */
/* 08/01/01  JRB    02     Del rem_addr from cosp_enc_cn_ac proto.      */
/* 05/27/97  JRB   7.00    MMSEASE 7.0 release.                         */
/* 01/13/97  EJV    01     Created                                      */
/************************************************************************/
#ifndef COSP_INCLUDED
#define COSP_INCLUDED


#ifdef __cplusplus
extern "C" {
#endif


extern ST_UCHAR   cosp_only_ssel [1+MAX_SSEL_LEN];


        /*======================================================*/
        /*                                                      */
        /*      C O N N E C T I O N   S T A T E S               */
        /*                                                      */
        /*======================================================*/


/* Valid states, the first IDLE state must be 0) */
#define COSP_CSTATE_IDLE             0          /* idle, transport not connected*/
#define COSP_CSTATE_WAIT_TCON_CNF    1          /* wait T-CONNECT.cnf           */
#define COSP_CSTATE_IDLE_TCON        100        /* idle, transport connected    */
#define COSP_CSTATE_WAIT_AC          2          /* wait for ACCEPT SPDU         */
#define COSP_CSTATE_WAIT_DN          3          /* wait for DISCONNECT SPDU     */
#define COSP_CSTATE_WAIT_CON_RSP     8          /* wait S-CONNECT.rsp           */
#define COSP_CSTATE_WAIT_REL_RSP     9          /* wait for S-RELEASE.rsp       */
#define COSP_CSTATE_WAIT_TDISCON_IND 16         /* wait T-DISCONNECT.ind        */
#define COSP_CSTATE_DATA_XFER        713        /* Data Transfer state          */

        /*======================================================*/
        /*                                                      */
        /*      C O S P   P R O T O C O L   D E F I N E S       */
        /*                                                      */
        /*  D E C O D E D   S P D U   S T R U C T U R E S       */
        /*                                                      */
        /*======================================================*/


/* Encoding for supported COSP versions codes */
#define COSP_VER1       (ST_UCHAR) 1
#define COSP_VER2       (ST_UCHAR) 2


/* Encoding of SPDU Session Identifier (SI) codes */
#define COSP_SI_DATA            (ST_UCHAR)  1
#define COSP_SI_GIVE_TOKEN      (ST_UCHAR)  1  /* this is correct DT=GIVE-TOKEN */
#define COSP_SI_FINISH          (ST_UCHAR)  9
#define COSP_SI_DISCON          (ST_UCHAR) 10
#define COSP_SI_REFUSE          (ST_UCHAR) 12
#define COSP_SI_CONNECT         (ST_UCHAR) 13
#define COSP_SI_ACCEPT          (ST_UCHAR) 14
#define COSP_SI_ABORT           (ST_UCHAR) 25
#define COSP_SI_AB_ACCEPT       (ST_UCHAR) 26


/* struct used to store decoded params from CONNECT or ACCEPT SPDU      */
typedef struct tagCOSP_CN_AC
  {
  ST_UCHAR              prot_option;
  ST_UINT16     initiator_tsdu_size;
  ST_UINT16     responder_tsdu_size;
  ST_UCHAR              ver_num;
  ST_UCHAR              ses_urequir [2];        /* byte0 (bits 9-16), byte1 (bits 1-8)*/
  ST_UCHAR              loc_ssel [1+MAX_SSEL_LEN];
  ST_UCHAR              rem_ssel [1+MAX_SSEL_LEN];
  ST_UINT       udata_len;              /* SS-user data length          */
  ST_UCHAR             *udata_ptr;              /* SS-user data pointer         */
  }COSP_CN_AC;


/* Encoding for Reason Code PI in a REFUSE SPDU generated by SS-user */
#define COSP_RF_REASON_U_NOT_SPECIFIED  (ST_UCHAR)     0
#define COSP_RF_REASON_U_CONGESTION     (ST_UCHAR)     1
#define COSP_RF_REASON_U_REJECT         (ST_UCHAR)     2
/* Encoding for Reason Code PI in a REFUSE SPDU generated by SS-provider */
#define COSP_RF_REASON_S_INV_SSEL       (ST_UCHAR) 128+1
#define COSP_RF_REASON_S_NOT_ATTACHED   (ST_UCHAR) 128+2
#define COSP_RF_REASON_S_CONGESTION     (ST_UCHAR) 128+3
#define COSP_RF_REASON_S_INV_VERSION    (ST_UCHAR) 128+4
#define COSP_RF_REASON_S_NOT_SPECIFIED  (ST_UCHAR) 128+5
#define COSP_RF_REASON_S_RESTRICTIONS   (ST_UCHAR) 128+6


/* struct used to store decoded params from REFUSE SPDU */
typedef struct tagCOSP_RF
  {
  ST_BOOLEAN    disconnect;             /* if SD_TRUE then disconnect transport */
  ST_UCHAR              ver_num;
  ST_UCHAR              reason;                 /* reason code for refuse connection    */
  ST_UCHAR              ses_urequir [2];        /* byte0 (bits 9-16), byte1 (bits 1-8)  */
  ST_UINT       udata_len;              /* SS-user data length          */
  ST_UCHAR             *udata_ptr;              /* SS-user data pointer         */
  }COSP_RF;


#define COSP_P_AB_SPDU_LEN      9       /* P-ABORT is fixed in size     */

/* Encoding for Transport Disconnect PI in an ABORT (and FINISH) SPDU */
#define COSP_TCONN_KEEP                 (ST_UCHAR) 0x00
#define COSP_TCONN_RELEASE              (ST_UCHAR) 0x01
/* | with one reason code below */
#define COSP_AB_REASON_USER_ABORT       (ST_UCHAR) 0x02
#define COSP_AB_REASON_PROT_ERROR       (ST_UCHAR) 0x04
#define COSP_AB_REASON_NO_REASON        (ST_UCHAR) 0x08
#define COSP_AB_REASON_IMPLEMENTATION   (ST_UCHAR) 0x10


/* struct used to store decoded params from ABORT SPDU  */
typedef struct tagCOSP_AB
  {
  ST_BOOLEAN    disconnect;             /* if SD_TRUE then disconnect transport */
  ST_UCHAR              reason;                 /* reason code for abort connection     */
  ST_UINT       reflect_par_len;        /* Reflect param length         */
  ST_UCHAR              reflect_par [9];        /* Reflect param                */
  ST_UINT       udata_len;              /* SS-user data length          */
  ST_UCHAR             *udata_ptr;              /* SS-user data pointer         */
  }COSP_AB;


/* struct used to store decoded params from FINISH SPDU */
typedef struct tagCOSP_FN_DN
  {
  ST_BOOLEAN    disconnect;             /* SD_TRUE to discon transport (FN)*/
  ST_UINT       udata_len;              /* SS-user data length          */
  ST_UCHAR             *udata_ptr;              /* SS-user data pointer         */
  }COSP_FN_DN;

/* struct used to store decoded params from DATA SPDU   */
typedef struct tagCOSP_DT
  {
  ST_UINT       udata_len;              /* SS-user data length          */
  ST_UCHAR             *udata_ptr;              /* SS-user data pointer         */
  }COSP_DT;



        /*======================================================*/
        /*                                                      */
        /*      E N C O D E / D E C O D E   F U N C T I O N S   */
        /*                                                      */
        /*======================================================*/

/* cosp_enc.c */
ST_RET cosp_enc_cn (
        ACSE_CONN *con,         /* Connection info                      */
        char **spdu_ptr,        /* Pointer where to return pointer to SPDU*/
        ST_UINT *spdu_len);     /* Pointer where to return the SPDU len */
ST_RET cosp_enc_ac (
        ACSE_CONN *con,         /* Connection info                      */
        char **spdu_ptr,        /* Pointer where to return pointer to SPDU*/
        ST_UINT *spdu_len);     /* Pointer where to return the SPDU len */
ST_RET cosp_enc_rf    (ACSE_CONN *con, char **spdu_ptr, ST_UINT *spdu_len, ST_UCHAR reason);
ST_RET cosp_enc_fn_dn (ACSE_CONN *con, char **spdu_ptr, ST_UINT *spdu_len, ST_UCHAR spdu_type);
ST_RET cosp_enc_u_ab  (ACSE_CONN *con, char **spdu_ptr, ST_UINT *spdu_len);
ST_RET cosp_enc_p_ab  (char *spdu_ptr, ST_INT err_code);
ST_RET cosp_enc_dt    (ACSE_CONN *con, char **spdu_ptr, ST_UINT *spdu_len);

/* cosp_dec.c */
ST_RET cosp_dec_cn_ac (COSP_CN_AC *dec_par, char *spdu_buf, ST_UINT spdu_len, ST_UCHAR spdu_type);
ST_RET cosp_dec_rf    (COSP_RF    *dec_par, char *spdu_buf, ST_UINT spdu_len);
ST_RET cosp_dec_fn_dn (COSP_FN_DN *dec_par, char *spdu_buf, ST_UINT spdu_len, ST_UCHAR spdu_type);
ST_RET cosp_dec_ab    (COSP_AB    *dec_par, char *spdu_buf, ST_UINT spdu_len);
ST_RET cosp_dec_dt    (COSP_DT    *dec_par, char *spdu_buf, ST_UINT spdu_len);

/* cospmain.c */
ST_RET cosp_process_connect (ACSE_CONN *con, ST_UINT spdu_len, char *spdu_buf);


#ifdef __cplusplus
}
#endif

#endif  /* end of 'already included'    */



