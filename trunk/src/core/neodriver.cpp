// #########################################################################

#include <neodriver.h>

#undef neodriver_win2kxp_g

// #########################################################################

neodriver*getdriver()
{
    neodriver*neodrv=0;
#ifdef neodriver_libusb_g
    if(neodrv==0)
    {
        neodrv=new neodriver_libusb;
        if(!neodrv->open())
        {
            neodrv->close();
            delete neodrv;
            neodrv=0;
        }
    }
#endif
#ifdef neodriver_win2kxp_g
    if(neodrv==0)
    {
        neodrv=new neodriver_win2kxp;
        if(!neodrv->open())
        {
            neodrv->close();
            delete neodrv;
            neodrv=0;
        }
    }
#endif
    return neodrv;
}

// #########################################################################
