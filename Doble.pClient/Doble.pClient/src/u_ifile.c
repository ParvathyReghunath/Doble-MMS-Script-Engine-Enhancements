/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*      (c) Copyright Systems Integration Specialists Company, Inc.,    */
/*      2017-2018 All Rights Reserved                                   */
/*                                                                      */
/* MODULE NAME : u_ifile.c                                              */
/* PRODUCT(S)  : MMS Lite                                               */
/*                                                                      */
/* MODULE DESCRIPTION :                                                 */
/*      MMS File service indication functions for 32-bit or 64-bit      */
/*      applications. The "frsmid" is an index into an array of FILE    */
/*      pointers, so it works with any size pointer.                    */
/*                                                                      */
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :                            */
/*      u_mvl_fopen_ind                                                 */
/*      u_mvl_fread_ind                                                 */
/*      u_mvl_fclose_ind                                                */
/*      u_mvl_fdir_ind                                                  */
/*      u_mvl_fdelete_ind                                               */
/*      u_mvl_frename_ind                                               */
/*      u_mvl_obtfile_ind                                               */
/*                                                                      */
/* MODIFICATION LOG :                                                   */
/*  Date     Who  Comments                                              */
/* --------  ---  ----------------------------------------------------  */
/* 09/14/18  JRB  Use str_bld_filepath to build filepath string.        */
/*                u_mvl_fopen_ind: close file if anything fails.        */
/* 04/14/17  JRB  Created to use in any application.                    */
/************************************************************************/
#include "glbtypes.h"
#include "sysincs.h"
#include "mvl_defs.h"
#include "mvl_log.h"
#include "mmsop_en.h"   /* need MMS_FOPEN_EN, etc. defines      */
#include "str_util.h"
#include <errno.h>      /* for ENOENT, etc. */

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/************************************************************************/
/*                      mvl_fp_to_frsmid                                */
/* Use FILE pointer to generate "frsmid".                               */
/************************************************************************/
ST_INT32 mvl_fp_to_frsmid (MVL_NET_INFO *net_info, FILE *fp)
  {
  ST_INT32 j;
  for (j = 0; j < MAX_FILES_OPEN_PER_CONN; j++)
    {
    if (net_info->file_array [j] == NULL)
      {
      net_info->file_array [j] = fp; /* save file ptr here */
      break;
      }
    }
  if (j >= MAX_FILES_OPEN_PER_CONN)
    j = -1;   /* error, array is full */
  return (j); /* return array index as frsmid (-1 on error)  */
  }

/************************************************************************/
/*                      mvl_frsmid_to_fp                                */
/* Use "frsmid" to find FILE pointer.                                   */
/************************************************************************/
FILE *mvl_frsmid_to_fp (MVL_NET_INFO *net_info, ST_INT32 frsmid)
  {
  FILE *fp;
  if (frsmid >= 0 && frsmid < MAX_FILES_OPEN_PER_CONN)
    fp = net_info->file_array [frsmid];
  else
    fp = NULL;  /* frsmid is invalid  */
  return (fp);
  }

/************************************************************************/
/*                      mvl_frsmid_remove                               */
/* Remove "frsmid" from array of open files for this connection.        */
/************************************************************************/
ST_RET mvl_frsmid_remove (MVL_NET_INFO *net_info, ST_INT32 frsmid)
  {
  ST_RET retcode;
  if (frsmid >= 0 && frsmid < MAX_FILES_OPEN_PER_CONN)
    {
    net_info->file_array [frsmid] = NULL;  /* clear this array entry */
    retcode = SD_SUCCESS;
    }
  else
    retcode = SD_FAILURE;  /* frsmid is invalid  */
  return (retcode);
  }

#if (MMS_FOPEN_EN & RESP_EN)
/************************************************************************/
/*                      u_mvl_fopen_ind                                 */
/************************************************************************/
ST_VOID u_mvl_fopen_ind (MVL_IND_PEND *indCtrl)
  {
  FILE *fp;
  FOPEN_RESP_INFO resp_info;
  struct stat  stat_buf;
  ST_CHAR filepath [MAX_FILE_NAME + 1];

  if (str_bld_filepath (mvl_fdir_resp_base_path, indCtrl->u.fopen.filename, filepath, sizeof(filepath)) != SD_SUCCESS)
    {
    MVL_LOG_NERR1 ("File Open file name '%s' is not allowed", indCtrl->u.fopen.filename);
    _mplas_err_resp (indCtrl, MMS_ERRCLASS_FILE, MMS_ERRCODE_FILE_ACCESS_DENIED);
    return;
    }

  fp = fopen (filepath, "rb"); /* CRITICAL: use "b" flag for binary transfer*/
  if (fp == NULL)
    {
    if (errno == ENOENT)
      /* This response is required for IEC 61850 conformance test "FtN1".*/
      _mplas_err_resp (indCtrl, MMS_ERRCLASS_FILE, MMS_ERRCODE_FILE_NON_EXISTENT);
    else
      _mplas_err_resp (indCtrl, MMS_ERRCLASS_FILE, MMS_ERRCODE_FILE_ACCESS_DENIED);
    return;
    }
  if (fseek (fp, indCtrl->u.fopen.init_pos, SEEK_SET))
    {
    _mplas_err_resp (indCtrl, MMS_ERRCLASS_FILE, MMS_ERRCODE_POSITION_INVALID);
    fclose (fp);  /* don't want open file hanging around  */
    return;
    }

  /* Generate "frsmid" for response.  */
  resp_info.frsmid = mvl_fp_to_frsmid (indCtrl->event->net_info, fp);
  if (resp_info.frsmid < 0)
    {
    /* Can't generate frsmid. Close this file and send error response.*/
    fclose (fp);
    _mplas_err_resp (indCtrl, MMS_ERRCLASS_FILE, MMS_ERRCODE_OTHER);
    return;
    }

  if (fstat (fileno (fp), &stat_buf))
    {                                       /* Can't get file size or time  */
    _mplas_err_resp (indCtrl, MMS_ERRCLASS_FILE, MMS_ERRCODE_OTHER);
    fclose (fp);
    return;
    }
  else
    {
    resp_info.ent.fsize    = stat_buf.st_size;
    resp_info.ent.mtimpres = SD_TRUE;
    resp_info.ent.mtime    = stat_buf.st_mtime;
    }

  indCtrl->u.fopen.resp_info = &resp_info;
  mplas_fopen_resp (indCtrl);
  }
#endif  /* MMS_FOPEN_EN & RESP_EN       */


#if (MMS_FREAD_EN & RESP_EN)
/************************************************************************/
/*                      u_mvl_fread_ind                                 */
/************************************************************************/
ST_VOID u_mvl_fread_ind (MVL_IND_PEND *indCtrl)
  {
  FILE *fp;
  ST_UCHAR *tmp_buf;
  MVLAS_FREAD_CTRL *fread_ctrl = &indCtrl->u.fread;
  FREAD_RESP_INFO resp_info;

  /* Use frsmid to find (FILE *).       */
  fp = mvl_frsmid_to_fp (indCtrl->event->net_info, fread_ctrl->req_info->frsmid);
  if (fp == NULL)
    { /* client passed invalid frsmid */
    _mplas_err_resp (indCtrl, MMS_ERRCLASS_FILE, MMS_ERRCODE_FILE_NON_EXISTENT);
    return;
    }

  /* Do NOT read more than "max_size".                          */
  tmp_buf = (ST_UCHAR *) chk_malloc (fread_ctrl->max_size);

  resp_info.fd_len = fread (tmp_buf, 1, fread_ctrl->max_size, fp);
  if (resp_info.fd_len == 0  &&  ferror (fp))
    {
    _mplas_err_resp (indCtrl, MMS_ERRCLASS_FILE, MMS_ERRCODE_OTHER);
    return;
    }

  resp_info.filedata = tmp_buf;
  if (resp_info.fd_len == fread_ctrl->max_size)
    resp_info.more_follows = SD_TRUE;
  else
    resp_info.more_follows = SD_FALSE;

  fread_ctrl->resp_info = &resp_info;
  mplas_fread_resp (indCtrl);
  chk_free (tmp_buf);           /* Temporary buffer     */
  }
#endif  /* #if (MMS_FREAD_EN & RESP_EN) */

#if (MMS_FCLOSE_EN & RESP_EN)
/************************************************************************/
/*                      u_mvl_fclose_ind                                        */
/************************************************************************/
ST_VOID u_mvl_fclose_ind (MVL_IND_PEND *indCtrl)
  {
  FILE *fp;
  MVLAS_FCLOSE_CTRL *fclose_ctrl = &indCtrl->u.fclose;

  /* Use frsmid to find (FILE *).       */
  fp = mvl_frsmid_to_fp (indCtrl->event->net_info, fclose_ctrl->req_info->frsmid);
  if (fp == NULL)
    { /* client passed invalid frsmid */
    _mplas_err_resp (indCtrl, MMS_ERRCLASS_FILE, MMS_ERRCODE_FILE_NON_EXISTENT);
    return;
    }
  else
    mvl_frsmid_remove (indCtrl->event->net_info,  fclose_ctrl->req_info->frsmid);

  if (fclose (fp))
    _mplas_err_resp (indCtrl, MMS_ERRCLASS_FILE, MMS_ERRCODE_OTHER);
  else
    mplas_fclose_resp (indCtrl);
  }
#endif  /* #if (MMS_FCLOSE_EN & RESP_EN)        */

#if (MMS_FDIR_EN & RESP_EN)
/************************************************************************/
/*                      u_mvl_fdir_ind                                  */
/************************************************************************/
ST_VOID u_mvl_fdir_ind (MVL_IND_PEND *indCtrl)
  {
  /* This function works for _WIN32, linux, and other Posix systems.    */
  /* It may need to be ported for other systems.                        */
  mvlas_fdir_resp (indCtrl);
  }
#endif  /* #if (MMS_FDIR_EN & RESP_EN)  */

#if (MMS_FDELETE_EN & RESP_EN)
/************************************************************************/
/*                      u_mvl_fdelete_ind                               */
/************************************************************************/
ST_VOID u_mvl_fdelete_ind (MVL_IND_PEND *indCtrl)
  {
  ST_CHAR filepath [MAX_FILE_NAME + 1];
  MVLAS_FDELETE_CTRL *fdelete_ctrl = &indCtrl->u.fdelete;

  if (str_bld_filepath (mvl_fdir_resp_base_path, fdelete_ctrl->filename, filepath, sizeof(filepath)) != SD_SUCCESS)
    {
    MVL_LOG_NERR1 ("File Delete file name '%s' is not allowed", fdelete_ctrl->filename);
    _mplas_err_resp (indCtrl, MMS_ERRCLASS_FILE, MMS_ERRCODE_FILE_ACCESS_DENIED);
    return;
    }

  /* Use the ANSI "remove" function if available on your OS.            */
  /* Otherwise, use the appropriate function for your OS.               */
  if (remove (filepath))
    _mplas_err_resp (indCtrl, MMS_ERRCLASS_FILE, MMS_ERRCODE_OTHER);
  else
    mplas_fdelete_resp (indCtrl);
  }
#endif  /* #if (MMS_FDELETE_EN & RESP_EN)       */

#if (MMS_FRENAME_EN & RESP_EN)
/************************************************************************/
/*                      u_mvl_frename_ind                               */
/************************************************************************/
ST_VOID u_mvl_frename_ind (MVL_IND_PEND *indCtrl)
  {
  ST_CHAR curfilepath [MAX_FILE_NAME + 1];
  ST_CHAR newfilepath [MAX_FILE_NAME + 1];
  MVLAS_FRENAME_CTRL *frename_ctrl = &indCtrl->u.frename;

  /* NOTE: We must add the path to curfilename AND newfilename. */
  if (str_bld_filepath (mvl_fdir_resp_base_path, frename_ctrl->curfilename, curfilepath, sizeof(curfilepath)) != SD_SUCCESS)
    {
    MVL_LOG_NERR1 ("File Rename Current file name '%s' is not allowed", frename_ctrl->curfilename);
    _mplas_err_resp (indCtrl, MMS_ERRCLASS_FILE, MMS_ERRCODE_FILE_ACCESS_DENIED);
    return;
    }

  if (str_bld_filepath (mvl_fdir_resp_base_path, frename_ctrl->newfilename, newfilepath, sizeof(newfilepath)) != SD_SUCCESS)
    {
    MVL_LOG_NERR1 ("File Rename New file name '%s' is not allowed", frename_ctrl->newfilename);
    _mplas_err_resp (indCtrl, MMS_ERRCLASS_FILE, MMS_ERRCODE_FILE_ACCESS_DENIED);
    return;
    }

  /* Use the ANSI "rename" function if available on your OS.            */
  /* Otherwise, use the appropriate function for your OS.               */
  if (rename (curfilepath, newfilepath))
    {
    _mplas_err_resp (indCtrl, MMS_ERRCLASS_FILE, MMS_ERRCODE_OTHER);
    }
  else
    mplas_frename_resp (indCtrl);    /* create response to confirm completion */
  }
#endif  /* #if (MMS_FRENAME_EN & RESP_EN)       */

#if (MMS_OBTAINFILE_EN & RESP_EN)
/************************************************************************/
/*                      u_mvl_obtfile_ind                               */
/************************************************************************/
ST_VOID u_mvl_obtfile_ind (MVL_IND_PEND *indCtrl)
  {
  mvlas_obtfile_resp (indCtrl);      /* indicate state machine starting */
  }
#endif  /* #if (MMS_OBTAINFILE_EN & RESP_EN)    */

