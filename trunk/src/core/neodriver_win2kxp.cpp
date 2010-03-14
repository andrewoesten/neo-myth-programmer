// #########################################################################

#include <neodriver.h>

#ifdef neodriver_win2kxp_g
// #########################################################################

#include <setupapi.h>

// #########################################################################

neodriver_win2kxp::neodriver_win2kxp()
{
    state=state_undefined;
}

// #########################################################################

neodriver_win2kxp::~neodriver_win2kxp()
{
    close();
}

// #########################################################################

int neodriver_win2kxp::open()
{
    close();
    state=state_loading;
    rPipe=INVALID_HANDLE_VALUE;
    wPipe=INVALID_HANDLE_VALUE;
    wEvent=INVALID_HANDLE_VALUE;
    GUID guid;
    memcpy(&guid,"\xFB\xD4\x26\xF8\x52\x51\xB0\x40\xB5\x46\xA6\xED\xF2\x45\x2F\x05",16);
    HDEVINFO ncart=SetupDiGetClassDevs(&guid,0,0,DIGCF_DEVICEINTERFACE|DIGCF_PRESENT);
    if(ncart!=INVALID_HANDLE_VALUE)
    {
        SP_DEVICE_INTERFACE_DATA DeviceInterfaceData;
        DeviceInterfaceData.cbSize=sizeof(SP_DEVICE_INTERFACE_DATA);
        if(SetupDiEnumDeviceInterfaces(ncart,0,&guid,0,&DeviceInterfaceData))
        {
            DWORD RequiredSize;
            SetupDiGetDeviceInterfaceDetail(ncart,&DeviceInterfaceData,0,0,&RequiredSize,0);
            if(RequiredSize<0x400)
            {
                char data[0x400];
                SP_DEVICE_INTERFACE_DETAIL_DATA*DeviceInterfaceDetailData=(SP_DEVICE_INTERFACE_DETAIL_DATA*)data;
                DeviceInterfaceDetailData->cbSize=sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
                if(SetupDiGetDeviceInterfaceDetail(ncart,&DeviceInterfaceData,DeviceInterfaceDetailData,RequiredSize,&RequiredSize,0))
                {
                    TCHAR devname[0x400];
                    lstrcpy(devname,DeviceInterfaceDetailData->DevicePath);
                    lstrcat(devname,TEXT("\\PIPE0"));
                    rPipe=CreateFile(devname,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
                    if(rPipe!=INVALID_HANDLE_VALUE)
                    {
                        lstrcpy(devname,DeviceInterfaceDetailData->DevicePath);
                        lstrcat(devname,TEXT("\\PIPE1"));
                        wPipe=CreateFile(devname,GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,0,OPEN_EXISTING,FILE_FLAG_OVERLAPPED,0);
                        if(wPipe!=INVALID_HANDLE_VALUE)
                        {
                            wEvent=CreateEvent(0,1,1,TEXT(""));
                            if(wEvent!=INVALID_HANDLE_VALUE)
                            {
                                state=state_ready;
                            }
                        }
                    }
                }
            }
        }
        SetupDiDestroyDeviceInfoList(ncart);
    }
    if(state!=state_ready)
    {
        close();
    }
    return state;
}

// #########################################################################

int neodriver_win2kxp::isConnected()
{
    int res=0;
    GUID guid;
    memcpy(&guid,"\xFB\xD4\x26\xF8\x52\x51\xB0\x40\xB5\x46\xA6\xED\xF2\x45\x2F\x05",16);
    HDEVINFO ncart=SetupDiGetClassDevs(&guid,0,0,DIGCF_DEVICEINTERFACE|DIGCF_PRESENT);
    if(ncart!=INVALID_HANDLE_VALUE)
    {
        SP_DEVICE_INTERFACE_DATA DeviceInterfaceData;
        DeviceInterfaceData.cbSize=sizeof(SP_DEVICE_INTERFACE_DATA);
        if(SetupDiEnumDeviceInterfaces(ncart,0,&guid,0,&DeviceInterfaceData))
        {
            DWORD RequiredSize;
            SetupDiGetDeviceInterfaceDetail(ncart,&DeviceInterfaceData,0,0,&RequiredSize,0);
            if(RequiredSize<0x400)
            {
                char data[0x400];
                SP_DEVICE_INTERFACE_DETAIL_DATA*DeviceInterfaceDetailData=(SP_DEVICE_INTERFACE_DETAIL_DATA*)data;
                DeviceInterfaceDetailData->cbSize=sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
                if(SetupDiGetDeviceInterfaceDetail(ncart,&DeviceInterfaceData,DeviceInterfaceDetailData,RequiredSize,&RequiredSize,0))
                {
                    res=1;
                }
            }
        }
        SetupDiDestroyDeviceInfoList(ncart);
    }
    return res;
}

// #########################################################################

int neodriver_win2kxp::deviceio(void*lpInBuffer,DWORD nInBufferSize,void*lpOutBuffer,DWORD nOutBufferSize)
{
    if(state!=state_ready)return 0;
    DWORD bytesReturned=0;
    if(DeviceIoControl(wPipe,0x22002a,lpInBuffer,nInBufferSize,lpOutBuffer,nOutBufferSize,&bytesReturned,0))
    {
        if(bytesReturned==nOutBufferSize)
        {
            return 1;
        }
    }
    return 0;
}

// #########################################################################

int neodriver_win2kxp::read(void*lpOutBuffer,DWORD nOutBufferSize)
{
    if(state!=state_ready)return 0;
    DWORD bytesReturned=0;
    if(ReadFile(rPipe,lpOutBuffer,nOutBufferSize,&bytesReturned,0))
    {
        if(bytesReturned==nOutBufferSize)
        {
            return 1;
        }
    }
    return 0;
}

// #########################################################################

int neodriver_win2kxp::write(const void*lpInBuffer,DWORD nInBufferSize)
{
    if(state!=state_ready)return 0;
    OVERLAPPED ovp;
    memset(&ovp,0,sizeof(ovp));
    ovp.hEvent=wEvent;
    WriteFile(wPipe,lpInBuffer,nInBufferSize,0,&ovp);
    DWORD wr=WaitForSingleObject(wEvent,neo_timeout);
    if(wr==WAIT_OBJECT_0)
    {
        return 1;
    }
    if(wr==WAIT_TIMEOUT)
    {
        //CancelIo(wPipe);
    }
    ResetEvent(wEvent);
    return 0;
}

// #########################################################################

void neodriver_win2kxp::close()
{
    if(state==state_undefined)return;
    if(rPipe!=INVALID_HANDLE_VALUE)
    {
        CloseHandle(rPipe);
    }
    if(wPipe!=INVALID_HANDLE_VALUE)
    {
        CloseHandle(wPipe);
    }
    if(wEvent!=INVALID_HANDLE_VALUE)
    {
        CloseHandle(wEvent);
    }
    state=state_undefined;
}

// #########################################################################
#endif
