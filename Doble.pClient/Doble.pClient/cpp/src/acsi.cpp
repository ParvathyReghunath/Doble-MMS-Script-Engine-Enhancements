/*******************************************************************************
File      : acsi.cpp
By        : Arun Lal K M
Date      : 18th Jun 2019
Reference : iec61850-8-1{ed2.0}
6.2.1 Client/server services
Table 2 – Services requiring client/server Communication Profile
*******************************************************************************/


/*include*/
#include "acsi.hpp"

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR* SD_CONST thisFileName = __FILE__;
#endif

/*Global variables*/
/* NOTE: this global variable can be avoided if a (MVL_CONTROL_INFO *)  */
/*   is saved in the "user_info" member of MVL_NET_INFO, but to do so,  */
/*   you must make sure "user_info" is not used for something else.     */
extern "C" MY_CONTROL_INFO my_control_info;
extern "C" MVL_TYPE_ID LastApplErrorTypeId;  /* type for decoding LastApplError    */

extern "C" extern ST_RET all_obj_destroy_scd (MVL_VMD_CTRL** vmd_ctrl_arr, ST_UINT vmd_count);
/*extern */
extern ST_BOOLEAN doIt = SD_TRUE;

/*mServer*/
mServer::mServer ()
    {
    }

mServer::~mServer ()
    {
    }

int mServer::sGetServerDirectory (MVL_NET_INFO* pNetInfo, char* getafter, STRUCT_GET_LIST* list)
    {
    int ret = SD_FAILURE;
    NAMELIST_REQ_INFO getnam_req = { 0 };
    MVL_REQ_PEND* reqCtrl = NULL;
    ST_INT timeOut = MAX_GET_LD_TIMEOUT;
    NAMELIST_RESP_INFO* getnam_resp = NULL;
    ST_CHAR** nptr = NULL;
    int i = 0;

    getnam_req.cs_objclass_pres = SD_FALSE;
    getnam_req.obj.mms_class = 9;
    getnam_req.objscope = VMD_SPEC;
    if (strcmp (getafter, "") == 0)
        {
        getnam_req.cont_after_pres = SD_FALSE;
        }
    else
        {
        getnam_req.cont_after_pres = SD_TRUE;
        strncpy (getnam_req.continue_after, getafter, sizeof (getnam_req.continue_after));
        }

    ret = mvla_getnam (pNetInfo, &getnam_req, &reqCtrl);

    if (ret == SD_SUCCESS)
        {
        ret = waitReqDoneEx (reqCtrl, timeOut);

        if (ret == SD_SUCCESS)
            {
            getnam_resp = (NAMELIST_RESP_INFO*)reqCtrl->u.ident.resp_info;

            list->moreFollows = getnam_resp->more_follows;

            nptr = (ST_CHAR * *)(getnam_resp + 1);
            if (list->moreFollows)
                {
                strncpy (list->continueAfter, nptr[getnam_resp->num_names - 1], sizeof (list->continueAfter));
                }

            if (getnam_resp->num_names > 0)
                {
                list->count += getnam_resp->num_names;
                for (i = 0; i < getnam_resp->num_names; ++i)
                    {
                    STRUCT_LEAF* leaf = NULL;

                    leaf = (STRUCT_LEAF*)chk_calloc (1, sizeof (STRUCT_LEAF));
                    if (leaf == NULL)
                        {
                        pClientLog ("ALLOC", "ERROR", -1);
                        }
                    leaf->pName = (char*)chk_calloc (1, strlen (nptr[i]) + 1);
                    if (leaf->pName == NULL)
                        {
                        pClientLog ("ALLOC", "ERROR", -1);
                        }

                    strcpy (leaf->pName, nptr[i]);
                    list_add_last (&list->pListItem, leaf);
                    }
                }
            else
                {
                pClientLog ("CODE 1", "ERROR", -1);
                }
            }
        }
    mvl_free_req_ctrl (reqCtrl);

    return ret;
    }

/*mAssociation*/
mAssociation::mAssociation ()
    {
    }

mAssociation::~mAssociation ()
    {
    }

int mAssociation::addToDib (char* name, char* ip)
    {
    return addToDibTable (name, ip);
    }

int mAssociation::removeFromDib (char* name)
    {
    removeFromDibTable (name);
    return SD_SUCCESS;
    }

int mAssociation::sAssociate (char* name, MVL_NET_INFO** pNetInfo)
    {
    int i = 0;
    int ret = SD_FAILURE;

    for (i = 0; i < MAX_CONNECT_ATTEMPT; i++)
        {
        *pNetInfo = connectToServer (mvlCfg.local_ar_name, name);
        if (*pNetInfo == NULL)
            {
            sMsSleep (1);
            }
        else
            {
            ret = SD_SUCCESS;
            break;
            }
        }

    return ret;
    }

int mAssociation::sRelease (MVL_NET_INFO* pNetInfo)
    {
    return disconnectFromServer (pNetInfo);
    }

/*mLogicalDevice*/
mLogicalDevice::mLogicalDevice ()
    {
    }

mLogicalDevice::~mLogicalDevice ()
    {
    }

int mLogicalDevice::sGetLogicalDeviceDirectory (MVL_NET_INFO* pNetInfo, char* domName, char* getafter, STRUCT_GET_LIST* list)
    {
    int ret = SD_FAILURE;
    NAMELIST_REQ_INFO getnam_req = { 0 };
    MVL_REQ_PEND* reqCtrl = NULL;
    ST_INT timeOut = MAX_GET_LN_TIMEOUT;
    NAMELIST_RESP_INFO* getnam_resp = NULL;
    ST_CHAR** nptr = NULL;
    int i = 0;

    getnam_req.cs_objclass_pres = SD_FALSE;
    getnam_req.obj.mms_class = 0;
    getnam_req.objscope = DOM_SPEC;

    strncpy (getnam_req.dname, domName, sizeof (getnam_req.dname));

    if (strcmp (getafter, "") == 0)
        {
        getnam_req.cont_after_pres = SD_FALSE;
        }
    else
        {
        getnam_req.cont_after_pres = SD_TRUE;
        strncpy (getnam_req.continue_after, getafter, sizeof (getnam_req.continue_after));
        }

    ret = mvla_getnam (pNetInfo, &getnam_req, &reqCtrl);

    if (ret == SD_SUCCESS)
        {
        ret = waitReqDoneEx (reqCtrl, timeOut);

        if (ret == SD_SUCCESS)
            {
            getnam_resp = (NAMELIST_RESP_INFO*)reqCtrl->u.ident.resp_info;

            list->moreFollows = getnam_resp->more_follows;

            nptr = (ST_CHAR * *)(getnam_resp + 1);
            if (list->moreFollows)
                {
                strncpy (list->continueAfter, nptr[getnam_resp->num_names - 1], sizeof (list->continueAfter));
                }

            if (getnam_resp->num_names > 0)
                {
                for (i = 0; i < getnam_resp->num_names; ++i)
                    {
                    if (strstr (nptr[i], "$") == 0)
                        {
                        STRUCT_LEAF* leaf = NULL;

                        leaf = (STRUCT_LEAF*)chk_calloc (1, sizeof (STRUCT_LEAF));
                        if (leaf == NULL)
                            {
                            pClientLog ("ALLOC", "ERROR", -1);
                            }
                        leaf->pName = (char*)chk_calloc (1, strlen (nptr[i]) + 1);
                        if (leaf->pName == NULL)
                            {
                            pClientLog ("ALLOC", "ERROR", -1);
                            }

                        strcpy (leaf->pName, nptr[i]);
                        list_add_last (&list->pListItem, leaf);
                        }
                    }
                }
            else
                {
                pClientLog ("CODE 2", "ERROR", -1);
                }
            }
        }
    mvl_free_req_ctrl (reqCtrl);

    return ret;
    }

/*mLogicalDevice*/
mLogicalNode::mLogicalNode ()
    {
    }

mLogicalNode::~mLogicalNode ()
    {
    }

int mLogicalNode::deleteTypeId (STRUCT_GET* pLn)
    {
    mvl_type_id_destroy (pLn->typeId);
    return SD_SUCCESS;
    }

int mLogicalNode::sGetLogicalNodeDirectory (MVL_NET_INFO* pNetInfo, char* domName, char* lnName, STRUCT_GET* pLn)
    {
    GETVAR_REQ_INFO getvar_req = { 0 };
    int ret = SD_FAILURE;
    MVL_REQ_PEND* reqCtrl = NULL;

    getvar_req.req_tag = GETVAR_NAME;
    getvar_req.name.object_tag = DOM_SPEC;
    getvar_req.name.domain_id = domName;
    getvar_req.name.obj_name.vmd_spec = lnName;

    ret = mvla_getvar (pNetInfo, &getvar_req, &reqCtrl);

    if (ret == SD_SUCCESS)
        {
        ret = waitReqDoneEx (reqCtrl, MAX_GET_LN_TIMEOUT);

        if (ret == SD_SUCCESS)
            {
            pLn->typeId = mvl_type_id_create (NULL,
                                              reqCtrl->u.getvar.resp_info->type_spec.data,
                                              reqCtrl->u.getvar.resp_info->type_spec.len);

            if (pLn->typeId != MVL_TYPE_ID_INVALID)
                {
                ret = mvl_get_runtime (pLn->typeId,
                                       &pLn->rtPtrOut,
                                       &pLn->numRtOut);

                mvl_type_id_destroy (pLn->typeId);
                }
            }
        }
    mvl_free_req_ctrl (reqCtrl);

    return ret;
    }

/*mLogicalDevice*/
mData::mData ()
    {
    }

mData::~mData ()
    {
    }

int mData::sGetDataValuesAsync (MVL_NET_INFO* pNetInfo,
                                char* domName,
                                char* varName,
                                int type,
                                int len,
                                char* dataSrc,
                                MVL_REQ_PEND** reqCtrl,
                                READ_REQ_INFO** req_info,
                                ST_DOUBLE* stopTime,
                                MVL_READ_RESP_PARSE_INFO** parse_info)
    {
    int ret = SD_FAILURE;

    MVL_TYPE_ID typeId = getVarRuntimeType (type, len); /*TODO: Arun Store runtime type so that we can skip this step*/

    if (typeId != MVL_TYPE_ID_INVALID)
        {
        ret = namedVarReadAsync (pNetInfo, varName, DOM_SPEC, domName, typeId, dataSrc, reqCtrl, req_info, parse_info);

        *stopTime = sGetMsTime () + (ST_DOUBLE)MAX_GET_DATA_TIMEOUT * 1000.0;
        }

    return ret;
    }

int mData::sSetStructDataValues (MVL_NET_INFO* pNetInfo, char* domName, char* varName, char* pData)
    {
    GETVAR_REQ_INFO getvar_req = { 0 };
    int ret = SD_FAILURE;
    MVL_REQ_PEND* reqCtrl = NULL;
    STRUCT_GET structData = { 0 };
    int i = 0;
    char* bPtr = NULL;
    char* cPtr = NULL;
    char* dPtr = NULL;
    int timeOut = MAX_SET_DATA_TIMEOUT;

    char* ctrl_data_buf = NULL;

    getvar_req.req_tag = GETVAR_NAME;
    getvar_req.name.object_tag = DOM_SPEC;
    getvar_req.name.domain_id = domName;
    getvar_req.name.obj_name.vmd_spec = varName;

    ret = mvla_getvar (pNetInfo, &getvar_req, &reqCtrl);

    if (ret == SD_SUCCESS)
        {
        ret = waitReqDoneEx (reqCtrl, MAX_GET_LN_TIMEOUT);

        if (ret == SD_SUCCESS)
            {
            structData.typeId = mvl_type_id_create (NULL,
                                                    reqCtrl->u.getvar.resp_info->type_spec.data,
                                                    reqCtrl->u.getvar.resp_info->type_spec.len);

            if (structData.typeId != MVL_TYPE_ID_INVALID)
                {
                ret = mvl_get_runtime (structData.typeId,
                                       &structData.rtPtrOut,
                                       &structData.numRtOut);

                if (ret == SD_SUCCESS)
                    {
                    ctrl_data_buf = (char*)chk_calloc (1, structData.rtPtrOut[0].offset_to_last);
                    if (pData)
                        {
                        ST_INT data_offset = 0;
                        for (i = 0; i < structData.numRtOut; i++)
                            {
                            char buf[1024] = { 0 };

                            if (structData.rtPtrOut[i].el_tag == RT_STR_START ||
                                structData.rtPtrOut[i].el_tag == RT_ARR_START ||
                                structData.rtPtrOut[i].el_tag == RT_STR_END ||
                                structData.rtPtrOut[i].el_tag == RT_ARR_END)
                                {
                                }
                            else
                                {
                                dPtr = get_next_string (&pData, "|");
                                cPtr = get_next_string (&dPtr, ",");

                                if (strcmp (structData.rtPtrOut[i].comp_name_ptr, cPtr) == 0)
                                    {
                                    VAL_STRUCT val = { 0 };

                                    memcpy ((ctrl_data_buf + data_offset),
                                            stringToVal (structData.rtPtrOut[i].el_tag, abs (structData.rtPtrOut[i].u.p.el_len), dPtr, &val),
                                            structData.rtPtrOut[i].el_size);
                                    //if (rt_type->el_tag == RT_INTEGER && rt_type->u.p.el_len == 4)
                                    //    {
                                    //    /**(ST_INT32*)(ctrl_data_buf + data_offset) = 9;*/
                                    //    }
                                    }
                                }

                            data_offset += structData.rtPtrOut[i].el_size;
                            assert (data_offset <= structData.rtPtrOut[0].offset_to_last);
                            }

                        my_control_info.cmd_term_num_va = -1;
                        strcpy (my_control_info.oper_name, varName);        /* save oper_name for later*/

                        /* NOTE: if this is SBO control and SBO read failed, this write       */
                        /*   to "Oper" should fail, but try it anyway.                        */
                        ret = named_var_write (pNetInfo, varName, DOM_SPEC, domName,  /* no domain name*/
                                               structData.typeId, ctrl_data_buf, timeOut);
                        if (ret != SD_SUCCESS)
                            {
                            printf ("Control Activation Response received: FAILURE\n");
                            /* If LastApplError was set by a Report, display its data. */
                            if (my_control_info.LastApplError.CntrlObj[0] != 0)
                                {
                                printf ("AddCause = %d\n", (ST_INT)my_control_info.LastApplError.AddCause);
                                }
                            }

                        /* if buffer was allocated, free it.  */
                        if (ctrl_data_buf)
                            {
                            chk_free (ctrl_data_buf);
                            }
                        }
                    }
                }

            mvl_type_id_destroy (structData.typeId);
            }
        }

    mvl_free_req_ctrl (reqCtrl);

    return ret;
    }

int mData::sGetStructDataValues (MVL_NET_INFO* pNetInfo, char* domName, char* varName, char* pData, int allocSize, unsigned char* dataSrc)
    {
    GETVAR_REQ_INFO getvar_req = { 0 };
    int ret = SD_FAILURE;
    MVL_REQ_PEND* reqCtrl = NULL;
    STRUCT_GET structData = { 0 };
    int i = 0;

    getvar_req.req_tag = GETVAR_NAME;
    getvar_req.name.object_tag = DOM_SPEC;
    getvar_req.name.domain_id = domName;
    getvar_req.name.obj_name.vmd_spec = varName;

    ret = mvla_getvar (pNetInfo, &getvar_req, &reqCtrl);

    if (ret == SD_SUCCESS)
        {
        ret = waitReqDoneEx (reqCtrl, MAX_GET_LN_TIMEOUT);

        if (ret == SD_SUCCESS)
            {
            ret = SD_FAILURE;
            structData.typeId = mvl_type_id_create (NULL,
                                                    reqCtrl->u.getvar.resp_info->type_spec.data,
                                                    reqCtrl->u.getvar.resp_info->type_spec.len);

            if (structData.typeId != MVL_TYPE_ID_INVALID)
                {
                ret = mvl_get_runtime (structData.typeId,
                                       &structData.rtPtrOut,
                                       &structData.numRtOut);

                if (ret == SD_SUCCESS)
                    {
                    ret = named_var_read (pNetInfo, varName, DOM_SPEC, domName, structData.typeId, dataSrc, MAX_SET_DATA_TIMEOUT);

                    if (ret == SD_SUCCESS)
                        {
                        char leaf[1024] = { 0 };
                        strcpy (pData, "");
                        if (structData.numRtOut == 1)
                            {
                            valToString (dataSrc, pData, structData.rtPtrOut[i].el_tag, abs (structData.rtPtrOut[i].u.p.el_len));
                            }
                        else
                            {
                            for (i = 0; i < structData.numRtOut; i++)
                                {
                                char buf[1024] = { 0 };
                                if (structData.rtPtrOut[i].el_tag == RT_STR_START ||
                                    structData.rtPtrOut[i].el_tag == RT_ARR_START)
                                    {
                                    if (structData.rtPtrOut[i].comp_name_ptr)
                                        {
                                        addStringToEnd (leaf, structData.rtPtrOut[i].comp_name_ptr);
                                        }
                                    dataSrc += structData.rtPtrOut[i].el_size;
                                    }
                                else if (structData.rtPtrOut[i].el_tag == RT_STR_END ||
                                         structData.rtPtrOut[i].el_tag == RT_ARR_END)
                                    {
                                    removeStringFromEnd (leaf);
                                    dataSrc += structData.rtPtrOut[i].el_size;
                                    }
                                else
                                    {
                                    addStringToEnd (leaf, structData.rtPtrOut[i].comp_name_ptr);
                                    valToString (dataSrc, buf, structData.rtPtrOut[i].el_tag, abs (structData.rtPtrOut[i].u.p.el_len));
                                    strcat (pData, leaf);
                                    strcat (pData, "=");
                                    strcat (pData, buf);
                                    strcat (pData, "\n");
                                    if (allocSize <= (int)strlen (pData))
                                        {
                                        chk_free (dataSrc);
                                        ret = SD_FAILURE;
                                        break;
                                        }
                                    dataSrc += structData.rtPtrOut[i].el_size;
                                    removeStringFromEnd (leaf);
                                    }
                                }
                            pData[strlen (pData)] = '\0';
                            }
                        }
                    }

                mvl_type_id_destroy (structData.typeId);
                }
            }
        }

    mvl_free_req_ctrl (reqCtrl);

    return ret;
    }

int mData::sGetDataValuesEx (MVL_NET_INFO* pNetInfo, char* domName, char* varName, char* pData)
    {
    GETVAR_REQ_INFO getvar_req = { 0 };
    int ret = SD_FAILURE;
    MVL_REQ_PEND* reqCtrl = NULL;
    STRUCT_GET structData = { 0 };
    unsigned char dataSrc[1024] = { 0 };
    int i = 0;

    getvar_req.req_tag = GETVAR_NAME;
    getvar_req.name.object_tag = DOM_SPEC;
    getvar_req.name.domain_id = domName;
    getvar_req.name.obj_name.vmd_spec = varName;

    ret = mvla_getvar (pNetInfo, &getvar_req, &reqCtrl);

    if (ret == SD_SUCCESS)
        {
        ret = waitReqDoneEx (reqCtrl, MAX_GET_LN_TIMEOUT);

        if (ret == SD_SUCCESS)
            {
            structData.typeId = mvl_type_id_create (NULL,
                                                    reqCtrl->u.getvar.resp_info->type_spec.data,
                                                    reqCtrl->u.getvar.resp_info->type_spec.len);

            if (structData.typeId != MVL_TYPE_ID_INVALID)
                {
                ret = mvl_get_runtime (structData.typeId,
                                       &structData.rtPtrOut,
                                       &structData.numRtOut);

                if (ret == SD_SUCCESS)
                    {
                    ret = named_var_read (pNetInfo, varName, DOM_SPEC, domName, structData.typeId, dataSrc, MAX_SET_DATA_TIMEOUT);

                    if (ret == SD_SUCCESS)
                        {
                        if (structData.numRtOut == 1)
                            {
                            valToString (dataSrc, pData, structData.rtPtrOut[i].el_tag, abs (structData.rtPtrOut[i].u.p.el_len));
                            }
                        else
                            {
                            ret = SD_FAILURE;
                            }
                        }
                    }

                mvl_type_id_destroy (structData.typeId);
                }
            }
        }

    mvl_free_req_ctrl (reqCtrl);

    return ret;
    }


int mData::sGetDataValues (MVL_NET_INFO* pNetInfo, char* domName, char* varName, int type, int len, char* pData)
    {
    int ret = SD_FAILURE;
    unsigned char dataSrc[1024] = { 0 };

    MVL_TYPE_ID typeId = getVarRuntimeType (type, len); /*TODO: Arun Store runtime type so that we can skip this step*/

    if (typeId != MVL_TYPE_ID_INVALID)
        {
        ret = named_var_read (pNetInfo, varName, DOM_SPEC, domName, typeId, dataSrc, MAX_SET_DATA_TIMEOUT);
        valToString (dataSrc, pData, type, abs (len));
        }

    return ret;
    }

int mData::sSetDataValuesEx (MVL_NET_INFO* pNetInfo, char* domName, char* varName, char* pData, char* addCause)
    {

    GETVAR_REQ_INFO getvar_req = { 0 };
    int ret = SD_FAILURE;
    MVL_REQ_PEND* reqCtrl = NULL;
    STRUCT_GET structData = { 0 };
    unsigned char dataSrc[1024] = { 0 };
    int i = 0;
    VAL_STRUCT val = { 0 };
    int error = 0;

    getvar_req.req_tag = GETVAR_NAME;
    getvar_req.name.object_tag = DOM_SPEC;
    getvar_req.name.domain_id = domName;
    getvar_req.name.obj_name.vmd_spec = varName;

    ret = mvla_getvar (pNetInfo, &getvar_req, &reqCtrl);

    if (ret == SD_SUCCESS)
        {
        ret = waitReqDoneEx (reqCtrl, MAX_GET_LN_TIMEOUT);

        if (ret == SD_SUCCESS)
            {
            structData.typeId = mvl_type_id_create (NULL,
                                                    reqCtrl->u.getvar.resp_info->type_spec.data,
                                                    reqCtrl->u.getvar.resp_info->type_spec.len);

            if (structData.typeId != MVL_TYPE_ID_INVALID)
                {
                ret = mvl_get_runtime (structData.typeId,
                                       &structData.rtPtrOut,
                                       &structData.numRtOut);

                if (ret == SD_SUCCESS)
                    {
                    if (structData.numRtOut == 1)
                        {
                        ret = named_var_write_ex (pNetInfo,
                                                  varName,
                                                  DOM_SPEC,
                                                  domName,
                                                  structData.typeId,
                                                  stringToVal (structData.rtPtrOut[i].el_tag, abs (structData.rtPtrOut[i].u.p.el_len), pData, &val),
                                                  MAX_GET_DATA_TIMEOUT,
                                                  &error);
                        if (ret == SD_FAILURE)
                            {
                            writeRespToString (error, addCause);
                            }
                        }
                    else
                        {
                        strcpy (addCause, "Trying to write a structure");
                        ret = SD_FAILURE;
                        }
                    }

                mvl_type_id_destroy (structData.typeId);
                }
            }
        }

    mvl_free_req_ctrl (reqCtrl);

    return ret;
    }

int mData::sSetDataValues (MVL_NET_INFO* pNetInfo, char* domName, char* varName, int type, int len, char* pData)
    {
    int ret = SD_FAILURE;
    VAL_STRUCT val = { 0 };

    MVL_TYPE_ID typeId = getVarRuntimeType (type, abs (len)); /*TODO: Arun Store runtime type so that we can skip this step*/

    if (typeId != MVL_TYPE_ID_INVALID)
        {
        ret = named_var_write (pNetInfo, varName, DOM_SPEC, domName, typeId, stringToVal (type, len, pData, &val), MAX_GET_DATA_TIMEOUT);
        }

    return ret;
    }

mBRCB::mBRCB ()
    {
    }

mBRCB::~mBRCB ()
    {
    }

int mBRCB::sGetBRCBValues (MVL_NET_INFO* pNetInfo, char* domName, char* varName, STRUCT_RCB* pRcb, unsigned char* dataSrc)
    {
    GETVAR_REQ_INFO getvar_req = { 0 };
    int ret = SD_FAILURE;
    MVL_REQ_PEND* reqCtrl = NULL;
    STRUCT_GET structData = { 0 };
    int i = 0;

    getvar_req.req_tag = GETVAR_NAME;
    getvar_req.name.object_tag = DOM_SPEC;
    getvar_req.name.domain_id = domName;
    getvar_req.name.obj_name.vmd_spec = varName;

    ret = mvla_getvar (pNetInfo, &getvar_req, &reqCtrl);

    if (ret == SD_SUCCESS)
        {
        ret = waitReqDoneEx (reqCtrl, MAX_GET_LN_TIMEOUT);

        if (ret == SD_SUCCESS)
            {
            structData.typeId = mvl_type_id_create (NULL,
                                                    reqCtrl->u.getvar.resp_info->type_spec.data,
                                                    reqCtrl->u.getvar.resp_info->type_spec.len);

            if (structData.typeId != MVL_TYPE_ID_INVALID)
                {
                ret = mvl_get_runtime (structData.typeId,
                                       &structData.rtPtrOut,
                                       &structData.numRtOut);

                if (ret == SD_SUCCESS)
                    {
                    ret = named_var_read (pNetInfo, varName, DOM_SPEC, domName, structData.typeId, dataSrc, MAX_SET_DATA_TIMEOUT);

                    if (ret == SD_SUCCESS)
                        {
                        get_last_string (pRcb->name, varName);
                        for (i = 0; i < structData.numRtOut; i++)
                            {
                            char buf[1024] = { 0 };
                            if (structData.rtPtrOut[i].el_tag == RT_STR_START ||
                                structData.rtPtrOut[i].el_tag == RT_ARR_START ||
                                structData.rtPtrOut[i].el_tag == RT_STR_END ||
                                structData.rtPtrOut[i].el_tag == RT_ARR_END)
                                {
                                dataSrc += structData.rtPtrOut[i].el_size;
                                }
                            else
                                {

                                if (strcmp (structData.rtPtrOut[i].comp_name_ptr, "RptID") == 0)
                                    {
                                    valToString (dataSrc, pRcb->RptID, structData.rtPtrOut[i].el_tag, abs (structData.rtPtrOut[i].u.p.el_len));
                                    }
                                else if (strcmp (structData.rtPtrOut[i].comp_name_ptr, "DatSet") == 0)
                                    {
                                    valToString (dataSrc, pRcb->DatSet, structData.rtPtrOut[i].el_tag, abs (structData.rtPtrOut[i].u.p.el_len));
                                    }
                                else if (strcmp (structData.rtPtrOut[i].comp_name_ptr, "OptFlds") == 0)
                                    {
                                    valToString (dataSrc, pRcb->OptFlds, structData.rtPtrOut[i].el_tag, abs (structData.rtPtrOut[i].u.p.el_len));
                                    }
                                else if (strcmp (structData.rtPtrOut[i].comp_name_ptr, "TrgOps") == 0)
                                    {
                                    valToString (dataSrc, pRcb->TrgOps, structData.rtPtrOut[i].el_tag, abs (structData.rtPtrOut[i].u.p.el_len));
                                    }
                                else if (strcmp (structData.rtPtrOut[i].comp_name_ptr, "BufTm") == 0)
                                    {
                                    valToString (dataSrc, pRcb->BufTm, structData.rtPtrOut[i].el_tag, abs (structData.rtPtrOut[i].u.p.el_len));
                                    }
                                else if (strcmp (structData.rtPtrOut[i].comp_name_ptr, "ConfRev") == 0)
                                    {
                                    valToString (dataSrc, pRcb->ConfRev, structData.rtPtrOut[i].el_tag, abs (structData.rtPtrOut[i].u.p.el_len));
                                    }

                                dataSrc += structData.rtPtrOut[i].el_size;
                                }
                            }
                        }
                    }

                mvl_type_id_destroy (structData.typeId);
                }
            }
        }

    mvl_free_req_ctrl (reqCtrl);

    return ret;
    }

mURCB::mURCB ()
    {
    }

mURCB::~mURCB ()
    {
    }

int mURCB::sGetURCBValues (MVL_NET_INFO* pNetInfo, char* domName, char* varName, STRUCT_RCB* pRcb, unsigned char* dataSrc)
    {
    GETVAR_REQ_INFO getvar_req = { 0 };
    int ret = SD_FAILURE;
    MVL_REQ_PEND* reqCtrl = NULL;
    STRUCT_GET structData = { 0 };
    int i = 0;

    getvar_req.req_tag = GETVAR_NAME;
    getvar_req.name.object_tag = DOM_SPEC;
    getvar_req.name.domain_id = domName;
    getvar_req.name.obj_name.vmd_spec = varName;

    ret = mvla_getvar (pNetInfo, &getvar_req, &reqCtrl);

    if (ret == SD_SUCCESS)
        {
        ret = waitReqDoneEx (reqCtrl, MAX_GET_LN_TIMEOUT);

        if (ret == SD_SUCCESS)
            {
            structData.typeId = mvl_type_id_create (NULL,
                                                    reqCtrl->u.getvar.resp_info->type_spec.data,
                                                    reqCtrl->u.getvar.resp_info->type_spec.len);

            if (structData.typeId != MVL_TYPE_ID_INVALID)
                {
                ret = mvl_get_runtime (structData.typeId,
                                       &structData.rtPtrOut,
                                       &structData.numRtOut);

                if (ret == SD_SUCCESS)
                    {
                    ret = named_var_read (pNetInfo, varName, DOM_SPEC, domName, structData.typeId, dataSrc, MAX_SET_DATA_TIMEOUT);

                    if (ret == SD_SUCCESS)
                        {
                        get_last_string (pRcb->name, varName);
                        for (i = 0; i < structData.numRtOut; i++)
                            {
                            char buf[1024] = { 0 };
                            if (structData.rtPtrOut[i].el_tag == RT_STR_START ||
                                structData.rtPtrOut[i].el_tag == RT_ARR_START ||
                                structData.rtPtrOut[i].el_tag == RT_STR_END ||
                                structData.rtPtrOut[i].el_tag == RT_ARR_END)
                                {
                                dataSrc += structData.rtPtrOut[i].el_size;
                                }
                            else
                                {

                                if (strcmp (structData.rtPtrOut[i].comp_name_ptr, "RptID") == 0)
                                    {
                                    valToString (dataSrc, pRcb->RptID, structData.rtPtrOut[i].el_tag, abs (structData.rtPtrOut[i].u.p.el_len));
                                    }
                                else if (strcmp (structData.rtPtrOut[i].comp_name_ptr, "DatSet") == 0)
                                    {
                                    valToString (dataSrc, pRcb->DatSet, structData.rtPtrOut[i].el_tag, abs (structData.rtPtrOut[i].u.p.el_len));
                                    }
                                else if (strcmp (structData.rtPtrOut[i].comp_name_ptr, "OptFlds") == 0)
                                    {
                                    valToString (dataSrc, pRcb->OptFlds, structData.rtPtrOut[i].el_tag, abs (structData.rtPtrOut[i].u.p.el_len));
                                    }
                                else if (strcmp (structData.rtPtrOut[i].comp_name_ptr, "TrgOps") == 0)
                                    {
                                    valToString (dataSrc, pRcb->TrgOps, structData.rtPtrOut[i].el_tag, abs (structData.rtPtrOut[i].u.p.el_len));
                                    }
                                else if (strcmp (structData.rtPtrOut[i].comp_name_ptr, "BufTm") == 0)
                                    {
                                    valToString (dataSrc, pRcb->BufTm, structData.rtPtrOut[i].el_tag, abs (structData.rtPtrOut[i].u.p.el_len));
                                    }
                                else if (strcmp (structData.rtPtrOut[i].comp_name_ptr, "ConfRev") == 0)
                                    {
                                    valToString (dataSrc, pRcb->ConfRev, structData.rtPtrOut[i].el_tag, abs (structData.rtPtrOut[i].u.p.el_len));
                                    }

                                dataSrc += structData.rtPtrOut[i].el_size;
                                }
                            }
                        }
                    }

                mvl_type_id_destroy (structData.typeId);
                }
            }
        }

    mvl_free_req_ctrl (reqCtrl);

    return ret;
    }

mGOOSE::mGOOSE ()
    {
    }

mGOOSE::~mGOOSE ()
    {
    }

int mGOOSE::sGetGoCBValues (MVL_NET_INFO* pNetInfo, char* domName, char* pVarName, STRUCT_GCB* pGcb, unsigned char* dataSrc)
    {
    GETVAR_REQ_INFO getvar_req = { 0 };
    int ret = SD_FAILURE;
    MVL_REQ_PEND* reqCtrl = NULL;
    STRUCT_GET structData = { 0 };
    int i = 0;
    char* varName = &pVarName[strlen (domName) + 1];

    getvar_req.req_tag = GETVAR_NAME;
    getvar_req.name.object_tag = DOM_SPEC;
    getvar_req.name.domain_id = domName;
    getvar_req.name.obj_name.vmd_spec = varName;

    ret = mvla_getvar (pNetInfo, &getvar_req, &reqCtrl);

    if (ret == SD_SUCCESS)
        {
        ret = waitReqDoneEx (reqCtrl, MAX_GET_LN_TIMEOUT);

        if (ret == SD_SUCCESS)
            {
            structData.typeId = mvl_type_id_create (NULL,
                                                    reqCtrl->u.getvar.resp_info->type_spec.data,
                                                    reqCtrl->u.getvar.resp_info->type_spec.len);

            if (structData.typeId != MVL_TYPE_ID_INVALID)
                {
                ret = mvl_get_runtime (structData.typeId,
                                       &structData.rtPtrOut,
                                       &structData.numRtOut);

                if (ret == SD_SUCCESS)
                    {
                    ret = named_var_read (pNetInfo, varName, DOM_SPEC, domName, structData.typeId, dataSrc, MAX_SET_DATA_TIMEOUT);

                    if (ret == SD_SUCCESS)
                        {
                        get_last_string (varName, pGcb->name);
                        for (i = 0; i < structData.numRtOut; i++)
                            {
                            char buf[1024] = { 0 };
                            if (structData.rtPtrOut[i].el_tag == RT_STR_START ||
                                structData.rtPtrOut[i].el_tag == RT_ARR_START ||
                                structData.rtPtrOut[i].el_tag == RT_STR_END ||
                                structData.rtPtrOut[i].el_tag == RT_ARR_END)
                                {
                                dataSrc += structData.rtPtrOut[i].el_size;
                                }
                            else
                                {

                                if (strcmp (structData.rtPtrOut[i].comp_name_ptr, "Addr") == 0)
                                    {
                                    valToString (dataSrc, pGcb->ADDR, structData.rtPtrOut[i].el_tag, abs (structData.rtPtrOut[i].u.p.el_len));
                                    }
                                else if (strcmp (structData.rtPtrOut[i].comp_name_ptr, "DatSet") == 0)
                                    {
                                    valToString (dataSrc, pGcb->DatSet, structData.rtPtrOut[i].el_tag, abs (structData.rtPtrOut[i].u.p.el_len));
                                    }
                                else if (strcmp (structData.rtPtrOut[i].comp_name_ptr, "GoID") == 0)
                                    {
                                    valToString (dataSrc, pGcb->GoID, structData.rtPtrOut[i].el_tag, abs (structData.rtPtrOut[i].u.p.el_len));
                                    }
                                else if (strcmp (structData.rtPtrOut[i].comp_name_ptr, "MinTime") == 0)
                                    {
                                    valToString (dataSrc, pGcb->MinTime, structData.rtPtrOut[i].el_tag, abs (structData.rtPtrOut[i].u.p.el_len));
                                    }
                                else if (strcmp (structData.rtPtrOut[i].comp_name_ptr, "MaxTime") == 0)
                                    {
                                    valToString (dataSrc, pGcb->MaxTime, structData.rtPtrOut[i].el_tag, abs (structData.rtPtrOut[i].u.p.el_len));
                                    }
                                else if (strcmp (structData.rtPtrOut[i].comp_name_ptr, "PRIORITY") == 0)
                                    {
                                    valToString (dataSrc, pGcb->PRIORITY, structData.rtPtrOut[i].el_tag, abs (structData.rtPtrOut[i].u.p.el_len));
                                    }
                                else if (strcmp (structData.rtPtrOut[i].comp_name_ptr, "VID") == 0)
                                    {
                                    valToString (dataSrc, pGcb->VID, structData.rtPtrOut[i].el_tag, abs (structData.rtPtrOut[i].u.p.el_len));
                                    }
                                else if (strcmp (structData.rtPtrOut[i].comp_name_ptr, "APPID") == 0)
                                    {
                                    valToString (dataSrc, pGcb->APPID, structData.rtPtrOut[i].el_tag, abs (structData.rtPtrOut[i].u.p.el_len));
                                    }
                                else if (strcmp (structData.rtPtrOut[i].comp_name_ptr, "ConfRev") == 0)
                                    {
                                    valToString (dataSrc, pGcb->ConfRev, structData.rtPtrOut[i].el_tag, abs (structData.rtPtrOut[i].u.p.el_len));
                                    }

                                dataSrc += structData.rtPtrOut[i].el_size;
                                }
                            }
                        }
                    }

                mvl_type_id_destroy (structData.typeId);
                }
            }
        }

    mvl_free_req_ctrl (reqCtrl);

    return ret;
    }

mDataSet::mDataSet ()
    {
    }

mDataSet::~mDataSet ()
    {
    }

int mDataSet::sGetDataSetDirectory (MVL_NET_INFO* pNetInfo, char* domName, char* dsName, STRUCT_GET_LIST* list)
    {
    int ret = SD_FAILURE;
    MVL_REQ_PEND* reqCtrl = NULL;
    GETVLIST_REQ_INFO getvlist_req;
    ST_INT timeOut = MAX_GET_DS_TIMEOUT;
    GETVLIST_RESP_INFO* getvlist_resp;
    VARIABLE_LIST* variable_list;
    int i = 0;
    int len = 0;

    getvlist_req.vl_name.domain_id = domName;
    getvlist_req.vl_name.object_tag = DOM_SPEC;
    getvlist_req.vl_name.obj_name.vmd_spec = dsName;

    ret = mvla_getvlist (pNetInfo, &getvlist_req, &reqCtrl);
    if (ret == SD_SUCCESS)
        {
        ret = waitReqDoneEx (reqCtrl, timeOut);

        if (ret == SD_SUCCESS)
            {
            getvlist_resp = reqCtrl->u.getvlist.resp_info;
            variable_list = (VARIABLE_LIST*)(getvlist_resp + 1);

            list->count = getvlist_resp->num_of_variables;
            for (i = 0; i < getvlist_resp->num_of_variables; i++, variable_list++)
                {
                STRUCT_LEAF* leaf = NULL;

                len = strlen (variable_list->var_spec.vs.name.domain_id);
                len += 1; // '/'
                len += strlen (variable_list->var_spec.vs.name.obj_name.vmd_spec);
                len += 1; // '\n'

                leaf = (STRUCT_LEAF*)chk_calloc (1, sizeof (STRUCT_LEAF));
                if (leaf == NULL)
                    {
                    pClientLog ("ALLOC", "ERROR", -1);
                    }
                leaf->pName = (char*)chk_calloc (1, len);
                if (leaf->pName == NULL)
                    {
                    pClientLog ("ALLOC", "ERROR", -1);
                    }

                sprintf (leaf->pName, "%s/%s", variable_list->var_spec.vs.name.domain_id, variable_list->var_spec.vs.name.obj_name.vmd_spec);
                list_add_last (&list->pListItem, leaf);
                }
            }
        }

    mvl_free_req_ctrl (reqCtrl);  /* CRITICAL:            */

    return ret;
    }

int mDataSet::getDataSetNames (MVL_NET_INFO* pNetInfo, char* domName, char* getafter, STRUCT_GET_LIST* list)
    {
    int ret = SD_FAILURE;
    NAMELIST_RESP_INFO* getnam_resp;
    NAMELIST_REQ_INFO getnam_req = { 0 };
    ST_INT timeOut = MAX_GET_DS_TIMEOUT;
    MVL_REQ_PEND* reqCtrl = NULL;
    ST_CHAR** nptr = NULL;
    int i = 0;

    getnam_req.cs_objclass_pres = SD_FALSE;
    getnam_req.obj.mms_class = 2;
    getnam_req.objscope = DOM_SPEC;

    if (strcmp (getafter, "") == 0)
        {
        getnam_req.cont_after_pres = SD_FALSE;
        }
    else
        {
        getnam_req.cont_after_pres = SD_TRUE;
        strncpy (getnam_req.continue_after, getafter, sizeof (getnam_req.continue_after));
        }

    strncpy (getnam_req.dname, domName, sizeof (getnam_req.dname));
    ret = mvla_getnam (pNetInfo, &getnam_req, &reqCtrl);

    if (ret == SD_SUCCESS)
        {
        ret = waitReqDoneEx (reqCtrl, timeOut);

        if (ret == SD_SUCCESS)
            {
            getnam_resp = (NAMELIST_RESP_INFO*)reqCtrl->u.ident.resp_info;
            list->count = getnam_resp->num_names;

            list->moreFollows = getnam_resp->more_follows;
            if (getnam_resp->num_names > 0)
                {
                nptr = (ST_CHAR * *)(getnam_resp + 1);
                for (i = 0; i < getnam_resp->num_names; ++i)
                    {
                    STRUCT_LEAF* leaf = NULL;

                    leaf = (STRUCT_LEAF*)chk_calloc (1, sizeof (STRUCT_LEAF));
                    if (leaf == NULL)
                        {
                        pClientLog ("ALLOC", "ERROR", -1);
                        }
                    leaf->pName = (char*)chk_calloc (1, strlen (nptr[i]) + 1);
                    if (leaf->pName == NULL)
                        {
                        pClientLog ("ALLOC", "ERROR", -1);
                        }

                    strcpy (leaf->pName, nptr[i]);
                    list_add_last (&list->pListItem, leaf);
                    }
                }
            }
        }
    mvl_free_req_ctrl (reqCtrl);

    return ret;
    }

mLocal::mLocal ()
    {
    }

mLocal::~mLocal ()
    {
    }

int mLocal::getSclData (char* pSclPath, SCL_INFO* pSclInfo)
    {
    int ret = SD_FAILURE;
    SCL_OPTIONS options = { 0 };

    options.forceEdition = 2;
    options.includeOwner = SD_FALSE;
    options.buf_size = 100000;
    options.datamap_filename = "datamap_saddr.cfg";

    try
        {
        ret = scl_parse_scd_all (pSclPath, &options, pSclInfo);
        }
    catch (exception ex)
        {
        ret = SD_FAILURE;
        }

    if (ret == SX_ERR_CONVERT)
        {
        /*Just warnings*/
        ret = SD_SUCCESS;
        }

    return ret;
    }

int mLocal::getVmdData (char* pSclPath, char* pIedName, char* pApName, MVL_VMD_CTRL** vmd_ctrl, SCL_INFO* pSclInfo)
    {
    int ret = SD_FAILURE;
    MVL_VMD_CTRL **vmd_ctrl_arr = NULL;     /* for "scl2_vmd_create_all"    */
    SCL_SERV_OPT serv_opt = {0};            /* for "scl2_vmd_create_all"    */
    ST_UINT vmd_count = 0;                      /* for "scl2_vmd_create_all"    */
    SCL_OPTIONS options = {0};              /* for "scl_parse_scd_all"      */

#if 1
    *vmd_ctrl = config_iec_remote_vmd (pSclPath, pIedName, pApName, pSclInfo);

    if (*vmd_ctrl != NULL)
        {
        ret = SD_SUCCESS;
        }
#else
    /* Set parser options and call SCL parser.  */
    options.forceEdition = 2;   /* Treat as Edition 2 file no matter what*/
    options.includeOwner = SD_FALSE;
    options.buf_size = 100000;
    options.datamap_filename = "datamap_saddr.cfg"; /* to demonstrate new feature*/
    ret = scl_parse_scd_all (pSclPath, &options, pSclInfo);
    if (ret == SD_SUCCESS || ret == SX_ERR_CONVERT)
        {
        vmd_ctrl_arr = scl2_vmd_create_all (pSclInfo, pIedName, pApName,
                                            &serv_opt, &vmd_count);
        if (vmd_ctrl_arr)
            {
            ret = SD_SUCCESS;
            all_obj_destroy_scd (vmd_ctrl_arr, vmd_count);
            }
        else
            {
            ret = SD_FAILURE;
            }

        mvl_num_types;
        scl_info_destroy (pSclInfo);
        }
#endif

    return ret;
    }

mControl::mControl ()
    {
    }

mControl::~mControl ()
    {
    }

int mControl::sSelect (MVL_NET_INFO* pNetInfo, char* domName, char* varName)
    {
    GETVAR_REQ_INFO getvar_req = { 0 };
    int ret = SD_FAILURE;
    MVL_REQ_PEND* reqCtrl = NULL;
    STRUCT_GET structData = { 0 };
    unsigned char dataSrc[1024] = { 0 };
    int i = 0;

    getvar_req.req_tag = GETVAR_NAME;
    getvar_req.name.object_tag = DOM_SPEC;
    getvar_req.name.domain_id = domName;
    getvar_req.name.obj_name.vmd_spec = varName;

    ret = mvla_getvar (pNetInfo, &getvar_req, &reqCtrl);

    if (ret == SD_SUCCESS)
        {
        ret = waitReqDoneEx (reqCtrl, MAX_GET_LN_TIMEOUT);

        if (ret == SD_SUCCESS)
            {
            structData.typeId = mvl_type_id_create (NULL,
                                                    reqCtrl->u.getvar.resp_info->type_spec.data,
                                                    reqCtrl->u.getvar.resp_info->type_spec.len);

            if (structData.typeId != MVL_TYPE_ID_INVALID)
                {
                ret = mvl_get_runtime (structData.typeId,
                                       &structData.rtPtrOut,
                                       &structData.numRtOut);

                if (ret == SD_SUCCESS)
                    {
                    ret = named_var_read (pNetInfo, varName, DOM_SPEC, domName, structData.typeId, dataSrc, MAX_SET_DATA_TIMEOUT);
                    }

                mvl_type_id_destroy (structData.typeId);
                }
            }
        }

    mvl_free_req_ctrl (reqCtrl);

    return ret;
    }

int mControl::sSelectWithValue (MVL_NET_INFO* pNetInfo,
                                char* domName,
                                char* varNameOper,
                                char* varNameSBOw,
                                char* valueInStr,
                                char* addCause,
                                char* orCat,
                                char* interlockCheck,
                                char* synchrocheck,
                                char* test,
                                int timeInSec)
    {
    GETVAR_REQ_INFO getvar_req = { 0 };
    int ret = SD_FAILURE;
    MVL_REQ_PEND* reqCtrl = NULL;
    STRUCT_GET structData = { 0 };
    int i = 0;
    ST_CHAR* ctrl_data_buf = NULL; /* "Oper" data. Get size from RUNTIME_TYPE & alloc*/
    RUNTIME_TYPE* rt_type = NULL;
    int rt_num = 0;
    ST_INT timeOut = MAX_GET_LN_TIMEOUT;
    ST_INT addcauseTimeOut = MAX_ADD_CAUSE_TIMEOUT;

    ST_CHAR* LastApplErrorTdl = "{\
    (CntrlObj)Vstring129,\
    (Error)Byte,\
    (Origin){(orCat)Byte,(orIdent)OVstring64},\
    (ctlNum)Ubyte,\
    (AddCause)Byte}";

    getvar_req.req_tag = GETVAR_NAME;
    getvar_req.name.object_tag = DOM_SPEC;
    getvar_req.name.domain_id = domName;
    getvar_req.name.obj_name.vmd_spec = varNameOper;

    ret = mvla_getvar (pNetInfo, &getvar_req, &reqCtrl);

    if (ret == SD_SUCCESS)
        {
        ret = waitReqDoneEx (reqCtrl, MAX_GET_LN_TIMEOUT);

        if (ret == SD_SUCCESS)
            {
            structData.typeId = mvl_type_id_create (NULL,
                                                    reqCtrl->u.getvar.resp_info->type_spec.data,
                                                    reqCtrl->u.getvar.resp_info->type_spec.len);

            if (structData.typeId != MVL_TYPE_ID_INVALID)
                {
                ret = mvl_get_runtime (structData.typeId,
                                       &structData.rtPtrOut,
                                       &structData.numRtOut);

                if (ret == SD_SUCCESS)
                    {
                    /* Allocate "Oper" data buffer. Loop through type to init data.*/
                    ctrl_data_buf = (char*)chk_calloc (1, structData.rtPtrOut[0].offset_to_last);
#ifdef USE_READ_BEFORE_WRITE
                    ret = named_var_read (pNetInfo, varNameOper, DOM_SPEC, domName, structData.typeId, ctrl_data_buf, MAX_SET_DATA_TIMEOUT);

                    if (ret == SD_SUCCESS)
                        {
#endif
                        ST_INT data_offset = 0;     /* use to compute offset to components  */
                        rt_num = structData.numRtOut;

                        for (int j = 0; j < rt_num; j++)
                            {
                            rt_type = &structData.rtPtrOut[j];
                            if (strcmp (ms_comp_name_find (rt_type), "ctlVal") == 0)
                                {
                                if (rt_type->el_tag == RT_BOOL)
                                    {
                                    if (valueInStr[0] == '0')
                                        {
                                        *(ST_CHAR*)(ctrl_data_buf + data_offset) = 0;
                                        }
                                    else
                                        {
                                        *(ST_CHAR*)(ctrl_data_buf + data_offset) = 1;
                                        }
                                    }
                                else if (rt_type->el_tag == RT_INTEGER)
                                    {
                                    if (rt_type->u.p.el_len == 4)
                                        {
                                        *(ST_INT32*)(ctrl_data_buf + data_offset) = (ST_INT32)atoi (valueInStr);
                                        }
                                    else if (rt_type->u.p.el_len == 2)
                                        {
                                        *(ST_INT16*)(ctrl_data_buf + data_offset) = (ST_INT16)atoi (valueInStr);
                                        }
                                    else if (rt_type->u.p.el_len == 1)
                                        {
                                        *(ST_INT8*)(ctrl_data_buf + data_offset) = (ST_INT8)atoi (valueInStr);
                                        }
                                    }
                                else if (rt_type->el_tag == RT_UNSIGNED)
                                    {
                                    if (rt_type->u.p.el_len == 4)
                                        {
                                        *(ST_UINT32*)(ctrl_data_buf + data_offset) = (ST_UINT32)atoi (valueInStr);
                                        }
                                    else if (rt_type->u.p.el_len == 2)
                                        {
                                        *(ST_UINT16*)(ctrl_data_buf + data_offset) = (ST_UINT16)atoi (valueInStr);
                                        }
                                    else if (rt_type->u.p.el_len == 1)
                                        {
                                        *(ST_UINT8*)(ctrl_data_buf + data_offset) = (ST_UINT8)atoi (valueInStr);
                                        }
                                    }
                                else if (rt_type->el_tag == RT_FLOATING_POINT)
                                    {
                                    *(ST_FLOAT*)(ctrl_data_buf + data_offset) = (ST_FLOAT)atof (valueInStr);
                                    }
                                else if (rt_type->el_tag == RT_BIT_STRING)
                                    {
                                    if (rt_type->u.p.el_len == 2)
                                        {
                                        for (int i = 0; i < rt_type->u.p.el_len; i++)
                                            {
                                            if (valueInStr[i] == '0')
                                                {
                                                BSTR_BIT_SET_ON ((unsigned char*)(ctrl_data_buf + data_offset), i);
                                                }
                                            else
                                                {
                                                BSTR_BIT_SET_OFF ((unsigned char*)(ctrl_data_buf + data_offset), i);
                                                }
                                            }
                                        }
                                    }
                                }
                            else if (strcmp (ms_comp_name_find (rt_type), "Test") == 0)
                                {
                                if (rt_type->el_tag == RT_BOOL)
                                    {
                                    if (test[0] == '0')
                                        {
                                        *(ST_CHAR*)(ctrl_data_buf + data_offset) = 0;
                                        }
                                    else
                                        {
                                        *(ST_CHAR*)(ctrl_data_buf + data_offset) = 1;
                                        }
                                    }
                                }
                            else if (strcmp (ms_comp_name_find (rt_type), "orCat") == 0)
                                {
                                if (rt_type->el_tag == RT_INTEGER)
                                    {
                                    if (rt_type->u.p.el_len == 1)
                                        {
                                        *(ST_INT8*)(ctrl_data_buf + data_offset) = (ST_INT8)atoi (orCat);
                                        }
                                    }
                                }
                            else if (strcmp (ms_comp_name_find (rt_type), "Check") == 0)
                                {
                                if (rt_type->el_tag == RT_BIT_STRING)
                                    {
                                    if (abs (rt_type->u.p.el_len) == 2)
                                        {
                                        if (strcmp (interlockCheck, "0") == 0)
                                            {
                                            BSTR_BIT_SET_OFF ((unsigned char*)(ctrl_data_buf + data_offset + 2), 1);
                                            }
                                        else
                                            {
                                            BSTR_BIT_SET_ON ((unsigned char*)(ctrl_data_buf + data_offset + 2), 1);
                                            }
                                        if (strcmp (synchrocheck, "0") == 0)
                                            {
                                            BSTR_BIT_SET_OFF ((unsigned char*)(ctrl_data_buf + data_offset + 2), 0);
                                            }
                                        else
                                            {
                                            BSTR_BIT_SET_ON ((unsigned char*)(ctrl_data_buf + data_offset + 2), 0);
                                            }
                                        }
                                    }
                                }
                            else if (strcmp (ms_comp_name_find(rt_type), "operTm") == 0)
                                {
                                if (rt_type->el_tag == RT_UTC_TIME)
                                    {
                                    MMS_UTC_TIME *utc_time;
                                    utc_time = (MMS_UTC_TIME *)(ctrl_data_buf + data_offset);
                                    utc_time->secs = (ST_UINT32) (timeInSec);
                                    utc_time->fraction = 0;
                                    utc_time->qflags = 0x00;
                                    }
                                }
                            data_offset += rt_type->el_size;
                            assert (data_offset <= structData.rtPtrOut[0].offset_to_last);

                            } /* end "for"    */

                            /* NOTE: -1 indicates "CommandTermination" not received yet.          */
                            /*  This must be set before "named_var_write" because                 */
                            /*  CommandTermination may be received before it returns.             */

                        my_control_info.cmd_term_num_va = -1;
                        strcpy (my_control_info.oper_name, varNameOper);        /* save oper_name for later*/


                        LastApplErrorTypeId = mvl_type_id_create_from_tdl (NULL, LastApplErrorTdl);

                        /* NOTE: if this is SBO control and SBO read failed, this write       */
                        /*   to "Oper" should fail, but try it anyway.                        */
                        ret = named_var_write (pNetInfo, varNameSBOw, DOM_SPEC, domName,  /* no domain name*/
                                               structData.typeId, ctrl_data_buf, timeOut);
                        if (ret != SD_SUCCESS)
                            {
                            /* If LastApplError was set by a Report, display its data. */
                            if (my_control_info.LastApplError.CntrlObj[0] != 0)
                                {
                                addCauseToString ((ST_INT)my_control_info.LastApplError.AddCause, addCause);
                                }
                            }

                        /* if buffer was allocated, free it.  */
                        if (ctrl_data_buf)
                            chk_free (ctrl_data_buf);

                        mvl_type_id_destroy (LastApplErrorTypeId);
#ifdef USE_READ_BEFORE_WRITE
                        }
#endif
                    }

                mvl_type_id_destroy (structData.typeId);
                }
            }
        }

    mvl_free_req_ctrl (reqCtrl);

    return ret;
    }

int mControl::sOperate (MVL_NET_INFO* pNetInfo,
                        char* domName,
                        char* varName,
                        char* valueInStr,
                        char* addCause,
                        char* orCat,
                        char* interlockCheck,
                        char* synchrocheck,
                        char* test,
                        int timeInSec,
                        int ctlModel)
    {
    GETVAR_REQ_INFO getvar_req = { 0 };
    int ret = SD_FAILURE;
    MVL_REQ_PEND* reqCtrl = NULL;
    STRUCT_GET structData = { 0 };
    int i = 0;
    ST_CHAR* ctrl_data_buf = NULL; /* "Oper" data. Get size from RUNTIME_TYPE & alloc*/
    RUNTIME_TYPE* rt_type = NULL;
    int rt_num = 0;
    ST_INT timeOut = MAX_GET_LN_TIMEOUT;
    ST_INT addcauseTimeOut = MAX_ADD_CAUSE_TIMEOUT;
    int error = 0;

    int addCauseReceived = 0;

    getvar_req.req_tag = GETVAR_NAME;
    getvar_req.name.object_tag = DOM_SPEC;
    getvar_req.name.domain_id = domName;
    getvar_req.name.obj_name.vmd_spec = varName;

    ST_CHAR* LastApplErrorTdl = "{\
    (CntrlObj)Vstring129,\
    (Error)Byte,\
    (Origin){(orCat)Byte,(orIdent)OVstring64},\
    (ctlNum)Ubyte,\
    (AddCause)Byte}";

    ret = mvla_getvar (pNetInfo, &getvar_req, &reqCtrl);

    if (ret == SD_SUCCESS)
        {
        ret = waitReqDoneEx (reqCtrl, MAX_GET_LN_TIMEOUT);

        if (ret == SD_SUCCESS)
            {
            structData.typeId = mvl_type_id_create (NULL,
                                                    reqCtrl->u.getvar.resp_info->type_spec.data,
                                                    reqCtrl->u.getvar.resp_info->type_spec.len);

            if (structData.typeId != MVL_TYPE_ID_INVALID)
                {
                ret = mvl_get_runtime (structData.typeId,
                                       &structData.rtPtrOut,
                                       &structData.numRtOut);

                if (ret == SD_SUCCESS)
                {
                    /* Allocate "Oper" data buffer. Loop through type to init data.*/
                    ctrl_data_buf = (char*)chk_calloc(1, structData.rtPtrOut[0].offset_to_last);
#ifdef USE_READ_BEFORE_WRITE
                    ret = named_var_read(pNetInfo, varName, DOM_SPEC, domName, structData.typeId, ctrl_data_buf, MAX_SET_DATA_TIMEOUT);

                    if (ret == SD_SUCCESS)
                    {
#endif
                        ST_INT data_offset = 0;     /* use to compute offset to components  */
                        rt_num = structData.numRtOut;

                        for (int j = 0; j < rt_num; j++)
                        {
                            rt_type = &structData.rtPtrOut[j];
                            if (strcmp(ms_comp_name_find(rt_type), "ctlVal") == 0)
                            {
                                if (rt_type->el_tag == RT_BOOL)
                                {
                                    if (valueInStr[0] == '0')
                                    {
                                        *(ST_CHAR*)(ctrl_data_buf + data_offset) = 0;
                                    }
                                    else
                                    {
                                        *(ST_CHAR*)(ctrl_data_buf + data_offset) = 1;
                                    }
                                }
                                else if (rt_type->el_tag == RT_INTEGER)
                                {
                                    if (rt_type->u.p.el_len == 4)
                                    {
                                        *(ST_INT32*)(ctrl_data_buf + data_offset) = (ST_INT32)atoi(valueInStr);
                                    }
                                    else if (rt_type->u.p.el_len == 2)
                                    {
                                        *(ST_INT16*)(ctrl_data_buf + data_offset) = (ST_INT16)atoi(valueInStr);
                                    }
                                    else if (rt_type->u.p.el_len == 1)
                                    {
                                        *(ST_INT8*)(ctrl_data_buf + data_offset) = (ST_INT8)atoi(valueInStr);
                                    }
                                }
                                else if (rt_type->el_tag == RT_UNSIGNED)
                                {
                                    if (rt_type->u.p.el_len == 4)
                                    {
                                        *(ST_UINT32*)(ctrl_data_buf + data_offset) = (ST_UINT32)atoi(valueInStr);
                                    }
                                    else if (rt_type->u.p.el_len == 2)
                                    {
                                        *(ST_UINT16*)(ctrl_data_buf + data_offset) = (ST_UINT16)atoi(valueInStr);
                                    }
                                    else if (rt_type->u.p.el_len == 1)
                                    {
                                        *(ST_UINT8*)(ctrl_data_buf + data_offset) = (ST_UINT8)atoi(valueInStr);
                                    }
                                }
                                else if (rt_type->el_tag == RT_FLOATING_POINT)
                                {
                                    *(ST_FLOAT*)(ctrl_data_buf + data_offset) = (ST_FLOAT)atof(valueInStr);
                                }
                                else if (rt_type->el_tag == RT_BIT_STRING)
                                {
                                    if (rt_type->u.p.el_len == 2)
                                    {
                                        for (int i = 0; i < rt_type->u.p.el_len; i++)
                                        {
                                            if (valueInStr[i] == '0')
                                            {
                                                BSTR_BIT_SET_ON((unsigned char*)(ctrl_data_buf + data_offset), i);
                                            }
                                            else
                                            {
                                                BSTR_BIT_SET_OFF((unsigned char*)(ctrl_data_buf + data_offset), i);
                                            }
                                        }
                                    }
                                }
                            }
                            else if (strcmp(ms_comp_name_find(rt_type), "Test") == 0)
                            {
                                if (rt_type->el_tag == RT_BOOL)
                                {
                                    if (test[0] == '0')
                                    {
                                        *(ST_CHAR*)(ctrl_data_buf + data_offset) = 0;
                                    }
                                    else
                                    {
                                        *(ST_CHAR*)(ctrl_data_buf + data_offset) = 1;
                                    }
                                }
                            }
                            else if (strcmp(ms_comp_name_find(rt_type), "orCat") == 0)
                            {
                                if (rt_type->el_tag == RT_INTEGER)
                                {
                                    if (rt_type->u.p.el_len == 1)
                                    {
                                        *(ST_INT8*)(ctrl_data_buf + data_offset) = (ST_INT8)atoi(orCat);
                                    }
                                }
                            }
                            else if (strcmp(ms_comp_name_find(rt_type), "Check") == 0)
                            {
                                if (rt_type->el_tag == RT_BIT_STRING)
                                {
                                    if (abs(rt_type->u.p.el_len) == 2)
                                    {
                                        if (strcmp(interlockCheck, "0") == 0)
                                        {
                                            BSTR_BIT_SET_OFF((unsigned char*)(ctrl_data_buf + data_offset + 2), 1);
                                        }
                                        else
                                        {
                                            BSTR_BIT_SET_ON((unsigned char*)(ctrl_data_buf + data_offset + 2), 1);
                                        }
                                        if (strcmp(synchrocheck, "0") == 0)
                                        {
                                            BSTR_BIT_SET_OFF((unsigned char*)(ctrl_data_buf + data_offset + 2), 0);
                                        }
                                        else
                                        {
                                            BSTR_BIT_SET_ON((unsigned char*)(ctrl_data_buf + data_offset + 2), 0);
                                        }
                                    }
                                }
                            }
                            else if (strcmp(ms_comp_name_find(rt_type), "operTm") == 0)
                            {
                                if (rt_type->el_tag == RT_UTC_TIME)
                                {
                                    MMS_UTC_TIME* utc_time;
                                    utc_time = (MMS_UTC_TIME*)(ctrl_data_buf + data_offset);
                                    utc_time->secs = (ST_UINT32)(timeInSec);
                                    utc_time->fraction = 0;
                                    utc_time->qflags = 0x00;
                                }
                            }
                            data_offset += rt_type->el_size;
                            assert(data_offset <= structData.rtPtrOut[0].offset_to_last);

                        } /* end "for"    */

                        /* NOTE: -1 indicates "CommandTermination" not received yet.          */
                        /*  This must be set before "named_var_write" because                 */
                        /*  CommandTermination may be received before it returns.             */

                        my_control_info.cmd_term_num_va = -1;
                        strcpy(my_control_info.oper_name, varName);        /* save oper_name for later*/

                        LastApplErrorTypeId = mvl_type_id_create_from_tdl(NULL, LastApplErrorTdl);

                        /* NOTE: if this is SBO control and SBO read failed, this write       */
                        /*   to "Oper" should fail, but try it anyway.                        */
                        ret = named_var_write_ex(pNetInfo, varName, DOM_SPEC, domName,  /* no domain name*/
                            structData.typeId, ctrl_data_buf, timeOut, &error);
                        if (ret != SD_SUCCESS)
                        {
                            writeRespToString(error, addCause);

                            //printf ("Control Activation Response received: FAILURE\n");
                            /* If LastApplError was set by a Report, display its data. */
                            if (my_control_info.LastApplError.CntrlObj[0] != 0)
                            {
                                addCauseReceived = 1;
                                addCauseToString((ST_INT)my_control_info.LastApplError.AddCause, addCause);
                            }
                        }

                        /* if buffer was allocated, free it.  */
                        if (ctrl_data_buf)
                            chk_free(ctrl_data_buf);

                    if(ctlModel >= 3)
                    {
                        /* Wait here for CommandTermination info rpt before returning to main loop.*/
                        if (addCauseReceived == 0)
                        {
                            time_t timeout = time(NULL) + addcauseTimeOut;    /* DEBUG: chg timeout as needed.*/

                            while (doIt)
                            {
                                doCommService();
                                if (time(NULL) > timeout)
                                {
                                    sprintf(addCause, "Timeout waiting for CommandTermination, waited for %d seconds", addcauseTimeOut);
                                    //printf ("Timeout waiting for CommandTermination\n");
                                    break;
                                }

                                /*if (my_control_info.LastApplError.CntrlObj[0] != 0)
                                    {
                                    addCauseReceived = 1;
                                    addCauseToString ((ST_INT)my_control_info.LastApplError.AddCause, addCause);
                                    break;
                                    }*/

                                if (my_control_info.cmd_term_num_va >= 0)
                                {
                                    if (my_control_info.cmd_term_num_va == 2 || my_control_info.cmd_term_num_va == 1)
                                    {
                                        addCauseToString((ST_INT)my_control_info.LastApplError.AddCause, addCause);
                                    }
                                    break;
                                }

                                /* .._num_va is set by u_mvl_info_rpt_ind when report received.   */
                                //if (my_control_info.cmd_term_num_va >= 0)
                                //    {
                                //    /* If 2 vars in rpt, must be error. If 1 var, must be OK.       */
                                //    //printf ("Command Termination received: %s\n",
                                //           // my_control_info.cmd_term_num_va == 1 ? "SUCCESS" : "FAILURE");
                                //    if (my_control_info.cmd_term_num_va == 1 || my_control_info.cmd_term_num_va == 2)
                                //        {
                                //        addCauseToString ((ST_INT)my_control_info.LastApplError.AddCause, addCause);
                                //        }
                                //    break;
                                //    }
                            }
                        }

                    }

                        mvl_type_id_destroy (LastApplErrorTypeId);
#ifdef USE_READ_BEFORE_WRITE
                        }
#endif
                    }

                mvl_type_id_destroy (structData.typeId);
                }
            }
        }

    mvl_free_req_ctrl (reqCtrl);

    return ret;
    }

/*functions*/
void valToString (void* dataSrc, char* pData, int type, int len)
    {
    unsigned char* bptr = NULL;
    char buff[32] = { 0 };
    int i = 0;

    if (dataSrc && dataSrc)
        {
        strcpy (pData, "");
        switch (type)
            {
                case RT_BOOL:
                    sprintf (pData, "%d", *(unsigned char*)dataSrc);

                    if (pData[0] == '0' || pData[0] == '1')
                        {
                        }
                    else
                        {
                        pClientLog ("CODE 3", "ERROR", -1);
                        }
                    if (pData[1] != '\0')
                        {
                        pClientLog ("CODE 4", "ERROR", -1);
                        }
                    break;

                case RT_BIT_STRING:
                    strcpy (pData, "");
                    bptr = (unsigned char*)dataSrc;
                    //bptr = (unsigned char*)dataSrc;                
                    if (len <= 8)
                        {
                        for (i = 0; i < len; i++)
                            {
                            if (BSTR_BIT_GET (bptr, i) == 0)
                                {
                                strcat (pData, "0");
                                }
                            else if (BSTR_BIT_GET (bptr, i) == 1)
                                {
                                strcat (pData, "1");
                                }
                            else
                                {
                                pClientLog ("CODE 5", "ERROR", -1);
                                }
                            }
                        }
                    else
                        {
                        for (i = 0; i < len; i++)
                            {
                            if (BSTR_BIT_GET (&bptr[2], i) == 0)
                                {
                                strcat (pData, "0");
                                }
                            else if (BSTR_BIT_GET (&bptr[2], i) == 1)
                                {
                                strcat (pData, "1");
                                }
                            else
                                {
                                pClientLog ("CODE 5", "ERROR", -1);
                                }
                            }
                        }
                    break;

                case RT_INTEGER:
                    switch (len)     /* determine length             */
                        {
                            case 1:                      /* one byte int                 */
                                sprintf (pData, "%d", *(char*)dataSrc);
                                break;

                            case 2:                        /* two byte int                 */
                                sprintf (pData, "%d", *(signed short*)dataSrc);
                                break;

                            case 4:                        /* four byte integer            */
                                sprintf (pData, "%d", *(int*)dataSrc);
                                break;

#ifdef INT64_SUPPORT
                            case 8:                        /* eight byte integer           */
                                sprintf (pData, "%d", (int)(*(__int64*)dataSrc));
                                break;
#endif
                        }
                    break;

                case RT_UNSIGNED:
                    switch (len)     /* determine length             */
                        {
                            case 1:                      /* one byte int                 */
                                sprintf (pData, "%d", *(unsigned char*)dataSrc);
                                break;

                            case 2:                        /* two byte int                 */
                                sprintf (pData, "%d", *(unsigned short*)dataSrc);
                                break;

                            case 4:                        /* four byte integer            */
                                sprintf (pData, "%d", *(unsigned int*)dataSrc);
                                break;

#ifdef INT64_SUPPORT
                            case 8:                        /* eight byte integer           */
                                sprintf (pData, "%d", (unsigned int)(*(unsigned __int64*)dataSrc));
                                break;
#endif  /* INT64_SUPPORT        */
                        }
                    break;

#ifdef FLOAT_DATA_SUPPORT
                case RT_FLOATING_POINT:
                    if (len != sizeof (ST_FLOAT))
                        {
                        sprintf (pData, "%f", *(double*)dataSrc);
                        }
                    else
                        {
                        sprintf (pData, "%f", *(float*)dataSrc);
                        }
                    break;
#endif

                case RT_OCTET_STRING:
                    bptr = &((unsigned char*)dataSrc)[2];
                    strcpy (pData, "");

                    for (i = 0; i < len; i++)
                        {
                        sprintf (buff, "%X", bptr[i]);
                        strcat (pData, buff);
                        }
                    break;

                case RT_VISIBLE_STRING:
                    strcpy (pData, (char*)dataSrc);
                    break;

                case RT_UTC_TIME:
                    {
                    MMS_UTC_TIME* utcPtr;
                    utcPtr = (MMS_UTC_TIME*)dataSrc;
                    utcToString (utcPtr, pData);
                    //sprintf (pData, "%sQ%.2x", pData, utcPtr->qflags); /*TODO: Arun*/
                    }
                    break;

                case RT_BCD:
                case RT_UTF8_STRING:
#ifdef BTOD_DATA_SUPPORT
                case RT_BINARY_TIME:
#endif
#ifdef TIME_DATA_SUPPORT
                case RT_GENERAL_TIME:
#endif
                case RT_ARR_START:
                case RT_STR_START:
                case RT_ARR_END:
                case RT_STR_END:
                default:
                    pClientLog ("CODE 6", "ERROR", -1);
                    break;
            }
        }
    else
        {
        pClientLog ("CODE 7", "ERROR", -1);
        }
    }

void* stringToVal (int type, int len, char* pData, VAL_STRUCT* val)
    {
    if (pData && val)
        {
        switch (type)
            {
                case RT_BOOL:
                    if (pData[0] == '0')
                        {
                        val->i8 = 0;
                        }
                    else
                        {
                        val->i8 = 1;
                        }
                    return &val->i8;
                    break;

                case RT_BIT_STRING:
                    val->u64 = 0;
                    for (int i = 0; i < len; i++)
                        {
                        if (pData[i] == '0')
                            {
                            BSTR_BIT_SET_ON (&val->u64, len);
                            }
                        else if (pData[i] == '1')
                            {
                            BSTR_BIT_SET_OFF (&val->u64, len);
                            }
                        else
                            {
                            pClientLog ("CODE 9", "ERROR", -1);
                            }
                        }
                    return &val->u64;
                    break;

                case RT_INTEGER:
                    switch (len)     /* determine length             */
                        {
                            case 1:                      /* one byte int                 */
                                val->i8 = (signed char)atoi (pData);
                                return &val->i8;
                                break;

                            case 2:                        /* two byte int                 */
                                val->i16 = (short)atoi (pData);
                                return &val->i16;
                                break;

                            case 4:                        /* four byte integer            */
                                val->i32 = atoi (pData);
                                return &val->i32;
                                break;

#ifdef INT64_SUPPORT
                            case 8:                        /* eight byte integer           */
                                val->i64 = atoi (pData);
                                return &val->i64;
                                break;
#endif
                        }
                    break;

                case RT_UNSIGNED:
                    switch (len)     /* determine length             */
                        {
                            case 1:                      /* one byte int                 */
                                val->u8 = (unsigned char)atoi (pData);
                                return &val->u8;
                                break;

                            case 2:                        /* two byte int                 */
                                val->u16 = (unsigned short)atoi (pData);
                                return &val->u16;
                                break;

                            case 4:                        /* four byte integer            */
                                val->u32 = atoi (pData);
                                return &val->u32;
                                break;

#ifdef INT64_SUPPORT
                            case 8:                        /* eight byte integer           */
                                val->u64 = atoi (pData);
                                return &val->u64;
                                break;
#endif  /* INT64_SUPPORT        */
                        }
                    break;

#ifdef FLOAT_DATA_SUPPORT
                case RT_FLOATING_POINT:
                    if (len != sizeof (ST_FLOAT))
                        {
                        val->f = (float)atof (pData);
                        return &val->f;
                        }
                    else
                        {
                        val->d = atof (pData);
                        return &val->d;
                        }
                    break;
#endif

                case RT_OCTET_STRING:
                    string2hexString (pData, &val->octStr[2]);
                    return val->octStr;
                    break;

                case RT_VISIBLE_STRING:
                    return pData;
                    break;

                case RT_UTC_TIME:
                    {
                    stringToUtc (&val->utc, pData);
                    return &val->utc;
                    break;
                    }

                case RT_BCD:
                case RT_UTF8_STRING:
#ifdef BTOD_DATA_SUPPORT
                case RT_BINARY_TIME:
#endif
#ifdef TIME_DATA_SUPPORT
                case RT_GENERAL_TIME:
#endif
                case RT_ARR_START:
                case RT_STR_START:
                case RT_ARR_END:
                case RT_STR_END:
                default:
                    pClientLog ("CODE 11", "ERROR", -1);
                    break;
            }
        }
    else
        {
        pClientLog ("CODE 12", "ERROR", -1);
        }

    pClientLog ("CODE 13", "ERROR", -1);
    return NULL;
    }
