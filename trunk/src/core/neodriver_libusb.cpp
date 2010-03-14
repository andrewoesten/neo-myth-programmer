// #########################################################################

#include <neodriver.h>

#ifdef neodriver_libusb_g
// #########################################################################

#include <usb.h>

// #########################################################################

int translate_usb_message(void*lpInBuffer,DWORD nInBufferSize,int&a,int&b,int&c,int&d)
{
    BYTE*code=(BYTE*)lpInBuffer;
    a=b=c=d=0;
    a=code[3];
    b=code[4];
    c=code[6];
    d=code[8];
    return 0;
}

// #########################################################################

neodriver_libusb::neodriver_libusb()
{
    state=state_undefined;
    usb_hndl=0;
}

// #########################################################################

neodriver_libusb::~neodriver_libusb()
{
    close();
}

// #########################################################################

int neodriver_libusb::open()
{
    close();
    state=state_loading;
    usb_init();
    usb_find_busses();
    usb_find_devices();
    struct usb_device*usb_dev=0;
    struct usb_bus*busses=usb_get_busses();
    for(struct usb_bus*bus=busses;bus;bus=bus->next)
    {
        for(struct usb_device*dev=bus->devices;dev;dev=dev->next)
        {
            if(dev->descriptor.idVendor==0xffab&&dev->descriptor.idProduct==0xdd03)
            {
                usb_dev=dev;
                break;
            }
        }
    }
    if(!usb_dev)
    {
        return 0;
    }
    usb_hndl=usb_open(usb_dev);
    if(!usb_hndl)
    {
        return 0;
    }
    if(usb_set_configuration(usb_hndl,usb_dev->config[0].bConfigurationValue)<0)
    {
        return 0;
    }
    iFace=usb_dev->config[0].interface[0].altsetting[0].bInterfaceNumber;
    if(usb_claim_interface(usb_hndl,iFace)<0)
    {
        return 0;
    }
    //usb_set_altinterface(usb_hndl ,usb_dev->config[0].interface[0].altsetting[0].bAlternateSetting);
    rPipe=usb_dev->config[0].interface[0].altsetting[0].endpoint[0].bEndpointAddress;
    wPipe=usb_dev->config[0].interface[0].altsetting[0].endpoint[1].bEndpointAddress;
    state=state_ready;
    return state;
}

// #########################################################################

int neodriver_libusb::isConnected()
{
    usb_init();
    usb_find_busses();
    usb_find_devices();
    struct usb_device*usb_dev=0;
    struct usb_bus*busses=usb_get_busses();
    for(struct usb_bus*bus=busses;bus;bus=bus->next)
    {
        for(struct usb_device*dev=bus->devices;dev;dev=dev->next)
        {
            if(dev->descriptor.idVendor==0xffab&&dev->descriptor.idProduct==0xdd03)
            {
                usb_dev=dev;
                break;
            }
        }
    }
    return usb_dev!=0;
}

// #########################################################################

int neodriver_libusb::deviceio(void*lpInBuffer,DWORD nInBufferSize,void*lpOutBuffer,DWORD nOutBufferSize)
{
    if(state!=state_ready)return 0;
    int a,b,c,d;
    translate_usb_message(lpInBuffer,nInBufferSize,a,b,c,d);
    int bytesReturned=usb_control_msg(usb_hndl,a,b,c,d,(char*)lpOutBuffer,nOutBufferSize,neo_timeout);
    if(bytesReturned>=0)
    {
        return 1;
    }
    if(bytesReturned==(int)nOutBufferSize)
    {
        return 1;
    }
    return 0;
}

// #########################################################################

int neodriver_libusb::read(void*lpOutBuffer,DWORD nOutBufferSize)
{
    if(state!=state_ready)return 0;
    if(usb_bulk_read(usb_hndl,rPipe,(char*)lpOutBuffer,nOutBufferSize,neo_timeout)==(int)nOutBufferSize)
    {
        return 1;
    }
    return 0;
}

// #########################################################################

int neodriver_libusb::write(const void*lpInBuffer,DWORD nInBufferSize)
{
    if(state!=state_ready)return 0;
    if(usb_bulk_write(usb_hndl,wPipe,(char*)lpInBuffer,nInBufferSize,neo_timeout)==(int)nInBufferSize)
    {
        return 1;
    }
    return 0;
}

// #########################################################################

void neodriver_libusb::close()
{
    if(state==state_undefined)return;
    if(usb_hndl)
    {
        usb_release_interface(usb_hndl,iFace);
        usb_close(usb_hndl);
        usb_hndl=0;
    }
    state=state_undefined;
}

// #########################################################################
#endif
