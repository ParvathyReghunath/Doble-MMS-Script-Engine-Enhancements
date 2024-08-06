/*******************************************************************************
File      : ied.hpp
By        : Arun Lal K M 
Date      : 27th Jun 2019 
*******************************************************************************/
#ifndef pipe_h
#define pipe_h

/*include*/
#include "glbtypes.h"
#include "sysincs.h"
#include <mvl_defs.h>
#include <mvl_acse.h>
#include <iostream>

/*#define*/
#define _ASSERT_(x) assert(x)
#define MAX_PIPE_BUFF_LEN 256

/*class*/
class nPipe
    {
    public:
        nPipe ();
        ~nPipe ();

        /*functions*/
        int read (char* chBuf, DWORD cbRead);
        int write (char* buf);

        /*variables*/
        int finished;
        HANDLE hPipe1;
        HANDLE hPipe2; 
    private:
    };

#endif