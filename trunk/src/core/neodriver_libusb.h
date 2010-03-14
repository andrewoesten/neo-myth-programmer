#ifndef neodriver_libusb_g
#define neodriver_libusb_g
// #########################################################################

#include <neodriver.h>
#include <usb.h>

// #########################################################################

class neodriver_libusb:public neodriver
{
    DWORD state;
    struct usb_dev_handle*usb_hndl;
    int iFace;
    int rPipe;
    int wPipe;
public:
    neodriver_libusb();
    virtual ~neodriver_libusb();
    virtual int open();
    virtual int isConnected();
    virtual int deviceio(void*lpInBuffer,DWORD nInBufferSize,void*lpOutBuffer,DWORD nOutBufferSize);
    virtual int read(void*lpOutBuffer,DWORD nOutBufferSize);
    virtual int write(const void*lpInBuffer,DWORD nInBufferSize);
    virtual void close();
};

// #########################################################################
#endif
