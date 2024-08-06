/*******************************************************************************
File      : xml.hpp
By        : Arun Lal K M 
Date      : 27th Jun 2019 
*******************************************************************************/
#ifndef xml_h
#define xml_h

/*include*/
#include "glbtypes.h"
#include "sysincs.h"
#include <mvl_defs.h>
#include <mvl_acse.h>
#include <iostream>

/*#define*/
#define DllExport __declspec(dllexport)
#define _ASSERT_(x) assert(x)

/*extern "C"*/
extern "C" void pClientLog (char* text, char* caption, int code);

/*enum*/

/*class*/
class XML
    {
    public:
        XML ();
        ~XML ();
        int xmlPrintNode (FILE* pXmlFp, char* nodeName, int type, int level);
        int xmlPrint (FILE* pXmlFp, char* name, int type, char* ref, char* nodeName, int level);
        int xmlPrintElement (FILE* pXmlFp, char* name, char* nodeName, int level);
        int xmlPrintElementEx (FILE* pXmlFp, char* name, char* nodeName, int level, char* data);
        int xmlPrintLeaf (FILE* pXmlFp, char* name, int tag, int len, char* ref, char* nodeName, int level);
    private:
    };

#endif