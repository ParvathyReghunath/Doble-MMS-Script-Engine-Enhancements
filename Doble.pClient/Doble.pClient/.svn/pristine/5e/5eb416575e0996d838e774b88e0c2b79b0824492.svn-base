/*******************************************************************************
File      : ied.cpp
By        : Arun Lal K M 
Date      : 27th Jun 2019 
*******************************************************************************/

/*include*/
#include "xml.hpp"

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

XML::XML ()
    {
    }

XML::~XML ()
    {
    }

int XML::xmlPrintLeaf (FILE* pXmlFp, char* name, int tag, int len, char* ref, char* nodeName, int level)
    {
    int ret = SD_SUCCESS;

    if (pXmlFp)
        {
        if (tag == RT_STR_START || 
            tag == RT_BOOL || 
            tag == RT_BIT_STRING  || 
            tag == RT_INTEGER  || 
            tag == RT_UNSIGNED  || 
            tag == RT_FLOATING_POINT || 
            tag == RT_OCTET_STRING || 
            tag == RT_VISIBLE_STRING || 
            tag == RT_GENERAL_TIME || 
            tag == RT_BINARY_TIME || 
            tag == RT_BCD || 
            tag == RT_BOOLEANARRAY || 
            tag == RT_UTC_TIME || 
            tag == RT_UTF8_STRING)
            {
            for (int i = 0; i < level; i++)
                {
                fprintf (pXmlFp, "\t");
                }

            fprintf (pXmlFp, "<%s Name=\"%s\" TYPE=\"%d\" LEN=\"%d\" Ref=\"%s\" />\n", 
                nodeName, name, tag, len, ref);
            fflush (pXmlFp);
            }
        else
            {
            pClientLog ("CODE 27", "ERROR", -1);
            }
        }
    else 
        {
        ret = SD_FAILURE;
        }

    return ret;
    }

int XML::xmlPrintElementEx (FILE* pXmlFp, char* name, char* nodeName, int level, char* data)
    {
    int ret = SD_SUCCESS;

    if (pXmlFp)
        {
        for (int i = 0; i < level; i++)
            {
            fprintf (pXmlFp, "\t");
            }
        fprintf (pXmlFp, "<%s Name=\"%s\" DATA=\"%s\" />\n", 
            nodeName, name, data);
        }
    else
        {
        ret = SD_FAILURE;
        }

    return ret;
    }

int XML::xmlPrintElement (FILE* pXmlFp, char* name, char* nodeName, int level)
    {
    int ret = SD_SUCCESS;

    if (pXmlFp)
        {
        for (int i = 0; i < level; i++)
            {
            fprintf (pXmlFp, "\t");
            }
        fprintf (pXmlFp, "<%s Name=\"%s\" />\n", 
            nodeName, name);
        }
    else
        {
        ret = SD_FAILURE;
        }

    return ret;
    }

int XML::xmlPrintNode (FILE* pXmlFp, char* nodeName, int type, int level)
    {
    int ret = SD_SUCCESS;

    if (pXmlFp)
        {
        for (int i = 0; i < level; i++)
            {
            fprintf (pXmlFp, "\t");
            }
        if (RT_STR_START == type)
            {
            fprintf (pXmlFp, "<%s >\n", nodeName);
            }
        else if (RT_STR_END == type)
            {
            fprintf (pXmlFp, "</%s>\n", nodeName);
            }
        }

    return ret;
    }

int XML::xmlPrint (FILE* pXmlFp, char* name, int tag, char* ref, char* nodeName, int level)
    {
    int ret = SD_SUCCESS;

    if (pXmlFp)
        {
        for (int i = 0; i < level; i++)
            {
            fprintf (pXmlFp, "\t");
            }
        if (RT_STR_END == tag)
            {
            fprintf (pXmlFp, "</%s>\n", nodeName);
            fflush (pXmlFp);
            }
        else if (tag == RT_STR_START || 
            tag == RT_ARR_START || 
            tag == RT_BOOL || 
            tag == RT_BIT_STRING  || 
            tag == RT_INTEGER  || 
            tag == RT_UNSIGNED  || 
            tag == RT_FLOATING_POINT || 
            tag == RT_OCTET_STRING || 
            tag == RT_VISIBLE_STRING || 
            tag == RT_GENERAL_TIME || 
            tag == RT_BINARY_TIME || 
            tag == RT_BCD || 
            tag == RT_BOOLEANARRAY || 
            tag == RT_UTC_TIME || 
            tag == RT_UTF8_STRING)
            {
            fprintf (pXmlFp, "<%s Name=\"%s\" TYPE=\"%d\" Ref=\"%s\" >\n", 
                nodeName, name, tag, ref);
            fflush (pXmlFp);
            }
        else
            {
            pClientLog ("CODE 28", "ERROR", -1);
            }
        }
    else 
        {
        ret = SD_FAILURE;
        }

    return ret;
    }
