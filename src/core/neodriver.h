#ifndef neodriver_g
#define neodriver_g
// #########################################################################

#include <neoutil.h>

// #########################################################################

#define neo_timeout (5*1000)
#define neo_block_size (64*KB)

// #########################################################################

class neodriver
{
public:
    virtual ~neodriver(){};
    virtual int open()=0;
    virtual int isConnected()=0;
    virtual int deviceio(void*lpInBuffer,DWORD nInBufferSize,void*lpOutBuffer,DWORD nOutBufferSize)=0;
    virtual int read(void*lpOutBuffer,DWORD nOutBufferSize)=0;
    virtual int write(const void*lpInBuffer,DWORD nInBufferSize)=0;
    virtual void close()=0;
};

// #########################################################################

#include <neodriver_win2kxp.h>
#include <neodriver_libusb.h>

// #########################################################################

neodriver*getdriver();

// #########################################################################
#endif
