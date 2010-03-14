#ifdef WIN32
#ifndef neodriver_win2kxp_g
#define neodriver_win2kxp_g
// #########################################################################

#include <neodriver.h>

// #########################################################################

class neodriver_win2kxp:public neodriver
{
    DWORD state;
    HANDLE rPipe;
    HANDLE wPipe;
    HANDLE wEvent;
public:
    neodriver_win2kxp();
    virtual ~neodriver_win2kxp();
    virtual int open();
    virtual int isConnected();
    virtual int deviceio(void*lpInBuffer,DWORD nInBufferSize,void*lpOutBuffer,DWORD nOutBufferSize);
    virtual int read(void*lpOutBuffer,DWORD nOutBufferSize);
    virtual int write(const void*lpInBuffer,DWORD nInBufferSize);
    virtual void close();
};

// #########################################################################
#endif
#endif
