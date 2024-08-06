/*******************************************************************************
File      : ied.cpp
By        : Arun Lal K M 
Date      : 27th Jun 2019 
*******************************************************************************/

/*#include*/
#include "pipe.hpp"

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

nPipe::nPipe ()
    {
    LPTSTR lpszPipename1 = TEXT("\\\\.\\pipe\\myNamedPipe1");
    LPTSTR lpszPipename2 = TEXT("\\\\.\\pipe\\myNamedPipe2");

    hPipe1=CreateFile (lpszPipename1, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
    hPipe2=CreateFile (lpszPipename2, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);

    if ((hPipe1 == NULL || hPipe1 == INVALID_HANDLE_VALUE)||
        (hPipe2 == NULL || hPipe2 == INVALID_HANDLE_VALUE))
        { 
        finished = 0;
        }
    else
        {
        finished = 1;
        }
    }

nPipe::~nPipe ()
    {
    if (hPipe1)
        {
        CloseHandle (hPipe1);
        }

    if (hPipe1)
        {
        CloseHandle (hPipe2);
        }
    }

int nPipe::read (char* chBuf, DWORD cbRead)
    {
    int ret = SD_FAILURE;
    BOOL fSuccess; 

    fSuccess = ReadFile (hPipe2, chBuf, MAX_PIPE_BUFF_LEN, &cbRead, NULL); 
    if(fSuccess)
        {
        ret = SD_SUCCESS;
        }

    return ret;
    }

int nPipe::write (char* buf)
    {
    int ret = SD_SUCCESS;
    DWORD cbWritten;

    WriteFile (hPipe1, buf, strlen(buf)+1, &cbWritten, NULL);

    return ret;
    }
