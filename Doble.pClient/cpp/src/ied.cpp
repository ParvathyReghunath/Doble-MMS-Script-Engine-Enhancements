/*******************************************************************************
File      : ied.cpp
By        : Arun Lal K M
Date      : 18th Jun 2019
*******************************************************************************/

/*include*/
#include "ied.hpp"
#include "dirent.h"

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR* SD_CONST thisFileName = __FILE__;
#endif

/*variables*/
std::map <int, IED*> gIedList;

IED::IED (int uId, char* pIp)
    {
    acsiServer = new mServer ();
    _ASSERT_ (acsiServer);

    acsiAssociation = new mAssociation ();
    _ASSERT_ (acsiAssociation);

    acsiLogicalDevice = new mLogicalDevice ();
    _ASSERT_ (acsiLogicalDevice);

    acsiLogicalNode = new mLogicalNode ();
    _ASSERT_ (acsiLogicalNode);

    acsiData = new mData ();
    _ASSERT_ (acsiData);

    acsiDataSet = new mDataSet ();
    _ASSERT_ (acsiDataSet);

    acsiBRCB = new mBRCB ();
    _ASSERT_ (acsiBRCB);

    acsiURCB = new mURCB ();
    _ASSERT_ (acsiURCB);

    acsiControl = new mControl ();
    _ASSERT_ (acsiControl);

    acsiGoCB = new mGOOSE ();
    _ASSERT_ (acsiGoCB);

    local = new mLocal ();
    _ASSERT_ (local);

    xml = new XML ();
    _ASSERT_ (xml);

    leafList = (POLL_LIST_STRUCT*)chk_calloc (1, sizeof (POLL_LIST_STRUCT));
    if (leafList == NULL)
        {
        pClientLog ("ALLOC", "ERROR", -1);
        }

    iId = uId;
    sprintf (sId, "%d", iId);
    strncpy (ip, pIp, sizeof (ip));
    associationStatus = STATUS_DISCONNECTED;
    netInfo = NULL;
    pXmlFp = NULL;
    strncpy (iedName, "", sizeof (iedName));
    }

IED::~IED ()
    {
    if (associationStatus == STATUS_CONNECTED)
        {
        acsiAssociation->sRelease (netInfo);
        }

    if (leafList)
        {
        chk_free (leafList);
        }

    delete acsiServer;
    delete acsiAssociation;
    delete acsiLogicalDevice;
    delete acsiLogicalNode;
    delete acsiData;
    delete acsiDataSet;
    delete acsiGoCB;
    delete acsiURCB;
    delete acsiBRCB;
    delete acsiControl;
    delete local;
    delete xml;
    }

int IED::printStruct (RUNTIME_TYPE* rtPtrOut, ST_INT numRtOut, int* cnt, char* pRef, int layer, STRUCT_GET_LIST* getCbList)
    {
    int ret = SD_SUCCESS;
    char ref[1024] = { 0 };
    int count = 0;
    int exitAt = 1;

    MIN_SLEEP;
    strncpy (ref, pRef, sizeof (ref));

    for (count = *cnt; count < numRtOut; count++)
        {
        if (exitAt == 0)
            {
            break;
            }

        if (NULL == rtPtrOut[count].comp_name_ptr
            && RT_STR_END != rtPtrOut[count].el_tag
            && RT_ARR_END != rtPtrOut[count].el_tag)
            {
            continue;
            }

        sprintf (ref, "%s$%s", pRef, rtPtrOut[count].comp_name_ptr);

        if (RT_STR_START == rtPtrOut[count].el_tag || RT_ARR_START == rtPtrOut[count].el_tag)
            {
            if (layer == LAYER_DO)
                {
                xml->xmlPrint (pXmlFp, rtPtrOut[count].comp_name_ptr, rtPtrOut[count].el_tag, ref, NODE_DO, 3 + layer);
                count++;

                if (strstr (ref, "$GO$") ||
                    strstr (ref, "$RG$") ||
                    strstr (ref, "$BR$") ||
                    strstr (ref, "$RP$") ||
                    strstr (ref, "$SG$") ||
                    strstr (ref, "$LG$") ||
                    strstr (ref, "$MS$") ||
                    strstr (ref, "$US$"))
                    {
                    STRUCT_LEAF* leaf = NULL;

                    leaf = (STRUCT_LEAF*)chk_calloc (1, sizeof (STRUCT_LEAF));
                    if (leaf == NULL)
                        {
                        pClientLog ("ALLOC", "ERROR", -1);
                        }
                    leaf->pName = (char*)chk_calloc (1, strlen (ref) + 1);
                    if (leaf->pName == NULL)
                        {
                        pClientLog ("ALLOC", "ERROR", -1);
                        }

                    strcpy (leaf->pName, ref);
                    list_add_last (&getCbList->pListItem, leaf);
                    }

                printStruct (rtPtrOut, numRtOut, &count, ref, layer + 1, getCbList);
                xml->xmlPrint (pXmlFp, rtPtrOut[count].comp_name_ptr, RT_STR_END, ref, NODE_DO, 3 + layer);
                }
            else if (layer == LAYER_DA)
                {
                xml->xmlPrint (pXmlFp, rtPtrOut[count].comp_name_ptr, rtPtrOut[count].el_tag, ref, NODE_DA, 3 + layer);
                count++;
                printStruct (rtPtrOut, numRtOut, &count, ref, layer + 1, getCbList);
                xml->xmlPrint (pXmlFp, rtPtrOut[count].comp_name_ptr, RT_STR_END, ref, NODE_DA, 3 + layer);
                }
            else if (layer > LAYER_DA)
                {
                xml->xmlPrint (pXmlFp, rtPtrOut[count].comp_name_ptr, rtPtrOut[count].el_tag, ref, NODE_BDA, 3 + layer);
                count++;
                printStruct (rtPtrOut, numRtOut, &count, ref, layer + 1, getCbList);
                xml->xmlPrint (pXmlFp, rtPtrOut[count].comp_name_ptr, RT_STR_END, ref, NODE_BDA, 3 + layer);
                }
            else
                {
                pClientLog ("CODE 16", "ERROR", -1);
                }
            }
        else if (RT_STR_END == rtPtrOut[count].el_tag || RT_ARR_END == rtPtrOut[count].el_tag)
            {
            exitAt--;
            if (exitAt == 0)
                {
                break;
                }
            continue;
            }
        else if (rtPtrOut[count].el_tag == RT_BOOL ||
                 rtPtrOut[count].el_tag == RT_BIT_STRING ||
                 rtPtrOut[count].el_tag == RT_INTEGER ||
                 rtPtrOut[count].el_tag == RT_UNSIGNED ||
                 rtPtrOut[count].el_tag == RT_FLOATING_POINT ||
                 rtPtrOut[count].el_tag == RT_OCTET_STRING ||
                 rtPtrOut[count].el_tag == RT_VISIBLE_STRING ||
                 rtPtrOut[count].el_tag == RT_GENERAL_TIME ||
                 rtPtrOut[count].el_tag == RT_BINARY_TIME ||
                 rtPtrOut[count].el_tag == RT_BCD ||
                 rtPtrOut[count].el_tag == RT_BOOLEANARRAY ||
                 rtPtrOut[count].el_tag == RT_UTC_TIME ||
                 rtPtrOut[count].el_tag == RT_UTF8_STRING)
            {
            if (layer == LAYER_DA)
                {
                xml->xmlPrintLeaf (pXmlFp, rtPtrOut[count].comp_name_ptr, rtPtrOut[count].el_tag, rtPtrOut[count].u.p.el_len, ref, NODE_DA, 3 + layer);
                }
            else if (layer > LAYER_DA)
                {
                xml->xmlPrintLeaf (pXmlFp, rtPtrOut[count].comp_name_ptr, rtPtrOut[count].el_tag, rtPtrOut[count].u.p.el_len, ref, NODE_BDA, 3 + layer);
                }
            else
                {
                pClientLog ("CODE 17", "ERROR", -1);
                }
            }
        else
            {
            pClientLog ("CODE 18", "ERROR", -1);
            }
        }

    *cnt = count;

    return ret;
    }

int IED::printlnStruct (RUNTIME_TYPE* rtPtrOut, ST_INT  numRtOut, char* pRef, STRUCT_GET_LIST* getCbList)
    {
    int ret = SD_SUCCESS;
    int count = 0;
    char ref[1024] = { 0 };

    strncpy (ref, pRef, sizeof (ref));

    for (count = 0; count < numRtOut; count++)
        {
        MIN_SLEEP;
        if (NULL == rtPtrOut[count].comp_name_ptr
            && RT_STR_END != rtPtrOut[count].el_tag
            && RT_ARR_END != rtPtrOut[count].el_tag)
            {
            continue;
            }

        if (RT_STR_START == rtPtrOut[count].el_tag || RT_ARR_START == rtPtrOut[count].el_tag)
            {
            sprintf (ref, "%s$%s", pRef, rtPtrOut[count].comp_name_ptr);

            xml->xmlPrint (pXmlFp, rtPtrOut[count].comp_name_ptr, rtPtrOut[count].el_tag, ref, NODE_FC, 3);

            count++;
            printStruct (rtPtrOut, numRtOut, &count, ref, 1, getCbList);

            xml->xmlPrint (pXmlFp, rtPtrOut[count].comp_name_ptr, RT_STR_END, ref, NODE_FC, 3);
            }
        else if (RT_STR_END == rtPtrOut[count].el_tag || RT_ARR_END == rtPtrOut[count].el_tag)
            {
            continue;
            }
        else
            {
            pClientLog ("CODE 19", "ERROR", -1);
            }

        if (RT_STR_END != rtPtrOut[count].el_tag)
            {
            pClientLog ("CODE 20", "ERROR", -1);
            }
        }

    return ret;
    }

int IED::getDataStruct (STRUCT_GET* pStruct, char* domName, char* lnName)
    {
    return acsiLogicalNode->sGetLogicalNodeDirectory (netInfo, domName, lnName, pStruct);
    }

int IED::deleteDataStruct (STRUCT_GET* pLn)
    {
    return acsiLogicalNode->deleteTypeId (pLn);
    }

int IED::getDsStruct (STRUCT_GET_LIST* getList, char* domName, char* dsName)
    {
    return acsiDataSet->sGetDataSetDirectory (netInfo, domName, dsName, getList);
    }

int IED::getDS (STRUCT_GET_LIST* getList, char* domName)
    {
    int ret = SD_SUCCESS;

    do
        {
        ret = acsiDataSet->getDataSetNames (netInfo, domName, getList->continueAfter, getList);
        if (ret != SD_SUCCESS)
            {
            break;
            }

        if (SD_FALSE == doIt)
            {
            break;
            }

        } while (getList->moreFollows == 1);

        return ret;
    }

int IED::getLN (STRUCT_GET_LIST* getList, char* domName)
    {
    int ret = SD_SUCCESS;

    do
        {
        ret = acsiLogicalDevice->sGetLogicalDeviceDirectory (netInfo, domName, getList->continueAfter, getList);
        if (ret != SD_SUCCESS)
            {
            break;
            }

        if (SD_FALSE == doIt)
            {
            break;
            }

        } while (getList->moreFollows == 1);

        return ret;
    }

int IED::getLD (STRUCT_GET_LIST* getList)
    {
    int ret = SD_SUCCESS;

    do
        {
        ret = acsiServer->sGetServerDirectory (netInfo, getList->continueAfter, getList);
        if (ret != SD_SUCCESS)
            {
            break;
            }

        if (SD_FALSE == doIt)
            {
            break;
            }

        } while (getList->moreFollows == 1);

        return ret;
    }

void IED::getIedName (STRUCT_GET_LIST* list, char* iedName)
    {
    ST_INT i = 0;
    ST_INT j = 0;
    ST_INT k = 0;
    ST_INT lenOfDomainName = 0;
    ST_INT tmpCnt = 0;
    ST_INT cntEx = 0;
    ST_INT cnt = 0;
    ST_INT flg = 0;
    ST_INT lenStr[MAX_AR_LEN + 1] = { 0 };
    ST_CHAR tempString[MAX_AR_LEN + 1] = { 0 };
    char** listItemName = NULL;
    STRUCT_LEAF* leaf = NULL;

    listItemName = (char**)chk_calloc (list->count, sizeof (char*));
    if (listItemName == NULL)
        {
        pClientLog ("ALLOC", "ERROR", -1);
        }

    i = 0;
    for (leaf = (STRUCT_LEAF*)list_find_last ((DBL_LNK*)list->pListItem);
         leaf != NULL;
         leaf = (STRUCT_LEAF*)list_find_prev ((DBL_LNK*)list->pListItem, (DBL_LNK*)leaf))
        {
        listItemName[i] = (char*)chk_calloc (1, strlen (leaf->pName) + 1);
        if (listItemName[i] == NULL)
            {
            pClientLog ("ALLOC", "ERROR", -1);
            }
        strcpy (listItemName[i], leaf->pName);

        i++;
        if (i > list->count)
            {
            pClientLog ("CODE 14", "ERROR", -1);
            }
        }

    if (list->count == 1)
        {
        lenOfDomainName = strlen (listItemName[0]);
        tmpCnt = lenOfDomainName / 2;
        }
    else if (list->count > 1)
        {
        for (i = 0; i < list->count; i++)
            {
            lenStr[i] = strlen (listItemName[i]);
            }

        tmpCnt = lenStr[0];

        for (i = 0; i < list->count - 1; i++)
            {
            cntEx = 0;
            flg = 0;

            for (k = 0; k < lenStr[i]; k++)
                {
                cnt = 0;
                for (j = i + 1; j < list->count; j++)
                    {
                    if (listItemName[i][k] == listItemName[j][k])
                        cnt++;
                    else
                        flg = 1;
                    }
                if (cnt == (list->count - i - 1) && flg == 0)
                    cntEx++;
                }
            if (tmpCnt > cntEx && cntEx != 0)
                tmpCnt = cntEx;
            }
        }

    strcpy (tempString, listItemName[0]);
    tempString[tmpCnt] = '\0';

    strcpy (iedName, tempString);
    }

int IED::getRemoteDataModellLite ()
    {
    int ret = SD_SUCCESS;
    STRUCT_GET_LIST getLdList = { 0 };

    ret = iedConnect ();

    if (SD_SUCCESS == ret)
        {
        ret = getLD (&getLdList);
        if (SD_SUCCESS == ret)
            {
            getIedName (&getLdList, iedName);

            listClear (getLdList.pListItem);
            memset (&getLdList, 0, sizeof (getLdList));
            }
        iedDisconnect ();
        }

    return ret;
    }

int IED::getLocalDataset (char* domName, MVL_DOM_CTRL* dom)
    {
    int ret = SD_FAILURE;
    char ref[1024] = { 0 };
    char leaf[1024] = { 0 };
    char* pOp = NULL;
    char dsName[MAX_AR_LEN + 1] = { 0 };

    if (dom->num_nvlist > 0)
        {
        xml->xmlPrintNode (pXmlFp, NODE_DATASET, RT_STR_START, 1);

        for (int ds = 0; ds < dom->num_nvlist; ds++)
            {
            pOp = strstr (dom->nvlist_tbl[ds]->name, "$");
            if (pOp)
                {
                strncpy (dsName, &pOp[1], sizeof (dsName));
                }
            else
                {
                strncpy (dsName, dom->nvlist_tbl[ds]->name, sizeof (dsName));
                }

            sprintf (ref, "%s/LLN0$%s", domName, dsName);

            xml->xmlPrint (pXmlFp, dsName, RT_STR_START, ref, NODE_DS, 2);

            for (int de = 0; de < dom->nvlist_tbl[ds]->num_of_entries; de++)
                {
                sprintf (leaf, "%s/%s", domName, dom->nvlist_tbl[ds]->entries[de]->name);
                xml->xmlPrintElement (pXmlFp, leaf, NODE_LEAF, 3);
                }
            xml->xmlPrint (pXmlFp, dsName, RT_STR_END, ref, NODE_DS, 2);
            }

        xml->xmlPrintNode (pXmlFp, NODE_DATASET, RT_STR_END, 1);
        }

    return ret;
    }

int IED::getGseComm (SCL_INFO* pSclInfo, char* pIedName, char* pApName, char* pCbName, SCL_GSE** pSclGse)
    {
    SCL_CAP* scl_cap = NULL;
    SCL_SUBNET* scl_subnet = NULL;
    SCL_GSE* scl_gse = NULL;

    for (scl_subnet = (SCL_SUBNET*)list_find_last ((DBL_LNK*)pSclInfo->subnetHead);
         scl_subnet != NULL;
         scl_subnet = (SCL_SUBNET*)list_find_prev ((DBL_LNK*)pSclInfo->subnetHead, (DBL_LNK*)scl_subnet))
        {
        for (scl_cap = (SCL_CAP*)list_find_last ((DBL_LNK*)scl_subnet->capHead);
             scl_cap != NULL;
             scl_cap = (SCL_CAP*)list_find_prev ((DBL_LNK*)scl_subnet->capHead, (DBL_LNK*)scl_cap))
            {
            if (strcmp (scl_cap->iedName, pIedName) == 0
                && strcmp (scl_cap->apName, pApName) == 0)
                {
                for (scl_gse = (SCL_GSE*)list_find_last ((DBL_LNK*)scl_cap->gseHead);
                     scl_gse != NULL;
                     scl_gse = (SCL_GSE*)list_find_prev ((DBL_LNK*)scl_cap->gseHead, (DBL_LNK*)scl_gse))
                    {
                    if (strcmp (scl_gse->cbName, pCbName) == 0)
                        {
                        *pSclGse = scl_gse;
                        return SD_SUCCESS;
                        }
                    }
                }
            }
        }

    *pSclGse = NULL;
    return SD_FAILURE;
    }

int IED::getAllLocalGoCbValues (char* pIedName, char* pApName, char* domName, STRUCT_GET_LIST* getCbList, SCL_INFO* pSclInfo)
    {
    int ret = SD_FAILURE;
    SCL_SERVER* scl_server = NULL;
    SCL_LD* scl_ld = NULL;
    SCL_LN* scl_ln = NULL;
    SCL_GCB* scl_gcb = NULL;
    SCL_SUBNET* scl_subnet = NULL;
    SCL_CAP* scl_cap = NULL;
    char data[1024] = { 0 };
    char buff[1024] = { 0 };
    char ref[1024] = { 0 };

    for (scl_server = (SCL_SERVER*)list_find_last ((DBL_LNK*)pSclInfo->serverHead);
         scl_server != NULL;
         scl_server = (SCL_SERVER*)list_find_prev ((DBL_LNK*)pSclInfo->serverHead, (DBL_LNK*)scl_server))
        {
        if (strcmp (scl_server->iedName, pIedName) == 0
            && strcmp (scl_server->apName, pApName) == 0)
            {
            for (scl_ld = (SCL_LD*)list_find_last ((DBL_LNK*)scl_server->ldHead);
                 scl_ld != NULL;
                 scl_ld = (SCL_LD*)list_find_prev ((DBL_LNK*)scl_server->ldHead, (DBL_LNK*)scl_ld))
                {
                if (strcmp (scl_ld->domName, domName) == 0)
                    {
                    for (scl_ln = (SCL_LN*)list_find_last ((DBL_LNK*)scl_ld->lnHead);
                         scl_ln != NULL;
                         scl_ln = (SCL_LN*)list_find_prev ((DBL_LNK*)scl_ld->lnHead, (DBL_LNK*)scl_ln))
                        {
                        for (scl_gcb = (SCL_GCB*)list_find_last ((DBL_LNK*)scl_ln->gcbHead);
                             scl_gcb != NULL;
                             scl_gcb = (SCL_GCB*)list_find_prev ((DBL_LNK*)scl_ln->gcbHead, (DBL_LNK*)scl_gcb))
                            {
                            if (scl_gcb->isRgoose == 0)
                                {
                                SCL_GSE* scl_gse = 0;

                                ret = getGseComm (pSclInfo, pIedName, pApName, scl_gcb->name, &scl_gse);
                                if (SD_SUCCESS == ret)
                                    {

                                    sprintf (buff, "%s/%s", domName, scl_ln->varName);
                                    xml->xmlPrint (pXmlFp, buff, RT_STR_START, scl_gcb->name, NODE_GO, 2);

                                    sprintf (ref, "%s$%s", buff, "DatSet");
                                    xml->xmlPrintElementEx (pXmlFp, ref, NODE_DATASET, 3, scl_gcb->datSet);

                                    sprintf (ref, "%s$%s", buff, "GoID");
                                    xml->xmlPrintElementEx (pXmlFp, ref, NODE_GOID, 3, scl_gcb->appID);

                                    sprintf (ref, "%s$%s", buff, "ADDR");
                                    sprintf (data, "%0.2X%0.2X%0.2X%0.2X%0.2X%0.2X",
                                             scl_gse->MAC[0], scl_gse->MAC[1],
                                             scl_gse->MAC[2], scl_gse->MAC[3],
                                             scl_gse->MAC[4], scl_gse->MAC[5]);
                                    xml->xmlPrintElementEx (pXmlFp, ref, NODE_ADDR, 3, data);

                                    sprintf (ref, "%s$%s", buff, "MinTime");
                                    sprintf (data, "%d", scl_gse->MinTime);
                                    xml->xmlPrintElementEx (pXmlFp, ref, NODE_MINTIME, 3, data);

                                    sprintf (ref, "%s$%s", buff, "MaxTime");
                                    sprintf (data, "%d", scl_gse->MaxTime);
                                    xml->xmlPrintElementEx (pXmlFp, ref, NODE_MINTIME, 3, data);

                                    sprintf (ref, "%s$%s", buff, "PRIORITY");
                                    sprintf (data, "%d", scl_gse->VLANPRI);
                                    xml->xmlPrintElementEx (pXmlFp, ref, NODE_PRIORITY, 3, data);

                                    sprintf (ref, "%s$%s", buff, "VID");
                                    sprintf (data, "%d", scl_gse->VLANID);
                                    xml->xmlPrintElementEx (pXmlFp, ref, NODE_VID, 3, data);

                                    sprintf (ref, "%s$%s", buff, "APPID");
                                    sprintf (data, "%d", scl_gse->APPID);
                                    xml->xmlPrintElementEx (pXmlFp, ref, NODE_VID, 3, data);

                                    sprintf (ref, "%s$%s", buff, "ConfRev");
                                    sprintf (data, "%d", scl_gcb->confRev);
                                    xml->xmlPrintElementEx (pXmlFp, ref, NODE_VID, 3, data);

                                    xml->xmlPrint (pXmlFp, scl_ln->varName, RT_STR_END, buff, NODE_GO, 2);

                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

    return ret;
    }

int IED::getAllLocalRgCbValues (char* pIedName, char* pApName, char* domName, STRUCT_GET_LIST* getCbList, SCL_INFO* pSclInfo)
    {
    int ret = SD_FAILURE;
    SCL_SERVER* scl_server = NULL;
    SCL_LD* scl_ld = NULL;
    SCL_LN* scl_ln = NULL;
    SCL_GCB* scl_gcb = NULL;
    SCL_SUBNET* scl_subnet = NULL;
    SCL_CAP* scl_cap = NULL;
    char data[1024] = { 0 };
    char buff[1024] = { 0 };
    char ref[1024] = { 0 };

    for (scl_server = (SCL_SERVER*)list_find_last ((DBL_LNK*)pSclInfo->serverHead);
         scl_server != NULL;
         scl_server = (SCL_SERVER*)list_find_prev ((DBL_LNK*)pSclInfo->serverHead, (DBL_LNK*)scl_server))
        {
        if (strcmp (scl_server->iedName, pIedName) == 0
            && strcmp (scl_server->apName, pApName) == 0)
            {
            for (scl_ld = (SCL_LD*)list_find_last ((DBL_LNK*)scl_server->ldHead);
                 scl_ld != NULL;
                 scl_ld = (SCL_LD*)list_find_prev ((DBL_LNK*)scl_server->ldHead, (DBL_LNK*)scl_ld))
                {
                if (strcmp (scl_ld->domName, domName) == 0)
                    {
                    for (scl_ln = (SCL_LN*)list_find_last ((DBL_LNK*)scl_ld->lnHead);
                         scl_ln != NULL;
                         scl_ln = (SCL_LN*)list_find_prev ((DBL_LNK*)scl_ld->lnHead, (DBL_LNK*)scl_ln))
                        {
                        for (scl_gcb = (SCL_GCB*)list_find_last ((DBL_LNK*)scl_ln->gcbHead);
                             scl_gcb != NULL;
                             scl_gcb = (SCL_GCB*)list_find_prev ((DBL_LNK*)scl_ln->gcbHead, (DBL_LNK*)scl_gcb))
                            {
                            if (scl_gcb->isRgoose == 1)
                                {
                                SCL_GSE* scl_gse = 0;

                                ret = getGseComm (pSclInfo, pIedName, pApName, scl_gcb->name, &scl_gse);
                                if (SD_SUCCESS == ret)
                                    {

                                    sprintf (buff, "%s/%s", domName, scl_ln->varName);
                                    xml->xmlPrint (pXmlFp, buff, RT_STR_START, scl_gcb->name, NODE_GO, 2);

                                    sprintf (ref, "%s$%s", buff, "DatSet");
                                    xml->xmlPrintElementEx (pXmlFp, ref, NODE_DATASET, 3, scl_gcb->datSet);

                                    sprintf (ref, "%s$%s", buff, "GoID");
                                    xml->xmlPrintElementEx (pXmlFp, ref, NODE_GOID, 3, scl_gcb->appID);

                                    sprintf (ref, "%s$%s", buff, "ADDR");
                                    if (inet_pton (AF_INET, data, &scl_gse->IP))
                                        {
                                        xml->xmlPrintElementEx (pXmlFp, ref, NODE_ADDR, 3, data);
                                        }
                                    else
                                        {
                                        xml->xmlPrintElementEx (pXmlFp, ref, NODE_ADDR, 3, "");
                                        }

                                    sprintf (ref, "%s$%s", buff, "MinTime");
                                    sprintf (data, "%d", scl_gse->MinTime);
                                    xml->xmlPrintElementEx (pXmlFp, ref, NODE_MINTIME, 3, data);

                                    sprintf (ref, "%s$%s", buff, "MaxTime");
                                    sprintf (data, "%d", scl_gse->MaxTime);
                                    xml->xmlPrintElementEx (pXmlFp, ref, NODE_MINTIME, 3, data);

                                    sprintf (ref, "%s$%s", buff, "PRIORITY");
                                    sprintf (data, "%d", scl_gse->VLANPRI);
                                    xml->xmlPrintElementEx (pXmlFp, ref, NODE_PRIORITY, 3, data);

                                    sprintf (ref, "%s$%s", buff, "VID");
                                    sprintf (data, "%d", scl_gse->VLANID);
                                    xml->xmlPrintElementEx (pXmlFp, ref, NODE_VID, 3, data);

                                    sprintf (ref, "%s$%s", buff, "APPID");
                                    sprintf (data, "%d", scl_gse->APPID);
                                    xml->xmlPrintElementEx (pXmlFp, ref, NODE_VID, 3, data);

                                    sprintf (ref, "%s$%s", buff, "ConfRev");
                                    sprintf (data, "%d", scl_gcb->confRev);
                                    xml->xmlPrintElementEx (pXmlFp, ref, NODE_VID, 3, data);

                                    xml->xmlPrint (pXmlFp, scl_ln->varName, RT_STR_END, buff, NODE_GO, 2);

                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

    return ret;
    }

int IED::getAllLocalBrCbValues (char* pIedName, char* pApName, char* domName, STRUCT_GET_LIST* getCbList, SCL_INFO* pSclInfo)
    {
    int ret = SD_FAILURE;



    return ret;
    }

int IED::getAllLocalUrCbValues (char* pIedName, char* pApName, char* domName, STRUCT_GET_LIST* getCbList, SCL_INFO* pSclInfo)
    {
    int ret = SD_FAILURE;



    return ret;
    }

int IED::getLocalControlBlock (char* pIedName, char* pApName, char* domName, STRUCT_GET_LIST* getCbList, SCL_INFO* pSclInfo)
    {
    int ret = SD_FAILURE;

    xml->xmlPrintNode (pXmlFp, NODE_CB, RT_STR_START, 1);

    ret = getAllLocalGoCbValues (pIedName, pApName, domName, getCbList, pSclInfo); /*TODO: Arun log*/
    ret = getAllLocalRgCbValues (pIedName, pApName, domName, getCbList, pSclInfo); /*TODO: Arun log*/

    ret = getAllLocalBrCbValues (pIedName, pApName, domName, getCbList, pSclInfo); /*TODO: Arun log*/
    ret = getAllLocalUrCbValues (pIedName, pApName, domName, getCbList, pSclInfo); /*TODO: Arun log*/

    xml->xmlPrintNode (pXmlFp, NODE_CB, RT_STR_END, 1);

    return ret;
    }

int IED::getAllGoCbValues (char* domName, STRUCT_GET_LIST* getCbList)
    {
    int ret = SD_FAILURE;
    STRUCT_LEAF* leafCb = NULL;
    STRUCT_GCB sGcb = { 0 };
    STRUCT_RCB sRcb = { 0 };
    char* leaf = NULL;
    unsigned char dataSrc[1024] = { 0 };
    char ref[1024] = { 0 };

    xml->xmlPrintNode (pXmlFp, NODE_GOCB, RT_STR_START, 1);
    for (leafCb = (STRUCT_LEAF*)list_find_last ((DBL_LNK*)getCbList->pListItem);
         leafCb != NULL;
         leafCb = (STRUCT_LEAF*)list_find_prev ((DBL_LNK*)getCbList->pListItem, (DBL_LNK*)leafCb))
        {
        leaf = strstr (leafCb->pName, domName);
        if (leaf)
            {
            if (strstr (leafCb->pName, "$GO$"))
                {
                ret = acsiGoCB->sGetGoCBValues (netInfo, domName, leaf, &sGcb, dataSrc);
                if (SD_SUCCESS == ret)
                    {
                    xml->xmlPrint (pXmlFp, sGcb.name, RT_STR_START, leafCb->pName, NODE_GO, 2);

                    sprintf (ref, "%s$%s", leafCb->pName, sGcb.DatSet);
                    xml->xmlPrintElement (pXmlFp, ref, NODE_DATASET, 3);

                    sprintf (ref, "%s$%s", leafCb->pName, sGcb.GoID);
                    xml->xmlPrintElement (pXmlFp, ref, NODE_GOID, 3);

                    sprintf (ref, "%s$%s", leafCb->pName, sGcb.ADDR);
                    xml->xmlPrintElement (pXmlFp, ref, NODE_ADDR, 3);

                    sprintf (ref, "%s$%s", leafCb->pName, sGcb.MinTime);
                    xml->xmlPrintElement (pXmlFp, ref, NODE_MINTIME, 3);

                    sprintf (ref, "%s$%s", leafCb->pName, sGcb.MaxTime);
                    xml->xmlPrintElement (pXmlFp, ref, NODE_MAXTIME, 3);

                    sprintf (ref, "%s$%s", leafCb->pName, sGcb.PRIORITY);
                    xml->xmlPrintElement (pXmlFp, ref, NODE_PRIORITY, 3);

                    sprintf (ref, "%s$%s", leafCb->pName, sGcb.VID);
                    xml->xmlPrintElement (pXmlFp, ref, NODE_VID, 3);

                    sprintf (ref, "%s$%s", leafCb->pName, sGcb.APPID);
                    xml->xmlPrintElement (pXmlFp, ref, NODE_APPID, 3);

                    sprintf (ref, "%s$%s", leafCb->pName, sGcb.ConfRev);
                    xml->xmlPrintElement (pXmlFp, ref, NODE_CONFREV, 3);

                    xml->xmlPrint (pXmlFp, sGcb.name, RT_STR_END, leafCb->pName, NODE_GO, 2);
                    }
                }
            }
        }

    xml->xmlPrintNode (pXmlFp, NODE_GOCB, RT_STR_END, 1);

    return ret;
    }

int IED::getAllRgCbValues (char* domName, STRUCT_GET_LIST* getCbList)
    {
    int ret = SD_FAILURE;
    STRUCT_LEAF* leafCb = NULL;
    STRUCT_GCB sGcb = { 0 };
    STRUCT_RCB sRcb = { 0 };
    char* leaf = NULL;
    unsigned char dataSrc[1024] = { 0 };
    char ref[1024] = { 0 };

    xml->xmlPrintNode (pXmlFp, NODE_RGCB, RT_STR_START, 1);
    for (leafCb = (STRUCT_LEAF*)list_find_last ((DBL_LNK*)getCbList->pListItem);
         leafCb != NULL;
         leafCb = (STRUCT_LEAF*)list_find_prev ((DBL_LNK*)getCbList->pListItem, (DBL_LNK*)leafCb))
        {
        leaf = strstr (leafCb->pName, domName);
        if (leaf)
            {
            if (strstr (leafCb->pName, "$RG$"))
                {
                ret = acsiGoCB->sGetGoCBValues (netInfo, domName, leaf, &sGcb, dataSrc);
                if (SD_SUCCESS == ret)
                    {
                    xml->xmlPrint (pXmlFp, sGcb.name, RT_STR_START, leafCb->pName, NODE_RG, 2);

                    sprintf (ref, "%s$%s", leafCb->pName, sGcb.DatSet);
                    xml->xmlPrintElement (pXmlFp, ref, NODE_DATASET, 3);

                    sprintf (ref, "%s$%s", leafCb->pName, sGcb.GoID);
                    xml->xmlPrintElement (pXmlFp, ref, NODE_GOID, 3);

                    sprintf (ref, "%s$%s", leafCb->pName, sGcb.ADDR);
                    xml->xmlPrintElement (pXmlFp, ref, NODE_ADDR, 3);

                    sprintf (ref, "%s$%s", leafCb->pName, sGcb.MinTime);
                    xml->xmlPrintElement (pXmlFp, ref, NODE_MINTIME, 3);

                    sprintf (ref, "%s$%s", leafCb->pName, sGcb.MaxTime);
                    xml->xmlPrintElement (pXmlFp, ref, NODE_MAXTIME, 3);

                    sprintf (ref, "%s$%s", leafCb->pName, sGcb.PRIORITY);
                    xml->xmlPrintElement (pXmlFp, ref, NODE_PRIORITY, 3);

                    sprintf (ref, "%s$%s", leafCb->pName, sGcb.VID);
                    xml->xmlPrintElement (pXmlFp, ref, NODE_VID, 3);

                    sprintf (ref, "%s$%s", leafCb->pName, sGcb.APPID);
                    xml->xmlPrintElement (pXmlFp, ref, NODE_APPID, 3);

                    sprintf (ref, "%s$%s", leafCb->pName, sGcb.ConfRev);
                    xml->xmlPrintElement (pXmlFp, ref, NODE_CONFREV, 3);

                    xml->xmlPrint (pXmlFp, sGcb.name, RT_STR_END, leafCb->pName, NODE_RG, 2);
                    }
                }
            }
        }

    xml->xmlPrintNode (pXmlFp, NODE_RGCB, RT_STR_END, 1);

    return ret;
    }

int IED::getAllBrCbValues (char* domName, STRUCT_GET_LIST* getCbList)
    {
    int ret = SD_FAILURE;
    STRUCT_LEAF* leafCb = NULL;
    STRUCT_GCB sGcb = { 0 };
    STRUCT_RCB sRcb = { 0 };
    char* leaf = NULL;
    unsigned char dataSrc[1024] = { 0 };
    char ref[1024] = { 0 };

    xml->xmlPrintNode (pXmlFp, NODE_BRCB, RT_STR_START, 1);
    for (leafCb = (STRUCT_LEAF*)list_find_last ((DBL_LNK*)getCbList->pListItem);
         leafCb != NULL;
         leafCb = (STRUCT_LEAF*)list_find_prev ((DBL_LNK*)getCbList->pListItem, (DBL_LNK*)leafCb))
        {
        leaf = strstr (leafCb->pName, domName);
        if (leaf)
            {
            if (strstr (leafCb->pName, "$BR$"))
                {
                ret = acsiBRCB->sGetBRCBValues (netInfo, domName, leaf, &sRcb, dataSrc);
                if (SD_SUCCESS == ret)
                    {
                    xml->xmlPrint (pXmlFp, sRcb.name, RT_STR_START, leafCb->pName, NODE_BR, 2);

                    sprintf (ref, "%s$%s", leafCb->pName, sRcb.DatSet);
                    xml->xmlPrintElement (pXmlFp, ref, NODE_DATASET, 3);

                    sprintf (ref, "%s$%s", leafCb->pName, sRcb.RptID);
                    xml->xmlPrintElement (pXmlFp, ref, NODE_RPTID, 3);

                    sprintf (ref, "%s$%s", leafCb->pName, sRcb.TrgOps);
                    xml->xmlPrintElement (pXmlFp, ref, NODE_TRGOPS, 3);

                    sprintf (ref, "%s$%s", leafCb->pName, sRcb.BufTm);
                    xml->xmlPrintElement (pXmlFp, ref, NODE_BUFTIME, 3);

                    sprintf (ref, "%s$%s", leafCb->pName, sRcb.OptFlds);
                    xml->xmlPrintElement (pXmlFp, ref, NODE_OPTFLDS, 3);

                    sprintf (ref, "%s$%s", leafCb->pName, sRcb.ConfRev);
                    xml->xmlPrintElement (pXmlFp, ref, NODE_CONFREV, 3);

                    xml->xmlPrint (pXmlFp, sRcb.name, RT_STR_END, leafCb->pName, NODE_BR, 2);
                    }
                }
            }
        }

    xml->xmlPrintNode (pXmlFp, NODE_BRCB, RT_STR_END, 1);

    return ret;
    }

int IED::getAllUrCbValues (char* domName, STRUCT_GET_LIST* getCbList)
    {
    int ret = SD_FAILURE;
    STRUCT_LEAF* leafCb = NULL;
    STRUCT_GCB sGcb = { 0 };
    STRUCT_RCB sRcb = { 0 };
    char* leaf = NULL;
    unsigned char dataSrc[1024] = { 0 };
    char ref[1024] = { 0 };

    xml->xmlPrintNode (pXmlFp, NODE_URCB, RT_STR_START, 1);
    for (leafCb = (STRUCT_LEAF*)list_find_last ((DBL_LNK*)getCbList->pListItem);
         leafCb != NULL;
         leafCb = (STRUCT_LEAF*)list_find_prev ((DBL_LNK*)getCbList->pListItem, (DBL_LNK*)leafCb))
        {
        leaf = strstr (leafCb->pName, domName);
        if (leaf)
            {
            if (strstr (leafCb->pName, "$RP$"))
                {
                ret = acsiBRCB->sGetBRCBValues (netInfo, domName, leaf, &sRcb, dataSrc);
                if (SD_SUCCESS == ret)
                    {
                    xml->xmlPrint (pXmlFp, sRcb.name, RT_STR_START, ref, NODE_RP, 2);

                    sprintf (ref, "%s$%s", leafCb->pName, sRcb.DatSet);
                    xml->xmlPrintElement (pXmlFp, ref, NODE_DATASET, 3);

                    sprintf (ref, "%s$%s", leafCb->pName, sRcb.RptID);
                    xml->xmlPrintElement (pXmlFp, ref, NODE_RPTID, 3);

                    sprintf (ref, "%s$%s", leafCb->pName, sRcb.TrgOps);
                    xml->xmlPrintElement (pXmlFp, ref, NODE_TRGOPS, 3);

                    sprintf (ref, "%s$%s", leafCb->pName, sRcb.BufTm);
                    xml->xmlPrintElement (pXmlFp, ref, NODE_BUFTIME, 3);

                    sprintf (ref, "%s$%s", leafCb->pName, sRcb.OptFlds);
                    xml->xmlPrintElement (pXmlFp, ref, NODE_OPTFLDS, 3);

                    sprintf (ref, "%s$%s", leafCb->pName, sRcb.ConfRev);
                    xml->xmlPrintElement (pXmlFp, ref, NODE_CONFREV, 3);

                    xml->xmlPrint (pXmlFp, sRcb.name, RT_STR_END, ref, NODE_RP, 2);
                    }
                }
            }
        }

    xml->xmlPrintNode (pXmlFp, NODE_URCB, RT_STR_END, 1);

    return ret;
    }


int IED::getControlBlock (char* domName, STRUCT_GET_LIST* getCbList)
    {
    int ret = SD_FAILURE;

    if (getCbList->pListItem)
        {
        xml->xmlPrintNode (pXmlFp, NODE_CB, RT_STR_START, 1);

        ret = getAllGoCbValues (domName, getCbList); /*TODO: Arun log*/
        ret = getAllRgCbValues (domName, getCbList); /*TODO: Arun log*/

        ret = getAllBrCbValues (domName, getCbList); /*TODO: Arun log*/
        ret = getAllUrCbValues (domName, getCbList); /*TODO: Arun log*/

        xml->xmlPrintNode (pXmlFp, NODE_CB, RT_STR_END, 1);
        }

    return ret;
    }

int IED::getDataset (char* domName)
    {
    int ret = SD_FAILURE;
    STRUCT_LEAF* leafDs = NULL;
    STRUCT_LEAF* leafDe = NULL;
    STRUCT_GET_LIST getDeList = { 0 };
    STRUCT_GET_LIST getDsList = { 0 };
    char dsName[MAX_AR_LEN + 1] = { 0 };
    char ref[1024] = { 0 };
    char* pOp = NULL;

    ret = getDS (&getDsList, domName);

    if (SD_SUCCESS == ret && getDsList.count > 0)
        {
        xml->xmlPrintNode (pXmlFp, NODE_DATASET, RT_STR_START, 1);

        for (leafDs = (STRUCT_LEAF*)list_find_last ((DBL_LNK*)getDsList.pListItem);
             leafDs != NULL;
             leafDs = (STRUCT_LEAF*)list_find_prev ((DBL_LNK*)getDsList.pListItem, (DBL_LNK*)leafDs))
            {

            pOp = strstr (leafDs->pName, "$");
            if (pOp)
                {
                strncpy (dsName, &pOp[1], sizeof (dsName));
                }
            else
                {
                strncpy (dsName, leafDs->pName, sizeof (dsName));
                }

            sprintf (ref, "%s/LLN0$%s", domName, dsName);
            xml->xmlPrint (pXmlFp, dsName, RT_STR_START, ref, NODE_DS, 2);

            getDsStruct (&getDeList, domName, leafDs->pName);
            for (leafDe = (STRUCT_LEAF*)list_find_last ((DBL_LNK*)getDeList.pListItem);
                 leafDe != NULL;
                 leafDe = (STRUCT_LEAF*)list_find_prev ((DBL_LNK*)getDeList.pListItem, (DBL_LNK*)leafDe))
                {
                xml->xmlPrintElement (pXmlFp, leafDe->pName, NODE_LEAF, 3);
                }

            xml->xmlPrint (pXmlFp, dsName, RT_STR_END, ref, NODE_DS, 2);
            listClear (getDeList.pListItem);
            memset (&getDeList, 0, sizeof (getDeList));
            }

        listClear (getDsList.pListItem);
        memset (&getDsList, 0, sizeof (getDsList));
        xml->xmlPrintNode (pXmlFp, NODE_DATASET, RT_STR_END, 1);
        }

    return ret;
    }

int IED::getRemoteDataModel (int keepConnected, char* pXmlPath)
    {
    int ret = SD_SUCCESS;
    STRUCT_LEAF* leafLd = NULL;
    STRUCT_LEAF* leafLn = NULL;
    STRUCT_GET_LIST getLdList = { 0 };
    STRUCT_GET_LIST getLnList = { 0 };
    STRUCT_GET_LIST getCbList = { 0 };
    STRUCT_GET getLnStruct = { 0 };
    char ref[1024] = { 0 };
    int ld = 0;

    pXmlFp = fopen (pXmlPath, "w");

    if (pXmlFp)
        {
        fprintf (pXmlFp, "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");
        ret = iedConnect ();

        if (SD_SUCCESS == ret)
            {
            ret = getLD (&getLdList);

            if (SD_SUCCESS == ret)
                {
                getIedName (&getLdList, iedName);
                pipeWrite ("REMOTE", "IED", iedName, iId, ret, 0);
                xml->xmlPrint (pXmlFp, iedName, RT_STR_START, iedName, NODE_IED, 0);

                ld = 0;
                for (leafLd = (STRUCT_LEAF*)list_find_last ((DBL_LNK*)getLdList.pListItem);
                     leafLd != NULL;
                     leafLd = (STRUCT_LEAF*)list_find_prev ((DBL_LNK*)getLdList.pListItem, (DBL_LNK*)leafLd))
                    {
                    MIN_SLEEP;
                    pipeWrite ("REMOTE", "LD", &leafLd->pName[strlen (iedName)], iId, ret, ld * (MAX_PERCENTAGE / getLdList.count));
                    ld++;
                    xml->xmlPrint (pXmlFp, &leafLd->pName[strlen (iedName)], RT_STR_START, leafLd->pName, NODE_LD, 1);
                    ret = getLN (&getLnList, leafLd->pName);

                    if (SD_SUCCESS == ret)
                        {
                        for (leafLn = (STRUCT_LEAF*)list_find_last ((DBL_LNK*)getLnList.pListItem);
                             leafLn != NULL;
                             leafLn = (STRUCT_LEAF*)list_find_prev ((DBL_LNK*)getLnList.pListItem, (DBL_LNK*)leafLn))
                            {
                            MIN_SLEEP;
                            ret = getDataStruct (&getLnStruct, leafLd->pName, leafLn->pName);
                            if (SD_SUCCESS == ret)
                                {
                                sprintf (ref, "%s/%s", leafLd->pName, leafLn->pName);

                                pipeWrite ("REMOTE", "LD", leafLn->pName, iId, ret, ld * (MAX_PERCENTAGE / getLdList.count));
                                xml->xmlPrint (pXmlFp, leafLn->pName, RT_STR_START, ref, NODE_LN, 2);
                                printlnStruct (getLnStruct.rtPtrOut, getLnStruct.numRtOut, ref, &getCbList);
                                xml->xmlPrint (pXmlFp, leafLn->pName, RT_STR_END, ref, NODE_LN, 2);

                                deleteDataStruct (&getLnStruct);
                                }
                            }
                        }

                    getDataset (leafLd->pName); /*TODO: Arun LOG for return fail*/
                    getControlBlock (leafLd->pName, &getCbList); /*TODO: Arun LOG for return fail*/

                    listClear (getCbList.pListItem);
                    listClear (getLnList.pListItem);
                    memset (&getLnList, 0, sizeof (getLnList));
                    xml->xmlPrint (pXmlFp, &leafLd->pName[strlen (iedName)], RT_STR_END, leafLd->pName, NODE_LD, 1);
                    }

                listClear (getLdList.pListItem);
                memset (&getLdList, 0, sizeof (getLdList));
                xml->xmlPrint (pXmlFp, iedName, RT_STR_END, iedName, NODE_IED, 0);
                }

            if (keepConnected == 0)
                {
                iedDisconnect ();
                }
            }

        fclose (pXmlFp);
        }
    else
        {
        ret = SD_FAILURE;
        }

    return ret;
    }

int IED::getSclInfo (char* pSclPath, SCL_INFO* pSclInfo)
    {
    return local->getSclData (pSclPath, pSclInfo);
    }

int IED::getVmdInfo (char* pXmlPath, char* pSclPath, char* pIedName, char* pApName)
    {
    int ret = SD_FAILURE;
    MVL_VMD_CTRL* vmd_ctrl = NULL;
    SCL_INFO sclInfo = { 0 };
    MVL_DOM_CTRL* dom = NULL;
    MVL_VAR_ASSOC* var = NULL;
    STRUCT_GET getLnStruct = { 0 };
    char ref[1024] = { 0 };
    STRUCT_GET_LIST getCbList = { 0 };


    pXmlFp = fopen (pXmlPath, "w");

    if (pXmlFp)
        {
        fprintf (pXmlFp, "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");

        ret = local->getVmdData (pSclPath, pIedName, pApName, &vmd_ctrl, &sclInfo);
        if (SD_SUCCESS == ret)
            {
#if 0
            pipeWrite ("LOCAL", "IED", iedName, iId, ret, 0);
            xml->xmlPrint (pXmlFp, pIedName, RT_STR_START, pIedName, NODE_IED, 0);
            for (int ld = 0; ld < vmd_ctrl->num_dom; ld++)
                {
                MIN_SLEEP;
                dom = vmd_ctrl->dom_tbl[ld];
                pipeWrite ("LOCAL", "LD", &vmd_ctrl->dom_tbl[ld]->name[strlen (pIedName)], iId, ret, (MAX_PERCENTAGE / vmd_ctrl->num_dom) * ld);
                xml->xmlPrint (pXmlFp, &vmd_ctrl->dom_tbl[ld]->name[strlen (pIedName)], RT_STR_START, vmd_ctrl->dom_tbl[ld]->name, NODE_LD, 1);

                for (int ln = 0; ln < dom->num_var_assoc; ln++)
                    {
                    MIN_SLEEP;
                    var = dom->var_assoc_tbl[ln];

                    ret = mvl_get_runtime (var->type_id, &getLnStruct.rtPtrOut, &getLnStruct.numRtOut);
                    if (SD_SUCCESS == ret)
                        {
                        sprintf (ref, "%s/%s", vmd_ctrl->dom_tbl[ld]->name, var->name);

                        pipeWrite ("LOCAL", "LD", var->name, iId, ret, (MAX_PERCENTAGE / vmd_ctrl->num_dom) * ld);
                        xml->xmlPrint (pXmlFp, var->name, RT_STR_START, ref, NODE_LN, 2);
                        printlnStruct (getLnStruct.rtPtrOut, getLnStruct.numRtOut, ref, &getCbList);
                        xml->xmlPrint (pXmlFp, var->name, RT_STR_END, ref, NODE_LN, 2);
                        }
                    }

                getLocalDataset (vmd_ctrl->dom_tbl[ld]->name, dom); /*TODO: Arun LOG for return fail*/
                getLocalControlBlock (pIedName, pApName, vmd_ctrl->dom_tbl[ld]->name, &getCbList, &sclInfo); /*TODO: Arun LOG for return fail*/

                listClear (getCbList.pListItem);

                xml->xmlPrint (pXmlFp, &vmd_ctrl->dom_tbl[ld]->name[strlen (pIedName)], RT_STR_END, vmd_ctrl->dom_tbl[ld]->name, NODE_LD, 1);
                }

            xml->xmlPrint (pXmlFp, pIedName, RT_STR_END, pIedName, NODE_IED, 0);
#endif

#if 1
            mvl_vmd_destroy (vmd_ctrl);
#endif
            }

        scl_info_destroy (&sclInfo);
        fclose (pXmlFp);
        }

    return ret;
    }

int IED::getMappedIeds (SCL_INFO* scl_info, SCL_SERVER* pSclServer, STRUCT_GET_LIST* getList)
    {
    int ret = SD_SUCCESS;
    SCL_SERVER* scl_server;
    SCL_LD* scl_ld;
    SCL_LN* scl_ln;
    SCL_EXTREF* scl_extref;
    STRUCT_LEAF* leaf = NULL;

    if (pSclServer)
        {
        for (scl_ld = (SCL_LD*)list_find_last ((DBL_LNK*)pSclServer->ldHead);
             scl_ld != NULL;
             scl_ld = (SCL_LD*)list_find_prev ((DBL_LNK*)pSclServer->ldHead, (DBL_LNK*)scl_ld))
            {
            for (scl_ln = (SCL_LN*)list_find_last ((DBL_LNK*)scl_ld->lnHead);
                 scl_ln != NULL;
                 scl_ln = (SCL_LN*)list_find_prev ((DBL_LNK*)scl_ld->lnHead, (DBL_LNK*)scl_ln))
                {
                if (scl_ln->extrefHead)
                    {
                    for (scl_extref = (SCL_EXTREF*)list_find_last ((DBL_LNK*)scl_ln->extrefHead);
                         scl_extref != NULL;
                         scl_extref = (SCL_EXTREF*)list_find_prev ((DBL_LNK*)scl_ln->extrefHead, (DBL_LNK*)scl_extref))
                        {
                        if (strcmp (scl_extref->serviceType, "GOOSE") == 0)
                            {
                            /*TODO: Arun validate extRef*/
                            for (scl_server = (SCL_SERVER*)list_find_last ((DBL_LNK*)scl_info->serverHead);
                                 scl_server != NULL;
                                 scl_server = (SCL_SERVER*)list_find_prev ((DBL_LNK*)scl_info->serverHead, (DBL_LNK*)scl_server))
                                {
                                if (strcmp (scl_server->iedName, scl_extref->iedName) == 0)
                                    {
                                    for (leaf = (STRUCT_LEAF*)list_find_last ((DBL_LNK*)getList->pListItem);
                                         leaf != NULL;
                                         leaf = (STRUCT_LEAF*)list_find_prev ((DBL_LNK*)getList->pListItem, (DBL_LNK*)leaf))
                                        {
                                        if (strcmp (scl_extref->iedName, leaf->pName) == 0)
                                            {
                                            break;
                                            }
                                        }

                                    if (leaf == NULL)
                                        {
                                        STRUCT_LEAF* leaf = (STRUCT_LEAF*)chk_calloc (1, sizeof (STRUCT_LEAF));
                                        if (leaf == NULL)
                                            {
                                            pClientLog ("ALLOC", "ERROR", -1);
                                            }
                                        leaf->pName = (char*)chk_calloc (1, strlen (scl_extref->iedName) + 1);
                                        if (leaf->pName == NULL)
                                            {
                                            pClientLog ("ALLOC", "ERROR", -1);
                                            }

                                        strcpy (leaf->pName, scl_extref->iedName);
                                        list_add_last (&getList->pListItem, leaf);
                                        getList->count++;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    else
        {
        ret = SD_FAILURE;
        }

    return ret;
    }

int IED::getEdition (SCL_INFO* scl_info, SCL_SERVER* scl_server, int* edition)
    {
    int ret = SD_SUCCESS;
    SCL_LD* scl_ld = NULL;
    SCL_LN* scl_ln = NULL;
    SCL_LNTYPE* ln_type = NULL;
    SCL_DOTYPE* do_type = NULL;
    SCL_DOTYPE* scl_do_type = NULL;

    for (scl_ld = (SCL_LD*)list_find_last ((DBL_LNK*)scl_server->ldHead);
         scl_ld != NULL;
         scl_ld = (SCL_LD*)list_find_prev ((DBL_LNK*)scl_server->ldHead, (DBL_LNK*)scl_ld))

        for (scl_ln = (SCL_LN*)list_find_last ((DBL_LNK*)scl_ld->lnHead);
             scl_ln != NULL;
             scl_ln = (SCL_LN*)list_find_prev ((DBL_LNK*)scl_ld->lnHead, (DBL_LNK*)scl_ln))

            if (scl_info->lnTypeHead)

                for (ln_type = (SCL_LNTYPE*)list_find_last ((DBL_LNK*)scl_info->lnTypeHead);
                     ln_type != NULL;
                     ln_type = (SCL_LNTYPE*)list_find_prev ((DBL_LNK*)scl_info->lnTypeHead, (DBL_LNK*)ln_type))

                    if (ln_type->id && scl_ln->lnType)

                        if (strcmp (ln_type->id, scl_ln->lnType) == 0)

                            for (scl_do_type = (SCL_DOTYPE*)list_find_last ((DBL_LNK*)ln_type->doHead);
                                 scl_do_type != NULL;
                                 scl_do_type = (SCL_DOTYPE*)list_find_prev ((DBL_LNK*)ln_type->doHead, (DBL_LNK*)scl_do_type))

                                if (strcmp (scl_do_type->id, "Beh") == 0)

                                    for (do_type = (SCL_DOTYPE*)list_find_last ((DBL_LNK*)scl_info->doTypeHead);
                                         do_type != NULL;
                                         do_type = (SCL_DOTYPE*)list_find_prev ((DBL_LNK*)scl_info->doTypeHead, (DBL_LNK*)do_type))

                                        if (scl_do_type->cdc && do_type->id)

                                            if (strcmp (scl_do_type->cdc, do_type->id) == 0)

                                                if (strcmp (do_type->cdc, "ENS") == 0
                                                    || strcmp (do_type->cdc, "ENC") == 0)
                                                    {
                                                    *edition = 2;
                                                    return SD_SUCCESS;
                                                    }

    *edition = 1;

    return ret;
    }

int IED::iedConnect ()
    {
    int ret = SD_SUCCESS;

    if (acsiAssociation)
        {
        if (STATUS_DISCONNECTED == associationStatus)
            {
            ret = acsiAssociation->sAssociate (sId, &netInfo);
            if (SD_SUCCESS == ret)
                {
                if (netInfo == NULL)
                    {
                    pClientLog ("CODE 21", "ERROR", -1);
                    }
                associationStatus = STATUS_CONNECTED;
                }
            }
        }
    else
        {
        pClientLog ("CODE 22", "ERROR", -1);
        }

    return ret;
    }

int IED::iedDisconnect ()
    {
    int ret = SD_SUCCESS;

    if (acsiAssociation)
        {
        if (STATUS_CONNECTED == associationStatus)
            {
            if (NULL == netInfo)
                {
                pClientLog ("CODE 23", "ERROR", -1);
                }
            ret = acsiAssociation->sRelease (netInfo);
            if (SD_SUCCESS != ret)
                {
                associationStatus = STATUS_DISCONNECTED;
                netInfo = NULL;
                /*pClientLog ("CODE 24", "ERROR", -1);*/
                }
            else
                {
                associationStatus = STATUS_DISCONNECTED;
                netInfo = NULL;
                }
            }
        }
    else
        {
        pClientLog ("CODE 25", "ERROR", -1);
        }

    return ret;
    }

int IED::setDataEx (char* domName, char* leaf, char* data, char* error)
    {
    int ret = SD_SUCCESS;

    ret = acsiData->sSetDataValuesEx (netInfo, domName, leaf, data, error);

    return ret;
    }

int IED::setData (char* domName, char* leaf, char* data, int type, int len)
    {
    int ret = SD_SUCCESS;

    ret = acsiData->sSetDataValues (netInfo, domName, leaf, type, len, data);

    return ret;
    }

int IED::getDataAsync (POLL_ARG_STRUCT* leafItem)
    {
    int ret = SD_SUCCESS;

    if (leafItem)
        {
        ret = acsiData->sGetDataValuesAsync (netInfo,
                                             leafItem->domName,
                                             leafItem->leaf,
                                             leafItem->type,
                                             leafItem->len,
                                             leafItem->result,
                                             &leafItem->reqCtrl,
                                             &leafItem->req_info,
                                             &leafItem->stopTime,
                                             &leafItem->parse_info);
        if (ret == SD_SUCCESS)
            {
            leafItem->state = REQ_SEND;
            }
        else
            {
            leafItem->state = REQ_NOT_SEND;
            }
        }
    else
        {
        ret = SD_FAILURE;
        }

    return ret;
    }

int IED::select (char* domName, char* leaf)
    {
    int ret = SD_SUCCESS;
    unsigned char dataSrc[2048] = { 0 };

    ret = acsiControl->sSelect (netInfo, domName, leaf);

    return ret;
    }

int IED::selectWithValue (char* domName,
                          char* leafOper,
                          char* leafSBOw,
                          char* data,
                          char* addcause,
                          char* orCat,
                          char* interlockCheck,
                          char* synchrocheck,
                          char* test,
                          int timeInSec)
    {
    int ret = SD_SUCCESS;
    unsigned char dataSrc[2048] = { 0 };

    ret = acsiControl->sSelectWithValue (netInfo,
                                         domName,
                                         leafOper,
                                         leafSBOw,
                                         data,
                                         addcause,
                                         orCat,
                                         interlockCheck,
                                         synchrocheck,
                                         test,
                                         timeInSec);

    return ret;
    }

int IED::operate (char* domName,
                  char* leaf,
                  char* data,
                  char* addcause,
                  char* orCat,
                  char* interlockCheck,
                  char* synchrocheck,
                  char* test,
                  int timeInSec,
                  int ctlModel)
    {
    int ret = SD_SUCCESS;
    unsigned char dataSrc[2048] = { 0 };

    ret = acsiControl->sOperate (netInfo,
                                 domName,
                                 leaf,
                                 data,
                                 addcause,
                                 orCat,
                                 interlockCheck,
                                 synchrocheck,
                                 test,
                                 timeInSec,
                                 ctlModel);

    return ret;
    }

int IED::setStructData (char* domName, char* leaf, char* data)
    {
    int ret = SD_SUCCESS;
    unsigned char dataSrc[2048] = { 0 };

    ret = acsiData->sSetStructDataValues (netInfo, domName, leaf, data);

    return ret;
    }

int IED::getStructData (char* domName, char* leaf, char* data, int allocSize)
    {
    int ret = SD_SUCCESS;
    unsigned char dataSrc[2048] = { 0 };

    ret = acsiData->sGetStructDataValues (netInfo, domName, leaf, data, allocSize, dataSrc);

    return ret;
    }

int IED::getDataEx (char* domName, char* leaf, char* data)
    {
    int ret = SD_SUCCESS;

    ret = acsiData->sGetDataValuesEx (netInfo, domName, leaf, data);

    return ret;
    }

int IED::getData (char* domName, char* leaf, char* data, int type, int len)
    {
    int ret = SD_SUCCESS;

    ret = acsiData->sGetDataValues (netInfo, domName, leaf, type, len, data);

    return ret;
    }

/*functions*/
int
getIedTreeLite (int id, char* iedName)
    {
    int ret = SD_SUCCESS;
    std::map <int, IED*>::iterator ied = gIedList.find (id);

    if (ied == gIedList.end ())
        {
        ret = SD_FAILURE;
        }
    else
        {
        ret = ied->second->getRemoteDataModellLite ();
        if (SD_SUCCESS == ret)
            {
            strcpy (iedName, ied->second->iedName);
            }
        else
            {
            strcpy (iedName, "");
            }
        }

    return ret;
    }

int
getIedListLocal (int id, char* pSclPath, char* result, int size)
    {
    int ret = SD_SUCCESS;
    SCL_INFO sclInfo = { 0 };
    int edition = 1;
    SCL_SERVER* scl_server = NULL;
    std::map <int, IED*>::iterator ied = gIedList.find (id);
#if 0
    STRUCT_GET_LIST getList = { 0 };
#endif
    char tempResult[1024] = { 0 };
    int flg = 0;
    STRUCT_LEAF* leaf = NULL;

    strcpy (result, "");
    if (ied == gIedList.end ())
        {
        ret = SD_FAILURE;
        }
    else
        {
        ret = ied->second->getSclInfo (pSclPath, &sclInfo);
        if (SD_SUCCESS == ret)
            {
#if 1
            for (scl_server = (SCL_SERVER*)list_find_last ((DBL_LNK*)sclInfo.serverHead);
                 scl_server != NULL;
                 scl_server = (SCL_SERVER*)list_find_prev ((DBL_LNK*)sclInfo.serverHead, (DBL_LNK*)scl_server))
                {
#if 0
                getList.count = 0;
                ied->second->getEdition (&sclInfo, scl_server, &edition);
                ied->second->getMappedIeds (&sclInfo, scl_server, &getList);
#endif

                sprintf (tempResult, "%s,%s,%d,%d", scl_server->iedName, scl_server->apName, edition,
#if 0
                         getList.count
#else
                         0
#endif
                );

                if (flg == 0)
                    {
                    flg = 1;
                    strcat_s (result, size, tempResult);
                    }
                else
                    {
                    strcat_s (result, size, "|");
                    strcat_s (result, size, tempResult);
                    }

#if 0
                if (getList.pListItem)
                    {
                    for (leaf = (STRUCT_LEAF*)list_find_last ((DBL_LNK*)getList.pListItem);
                        leaf != NULL;
                        leaf = (STRUCT_LEAF*)list_find_prev ((DBL_LNK*)getList.pListItem, (DBL_LNK*)leaf))
                        {
                        strcat_s (result, size, ",");
                        strcat_s (result, size, leaf->pName);
                        }
                    listClear (getList.pListItem);
                    getList.pListItem = NULL;
                    }
#endif
                }
#endif
            scl_info_destroy (&sclInfo);
            }
        }

    return ret;
    }

void
getIedThread (THREAD_DATA_STRUCT* pDataArray)
    {
    int ret = SD_SUCCESS;
    std::map <int, IED*>::iterator ied = gIedList.find (pDataArray->id);

    if (ied == gIedList.end ())
        {
        ret = SD_FAILURE;
        }
    else
        {
        ret = ied->second->getRemoteDataModel (pDataArray->keepConnected, pDataArray->pXmlPath);
        }

    pipeWrite ("REMOTE", "OVERALL", "DONE", pDataArray->id, ret, MAX_PERCENTAGE);

    delete pDataArray;
    }

void
getIedLocalThread (THREAD_DATA_STRUCT* pDataArray)
    {
    int ret = SD_SUCCESS;
    std::map <int, IED*>::iterator ied = gIedList.find (pDataArray->id);

    if (ied == gIedList.end ())
        {
        ret = SD_FAILURE;
        }
    else
        {
        ret = ied->second->getVmdInfo (pDataArray->pXmlPath, pDataArray->pSclPath, pDataArray->pIedName, pDataArray->pApName);
        }

    pipeWrite ("LOCAL", "OVERALL", "DONE", pDataArray->id, ret, MAX_PERCENTAGE);

    delete pDataArray;
    }

void pipeWrite (char* type, char* subType, char* status, int id, int ret, int percentage)
    {
    char toUi[MAX_PIPE_BUFF_LEN] = { 0 };

    if (pipe)
        {
        if (pipe->finished)
            {
            sprintf (toUi, "%s,%s,%s,%d,%d,%d", type, subType, status, id, percentage, ret);
            pipe->write (toUi);
            }
        }
    }

void
pollResLoop ()
    {
    std::map <int, IED*>::iterator it;
    int count = 0;
    POLL_ARG_STRUCT* leaf = NULL;
    char data[MAX_FILE_NAME];

    while (doIt)
        {
        count = 0;
        for (it = gIedList.begin (); it != gIedList.end (); it++)
            {
            for (leaf = (POLL_ARG_STRUCT*)list_find_last ((DBL_LNK*)it->second->leafList->listItem);
                 leaf != NULL;
                 leaf = (POLL_ARG_STRUCT*)list_find_prev ((DBL_LNK*)it->second->leafList->listItem, (DBL_LNK*)leaf))
                {
                if (leaf->state == REQ_SEND)
                    {
                    if (leaf->reqCtrl->done == SD_TRUE)
                        {
                        leaf->stopTime = 0;

                        valToString (leaf->result, data, leaf->type, abs (leaf->len));
                        pipeWrite ("READ", "LEAF", data, it->second->iId, SD_SUCCESS, 0);

                        chk_free (leaf->req_info);
                        chk_free (leaf->parse_info);
                        mvl_free_req_ctrl (leaf->reqCtrl);
                        leaf->state = REQ_NOT_SEND;
                        }
                    if (leaf->stopTime != 0 && sGetMsTime () > leaf->stopTime)
                        {
                        leaf->stopTime = 0;
                        pipeWrite ("READ", "LEAF", "NULL", it->second->iId, MVL_ERR_USR_TIMEOUT, 0);

                        chk_free (leaf->req_info);
                        chk_free (leaf->parse_info);
                        mvl_free_req_ctrl (leaf->reqCtrl);
                        leaf->state = REQ_NOT_SEND;
                        }
                    count++;
                    }
                }
            }

        if (count)
            {
            MIN_POLL_RES_SLEEP;
            }
        else
            {
            MAX_POLL_SLEEP;
            }
        }
    }

void
pollReqLoop ()
    {
    std::map <int, IED*>::iterator it;
    int count = 0;
    POLL_ARG_STRUCT* leaf = NULL;

    while (doIt)
        {
        count = 0;
        for (it = gIedList.begin (); it != gIedList.end (); it++)
            {
            if (it->second->associationStatus == STATUS_CONNECTED)
                {
                for (leaf = (POLL_ARG_STRUCT*)list_find_last ((DBL_LNK*)it->second->leafList->listItem);
                     leaf != NULL;
                     leaf = (POLL_ARG_STRUCT*)list_find_prev ((DBL_LNK*)it->second->leafList->listItem, (DBL_LNK*)leaf))
                    {
                    if (leaf->state == REQ_NOT_SEND)
                        {
                        it->second->getDataAsync (leaf);
                        count++;
                        }
                    }
                }
            }

        if (count)
            {
            MIN_POLL_REQ_SLEEP;
            }
        else
            {
            MAX_POLL_SLEEP;
            }
        }
    }

void
doComLoop ()
    {
    while (doIt)
        {
        doCommService ();   /* wait for events if possible & call mvl_comm_serve*/
        MIN_COM_SLEEP;
        }
    }

int
makeThread (LPTHREAD_START_ROUTINE fn, void* pData)
    {
    int ret = SD_SUCCESS;
    DWORD dwThreadId;
    HANDLE  hThread;

    hThread = CreateThread (
        NULL,
        0,
        (LPTHREAD_START_ROUTINE)fn,
        pData,
        0,
        &dwThreadId);

    if (hThread == NULL)
        {
        ret = SD_FAILURE;
        }

    return ret;
    }

void pipeRxloop ()
    {
    char chBuf[MAX_PIPE_BUFF_LEN];
    DWORD cbRead = 0;

    while (doIt)
        {
        pipe->read (chBuf, cbRead);
        if (cbRead)
            {
            }
        MIN_PIPE_SLEEP;
        }
    }

/*Exposed functions*/
int xGetIedLite (char* ip, char* iedName)
    {
    int ret = SD_SUCCESS;

    ret = xAddIed (RESERVED_REMOTE_LITE, ip);
    if (SD_SUCCESS == ret)
        {
        ret = getIedTreeLite (RESERVED_REMOTE_LITE, iedName);
        xRemoveIed (RESERVED_REMOTE_LITE);
        }

    return ret;
    }

int
xGetIedListLocal (char* pSclPath, char* result, int size)
    {
    int ret = SD_SUCCESS;

    ret = xAddIed (RESERVED_LOCAL_SCL, DUMMY_IP);
    if (SD_SUCCESS == ret)
        {
        ret = getIedListLocal (RESERVED_LOCAL_SCL, pSclPath, result, size);
        if ((int)strlen (result) > (size -1))
            {
            pClientLog ("CODE 29", "ERROR", -1);
            }
        xRemoveIed (RESERVED_LOCAL_SCL);
        }

    return ret;
    }

int
xGetIedLocal (char* pXmlPath, char* pSclPath, char* pIedName, char* pApName, int uId)
    {
    int ret = SD_SUCCESS;
    std::map <int, IED*>::iterator ied = gIedList.find (uId);

    if (ied == gIedList.end ())
        {
        ret = SD_FAILURE;
        }
    else
        {
        ret = ied->second->getVmdInfo (pXmlPath, pSclPath, pIedName, pApName);
        }

    pipeWrite ("LOCAL", "OVERALL", "DONE", uId, ret, MAX_PERCENTAGE);

    return ret;
    }

int
xGetIedLocalAsync (char* pXmlPath, char* pSclPath, char* pIedName, char* pApName, int uId)
    {
    int ret = SD_SUCCESS;
    THREAD_DATA_STRUCT* pData = new THREAD_DATA_STRUCT;

    strncpy (pData->pXmlPath, pXmlPath, sizeof (pData->pXmlPath));
    strncpy (pData->pSclPath, pSclPath, sizeof (pData->pSclPath));
    strncpy (pData->pIedName, pIedName, sizeof (pData->pIedName));
    strncpy (pData->pApName, pApName, sizeof (pData->pApName));
    pData->id = uId;

    ret = makeThread ((LPTHREAD_START_ROUTINE)getIedLocalThread, pData);

    return ret;
    }

int
xAddToPollingList (int id, char* domName, char* leaf, int type, int len)
    {
    int ret = SD_SUCCESS;
    std::map <int, IED*>::iterator ied = gIedList.find (id);
    POLL_ARG_STRUCT* leafItem = NULL;

    if (ied == gIedList.end ())
        {
        ret = SD_FAILURE;
        }
    else
        {
        POLL_ARG_STRUCT* pollData = { 0 };

        pollData = (POLL_ARG_STRUCT*)chk_calloc (1, sizeof (POLL_ARG_STRUCT));
        if (pollData == NULL)
            {
            pClientLog ("ALLOC", "ERROR", -1);
            }

        for (leafItem = (POLL_ARG_STRUCT*)list_find_last ((DBL_LNK*)ied->second->leafList->listItem);
             leafItem != NULL;
             leafItem = (POLL_ARG_STRUCT*)list_find_prev ((DBL_LNK*)ied->second->leafList->listItem, (DBL_LNK*)leafItem))
            {
            if ((strcmp (domName, leafItem->domName) == 0)
                && (strcmp (leaf, leafItem->leaf) == 0))
                {
                break;
                }
            }

        if (leafItem == NULL)
            {
            ied->second->leafList->count += 1;

            strncpy (pollData->domName, domName, sizeof (pollData->domName));
            strncpy (pollData->leaf, leaf, sizeof (pollData->leaf));
            pollData->type = type;
            pollData->len = len;

            list_add_last (&ied->second->leafList->listItem, pollData);
            }
        else
            {
            ret = SD_FAILURE;
            }
        }

    return ret;
    }

int
xRemoveFromPollingList (int id, char* domName, char* leaf)
    {
    int ret = SD_SUCCESS;
    std::map <int, IED*>::iterator ied = gIedList.find (id);
    POLL_ARG_STRUCT* leafItem = NULL;

    if (ied == gIedList.end ())
        {
        ret = SD_FAILURE;
        }
    else
        {

        for (leafItem = (POLL_ARG_STRUCT*)list_find_last ((DBL_LNK*)ied->second->leafList->listItem);
             leafItem != NULL;
             leafItem = (POLL_ARG_STRUCT*)list_find_prev ((DBL_LNK*)ied->second->leafList->listItem, (DBL_LNK*)leafItem))
            {
            if ((strcmp (domName, leafItem->domName) == 0)
                && (strcmp (leaf, leafItem->leaf) == 0))
                {
                break;
                }
            }

        if (leafItem == NULL)
            {
            ret = SD_FAILURE;
            }
        else
            {
            list_unlink (&ied->second->leafList, leafItem);
            }
        }

    return ret;
    }

int
xSelect (int id, char* domName, char* leaf)
    {
    int ret = SD_SUCCESS;
    std::map <int, IED*>::iterator ied = gIedList.find (id);

    if (ied == gIedList.end ())
        {
        ret = SD_FAILURE;
        }
    else
        {
        ret = ied->second->select (domName, leaf);
        }

    return ret;
    }

int
xSelectWithValue (int id,
                  char* domName,
                  char* leafOper,
                  char* leafSBOw,
                  char* data,
                  char* addcause,
                  char* orCat,
                  char* interlockCheck,
                  char* synchrocheck,
                  char* test,
                  int timeInSec)
    {
    int ret = SD_SUCCESS;
    std::map <int, IED*>::iterator ied = gIedList.find (id);
    char readData[5] = { 0 };
    char testData[5] = { 0 };

    if (ied == gIedList.end())
    {
        ret = SD_FAILURE;
    }
    else
    {

        if (strstr(leafOper, "LPHD") != 0)
        {
            char* LDbehVal = "LLN0$ST$Beh$stVal";

            ret = ied->second->getDataEx(domName, LDbehVal, readData);

            if (ret != SD_SUCCESS)
                std::cout << "error- unable to read Beh.stVal continuing as non test case" << endl;

            if (strcmp(readData, "3") == 0 || strcmp(readData, "4") == 0)
            {
                strncpy(testData, "1", 1);
            }
            else
            {
                strncpy(testData, "0", 1);
            }
        }
        else
        {
            std::string modleaf = leafOper;
            auto i1 = modleaf.find_first_of("$");
            std::string substr = modleaf.substr(0, i1);
            std::string LDbehVal = substr + "$ST$Beh$stVal";

            ret = ied->second->getDataEx(domName, (char*)LDbehVal.c_str(), readData);

            if (ret != SD_SUCCESS)
                std::cout << "error- unable to read Beh.stVal continuing as non test case" << endl;

            if (strcmp(readData, "3") == 0 || strcmp(readData, "4") == 0)
            {
                strncpy(testData, "1", 1);
            }
            else
            {
                strncpy(testData, "0", 1);
            }
        }

        ret = ied->second->selectWithValue (domName,
                                            leafOper,
                                            leafSBOw,
                                            data,
                                            addcause,
                                            orCat,
                                            interlockCheck,
                                            synchrocheck,
                                            testData,
                                            timeInSec);

    }
        return ret;
    }

int
xOperate (int id,
          char* domName,
          char* leaf,
          char* data,
          char* addcause,
          char* orCat,
          char* interlockCheck,
          char* synchrocheck,
          char* test,
          int timeInSec,
          int ctlModel)
    {
    int ret = SD_SUCCESS;
    std::map <int, IED*>::iterator ied = gIedList.find (id);
    char readData[5] = { 0 }; 
    char testData[5] = { 0 }; 

    if (ied == gIedList.end ())
        {
        ret = SD_FAILURE;
        }
    else
        {

        if (strstr(leaf, "LPHD") != 0)
        {
            char* LDbehVal = "LLN0$ST$Beh$stVal";

            ret = ied->second->getDataEx(domName, LDbehVal, readData);

            if (ret != SD_SUCCESS)
                std::cout << "error- unable to read Beh.stVal continuing as non test case" << endl;

            if (strcmp(readData, "3") == 0 || strcmp(readData, "4") == 0)
            {
                strncpy(testData, "1", 1);
            }
            else
            {
                strncpy(testData, "0", 1);
            }
        }
        else
        {
            std::string modleaf = leaf;
            auto i1 = modleaf.find_first_of("$");
            std::string substr = modleaf.substr(0, i1);
            std::string LDbehVal = substr + "$ST$Beh$stVal";

            ret = ied->second->getDataEx(domName, (char*)LDbehVal.c_str(), readData);

            if (ret != SD_SUCCESS)
                std::cout << "error- unable to read Beh.stVal continuing as non test case" << endl;


            if (strcmp(readData, "3") == 0 || strcmp(readData, "4") == 0)
            {
                strncpy(testData, "1", 1);
            }
            else
            {
                strncpy(testData, "0", 1);
            }

        }

        ret = ied->second->operate (domName,
                                    leaf,
                                    data,
                                    addcause,
                                    orCat,
                                    interlockCheck,
                                    synchrocheck,
                                    testData,
                                    timeInSec,
                                    ctlModel);

        }

    return ret;
    }

int
xSetStructData (int id, char* domName, char* leaf, char* data)
    {
    int ret = SD_SUCCESS;
    std::map <int, IED*>::iterator ied = gIedList.find (id);

    if (ied == gIedList.end ())
        {
        ret = SD_FAILURE;
        }
    else
        {
        ret = ied->second->setStructData (domName, leaf, data);
        }

    return ret;
    }

int
xGetStructData (int id, char* domName, char* leaf, char* data, int allocSize)
    {
    int ret = SD_SUCCESS;

    std::map <int, IED*>::iterator ied = gIedList.find (id);

    if (ied == gIedList.end ())
        {
        ret = SD_FAILURE;
        }
    else
        {
        ret = ied->second->getStructData (domName, leaf, data, allocSize);
        }

    return ret;
    }

int
xGetDataEx (int id, char* domName, char* leaf, char* data)
    {
    int ret = SD_SUCCESS;
    std::map <int, IED*>::iterator ied = gIedList.find (id);

    if (ied == gIedList.end ())
        {
        ret = SD_FAILURE;
        }
    else
        {
        ret = ied->second->getDataEx (domName, leaf, data);
        }

    return ret;
    }

int
xGetData (int id, char* domName, char* leaf, char* data, int type, int len)
    {
    int ret = SD_SUCCESS;
    std::map <int, IED*>::iterator ied = gIedList.find (id);

    if (ied == gIedList.end ())
        {
        ret = SD_FAILURE;
        }
    else
        {
        ret = ied->second->getData (domName, leaf, data, type, len);
        }

    return ret;
    }

int
xSetDataEx (int id, char* domName, char* leaf, char* data, char* error)
    {
    int ret = SD_SUCCESS;
    std::map <int, IED*>::iterator ied = gIedList.find (id);

    if (ied == gIedList.end ())
        {
        ret = SD_FAILURE;
        }
    else
        {
        ret = ied->second->setDataEx (domName, leaf, data, error);
        }

    return ret;
    }

int
xSetData (int id, char* domName, char* leaf, char* data, int type, int len)
    {
    int ret = SD_SUCCESS;
    std::map <int, IED*>::iterator ied = gIedList.find (id);

    if (ied == gIedList.end ())
        {
        ret = SD_FAILURE;
        }
    else
        {
        ret = ied->second->setData (domName, leaf, data, type, len);
        }

    return ret;
    }

int
xGetIed (int id, int keepConnected, char* pXmlPath)
    {
    int ret = SD_SUCCESS;
    std::map <int, IED*>::iterator ied = gIedList.find (id);

    if (ied == gIedList.end ())
        {
        ret = SD_FAILURE;
        }
    else
        {
        ret = ied->second->getRemoteDataModel (keepConnected, pXmlPath);
        }

    pipeWrite ("REMOTE", "OVERALL", "DONE", id, ret, MAX_PERCENTAGE);

    return ret;
    }

int
xGetIedAsync (int id, int keepConnected, char* pXmlPath)
    {
    int ret = SD_SUCCESS;
    THREAD_DATA_STRUCT* pData = new THREAD_DATA_STRUCT;

    strncpy (pData->pXmlPath, pXmlPath, sizeof (pData->pXmlPath));
    pData->id = id;
    pData->keepConnected = keepConnected;

    ret = makeThread ((LPTHREAD_START_ROUTINE)getIedThread, pData);

    return ret;
    }

int
xChangeIp (int id, char* newIp)
    {
    int ret = SD_SUCCESS;
    std::map <int, IED*>::iterator pIed = gIedList.find (id);

    if (pIed == gIedList.end ())
        {
        ret = SD_FAILURE;
        }
    else
        {

        if (pIed->second->associationStatus == STATUS_CONNECTED)
            {
            pIed->second->iedDisconnect ();
            }

        if (strcmp (pIed->second->ip, newIp) != 0)
            {
            strncpy (pIed->second->ip, newIp, sizeof (pIed->second->ip));
            pIed->second->acsiAssociation->removeFromDib (pIed->second->sId);
            pIed->second->acsiAssociation->addToDib (pIed->second->sId, pIed->second->ip);
            }
        }

    return ret;
    }

int
xAddIed (int id, char* ip)
    {
    int ret = SD_SUCCESS;
    IED* pIed = NULL;
    std::map <int, IED*>::iterator i = gIedList.find (id);

    if (i == gIedList.end ())
        {
        pIed = new IED (id, ip);

        if (pIed == NULL)
            {
            ret = SD_FAILURE;
            }
        else
            {
            ret = pIed->acsiAssociation->addToDib (pIed->sId, pIed->ip);
            if (SD_SUCCESS == ret)
                {
                gIedList.insert (std::pair <int, IED*> (id, pIed));
                }
            }
        }

    return ret;
    }

int
xRemoveIed (int id)
    {
    int ret = SD_SUCCESS;
    std::map <int, IED*>::iterator pIed = gIedList.find (id);

    if (pIed == gIedList.end ())
        {
        ret = SD_FAILURE;
        }
    else
        {
        if (pIed->second->associationStatus == STATUS_CONNECTED)
            {
            pIed->second->iedDisconnect ();
            }

        pIed->second->acsiAssociation->removeFromDib (pIed->second->sId);
        gIedList.erase (id);
        }

    return ret;
    }

int
xDllInit ()
    {
    int ret = SD_FAILURE;

    /*even if pipe fails let the user proceed
    It only means pipe based functions dont work*/
    pipe = new nPipe ();

    do
        {
        doIt = SD_TRUE;

#if 0
        if (SD_SUCCESS != makeThread ((LPTHREAD_START_ROUTINE)pipeRxloop, 0))
            {
            break;
            }

        if (SD_SUCCESS != makeThread ((LPTHREAD_START_ROUTINE)pollReqLoop, 0))
            {
            break;
            }

        if (SD_SUCCESS != makeThread ((LPTHREAD_START_ROUTINE)pollResLoop, 0))
            {
            break;
            }
#endif

        if (SD_SUCCESS != init ())
            {
            break;
            }

        if (SD_SUCCESS != makeThread ((LPTHREAD_START_ROUTINE)doComLoop, 0))
            {
            break;
            }

        ret = SD_SUCCESS;
        } while (0);

        if (SD_SUCCESS != ret)
            {
            doIt = SD_FALSE;
            }

        return ret;
    }

int
xDllPost ()
    {
    doIt = SD_FALSE;
    return post ();
    }

int
xConnect (int id)
    {
    int ret = SD_SUCCESS;
    std::map <int, IED*>::iterator pIed = gIedList.find (id);

    if (pIed == gIedList.end ())
        {
        ret = SD_FAILURE;
        }
    else
        {
        ret = pIed->second->iedConnect ();
        }

    return ret;
    }

int
xDisconnect (int id)
    {
    int ret = SD_SUCCESS;
    std::map <int, IED*>::iterator pIed = gIedList.find (id);

    if (pIed == gIedList.end ())
        {
        ret = SD_FAILURE;
        }
    else
        {
        pIed->second->iedDisconnect ();
        }

    return ret;
    }

/*Test code*/

void
main ()
    {
#if 0
    int ret = SD_SUCCESS;
    char* addcause = (char*)chk_calloc (1, 128);
#endif
    char fileName[1024] = { 0 };
    char buff[50000] = { 0 };
    char* result = (char*)buff; // chk_calloc (1, 50000);
    int sclCnt = 0;
    int iedCnt = 0;
    int iedPass = 0;
    int iedFail = 0;
    int sclPass = 0;
    int sclFail = 0;
    DIR* dir;
    struct dirent* ent;
    FILE* FP = fopen ("test.txt", "w");
    FILE* FP2 = fopen ("test2.txt", "w");
    FILE* out = fopen ("out.txt", "w");

    if (FP && FP2 && out)
        {
        if (xDllInit() == SD_SUCCESS)
            {
            if ((dir = opendir ("E:\\SCL6")) != NULL)
                {
                struct timeb timestart = { 0 };
                struct timeb timestop = { 0 };
                ftime (&timestart);
                while ((ent = readdir (dir)) != NULL)
                    {
                    result = (char*)buff;
                    sclCnt++;

                    sprintf (fileName, "E:\\SCL6\\%s", ent->d_name);
                    fprintf (FP, "%s\n", fileName);

                    fflush (FP);
                    if (SD_SUCCESS == xGetIedListLocal (fileName, result, 50000))
                        {
                        char* str = NULL;
                        str = get_next_string (&result, "|");
                        while (str)
                            {
                            char* ied = get_next_string (&str, ",");
                            char* ap = get_next_string (&str, ",");
                            iedCnt++;
                            
                            fprintf (FP2, "%s\t%s\n", ied, ap);
                            fflush (FP2);

                            xAddIed (13, "172.16.20.98");
                            if (SD_SUCCESS == xGetIedLocal ("test.xml", fileName, ied, ap, 13))
                                {
                                iedPass++;
                                fprintf (FP2, "iedPass\n");
                                }
                            else
                                {
                                iedFail++;
                                fprintf (FP2, "iedFail\n");
                                }
                            xRemoveIed (13);

                            str = get_next_string (&result, "|");
                            }
                        sclPass++;
                        fprintf (FP, "sclPass\n");
                        }
                    else
                        {
                        sclFail++;
                        fprintf (FP, "sclFail\n");
                        }
                    }
                ftime (&timestop);
                int diffs = (int)(timestop.time - timestart.time);

                fprintf (out, "sclCnt = %d iedCnt = %d iedPass = %d iedFail = %d sclPass = %d sclFail = %d time = %d", 
                         sclCnt, iedCnt, iedPass, iedFail, sclPass, sclFail, diffs);
                }
            xDllPost ();
            }
        fclose (FP);
        fclose (FP2);
        fclose (out);
        }
    }

void
listClear (STRUCT_LEAF* pListItem)
    {
    STRUCT_LEAF* leaf = NULL;

    while ((leaf = pListItem) != NULL)
        {
        list_unlink (&pListItem, leaf);
        chk_free (leaf->pName);
        chk_free (leaf);
        }
    }
