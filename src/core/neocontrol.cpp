// #########################################################################

#include <neocontrol.h>

// #########################################################################

#define check_read(x) if((x)==0){printf("\n   ***   error: %s failed in line %i of %s\n",_2str(x),__LINE__,__FILE__); check(cancelio(0)); return 0;}
#define check_write(x) if((x)==0){printf("\n   ***   error: %s failed in line %i of %s\n",_2str(x),__LINE__,__FILE__); check(cancelio(1)); return 0;}

// #########################################################################

int setFlashBankRead(membuf&ocmd,const flash_info*finfo,DWORD bank)
{
    if(bank<4)
    {
        if(ocmd.resize(finfo->write2select_flash.size))
        {
            memcpy(ocmd,finfo->write2select_flash.code,finfo->write2select_flash.size);
            BYTE*ucmd=(BYTE*)(char*)ocmd;
            for(DWORD i=0;i<finfo->write2select_flash.size;i++)
            {
                if(ucmd[i]==0xc4)
                {
                    ucmd[i+1]=_2byte(bank*4);
                    return 1;
                }
            }
        }
    }
    return 0;
}

// #########################################################################

int setFlashBankWrite(membuf&ocmd,const flash_info*finfo,DWORD bank)
{
    if(bank<4)
    {
        if(ocmd.resize(finfo->write2write_flash.size))
        {
            memcpy(ocmd,finfo->write2write_flash.code,finfo->write2write_flash.size);
            BYTE*ucmd=(BYTE*)(char*)ocmd;
            if(finfo->nDies==4)
            {
                if(bank==0)ucmd[1]=0xC2;
                if(bank==1)ucmd[1]=0xC0;
                if(bank==2)ucmd[1]=0xC0;
                if(bank==3)ucmd[1]=0xC4;
            }
            else
            {
                if(finfo->nSelect_low==1)
                {
                    ucmd[1]=0xC2;
                }
                else
                {
                    if(finfo->nSelect_high==1)
                    {
                        ucmd[1]=0xC4;
                    }
                    else
                    {
                        ucmd[1]=0xC0;
                    }
                }
            }
            return 1;
        }
    }
    return 0;
}

// #########################################################################

int setSramBank(membuf&ocmd,DWORD bank)
{
    if(bank<7)
    {
        if(ocmd.resize(neocontrolcodes[10].size))
        {
            memcpy(ocmd,neocontrolcodes[10].code,neocontrolcodes[10].size-1);
            BYTE*ucmd=(BYTE*)(char*)ocmd;
            ucmd[neocontrolcodes[10].size-1]=_2byte(bank*8);
            return 1;
        }
    }
    return 0;
}

// #########################################################################

neocontrol::neocontrol()
{
    state=state_undefined;
    prg=0;
}

// #########################################################################

neocontrol::~neocontrol()
{
    close();
}

// #########################################################################

int neocontrol::sync0()
{
    const DWORD sdelay=10;
    const DWORD maxloops=neo_timeout/sdelay;
    memcpy(inbuffer,neocontrolcodes[0].code,neocontrolcodes[0].size);
    memset(outbuffer,0,64);
    for(DWORD i=0;i<maxloops;i++)
    {
        Sleep(sdelay);
        if(neodrv->deviceio(inbuffer,neocontrolcodes[0].size,outbuffer,0x08))
        {
            if(outbuffer[0]&0x8)
            {
                break;
            }
            if(!(outbuffer[0]&0x4))
            {
                return 1;
            }
            continue;
        }
        break;
    }
    return 0;
}

// #########################################################################

int neocontrol::sync1()
{
    const DWORD sdelay=10;
    const DWORD maxloops=neo_timeout/sdelay;
    memcpy(inbuffer,neocontrolcodes[0].code,neocontrolcodes[0].size);
    memset(outbuffer,0,64);
    for(DWORD i=0;i<maxloops;i++)
    {
        Sleep(sdelay);
        if(neodrv->deviceio(inbuffer,neocontrolcodes[0].size,outbuffer,0x08))
        {
            if(outbuffer[0]&0x8)
            {
                break;
            }
            if(!((~outbuffer[0])&0x4))
            {
                return 1;
            }
            continue;
        }
        break;
    }
    return 0;
}

// #########################################################################

int neocontrol::w_cmd0(const void*cmd,DWORD size)
{
    check(sync0());
    memcpy(inbuffer,neocontrolcodes[1].code,neocontrolcodes[1].size);
    store_dword_le(size,outbuffer);
    check(neodrv->deviceio(inbuffer,neocontrolcodes[1].size,outbuffer,4));
    check(sync1());
    check_write(neodrv->write(cmd,size));
    return 1;
}

// #########################################################################

int neocontrol::w_cmd1(const void*cmd,DWORD size)
{
    check(sync0());
    memcpy(inbuffer,neocontrolcodes[21].code,neocontrolcodes[21].size);
    store_dword_le(size,outbuffer);
    check(neodrv->deviceio(inbuffer,neocontrolcodes[21].size,outbuffer,4));
    check(sync1());
    check_write(neodrv->write(cmd,size));
    return 1;
}

// #########################################################################

int neocontrol::r_res0(void*outp,DWORD size)
{
    if(!prg->doWork(0))return 0;
    check(sync0());
    memcpy(inbuffer,neocontrolcodes[2].code,neocontrolcodes[2].size);
    store_dword_le(size,outbuffer);
    check(neodrv->deviceio(inbuffer,neocontrolcodes[2].size,outbuffer,4));
    check(sync1());
    check_read(neodrv->read(outp,size));
    prg->doWork(size);
    return 1;
}

// #########################################################################

int neocontrol::r_res1(void*outp,DWORD a,DWORD offset,DWORD size)
{
    if(!prg->doWork(0))return 0;
    check(sync0());
    memcpy(inbuffer,neocontrolcodes[3].code,neocontrolcodes[3].size);
    DWORD b=offset/2;
    DWORD c=size/4;
    store_dword_le(a,outbuffer+0);
    store_dword_le(b,outbuffer+4);
    store_dword_le(c,outbuffer+8);
    check(neodrv->deviceio(inbuffer,neocontrolcodes[3].size,outbuffer,0x0c));
    check(sync1());
    DWORD tr;
    char*outbuff=(char*)outp;
    while(size)
    {
        tr=Min(size,neo_block_size);
        check_read(neodrv->read(outbuff,tr));
        size-=tr;
        outbuff+=tr;
        if(size&&(!prg->doWork(tr)))
        {
            cancelio(0);
            return 0;
        }
    }
    return 1;
}

// #########################################################################

int neocontrol::r_res2(void*outp)
{
    if(!prg->doWork(0))return 0;
    check(sync0());
    memcpy(inbuffer,neocontrolcodes[18].code,neocontrolcodes[18].size);
    DWORD c=neo_block_size/4;
    memset(outbuffer+0,0,8);
    store_dword_le(c,outbuffer+8);
    check(neodrv->deviceio(inbuffer,neocontrolcodes[18].size,outbuffer,0x0c));
    check(sync1());
    check_read(neodrv->read(outp,neo_block_size));
    prg->doWork(neo_block_size);
    return 1;
}

// #########################################################################

int neocontrol::r_res3(void*outp,DWORD a,DWORD offset,DWORD size)
{
    if(!prg->doWork(0))return 0;
    check(sync0());
    memcpy(inbuffer,neocontrolcodes[20].code,neocontrolcodes[20].size);
    DWORD b=offset/2;
    DWORD c=size/4;
    store_dword_le(a,outbuffer+0);
    store_dword_le(b,outbuffer+4);
    store_dword_le(c,outbuffer+8);
    check(neodrv->deviceio(inbuffer,neocontrolcodes[20].size,outbuffer,0x0c));
    check(sync1());
    DWORD tr;
    char*outbuff=(char*)outp;
    while(size)
    {
        tr=Min(size,neo_block_size);
        check_read(neodrv->read(outbuff,tr));
        size-=tr;
        outbuff+=tr;
        if(size&&(!prg->doWork(tr)))
        {
            cancelio(0);
            return 0;
        }
    }
    return 1;
}

// #########################################################################

int neocontrol::w_data0(const void*inb,DWORD a,DWORD b,DWORD offset,DWORD size)
{
    if(!prg->doWork(0))return 0;
    check(sync0());
    memcpy(inbuffer,neocontrolcodes[23].code,neocontrolcodes[23].size);
    DWORD c=offset/2;
    DWORD d=size/4;
    store_dword_le(a,outbuffer+0);
    store_dword_le(b,outbuffer+4);
    store_dword_le(c,outbuffer+8);
    store_dword_le(d,outbuffer+12);
    check(neodrv->deviceio(inbuffer,neocontrolcodes[23].size,outbuffer,0x10));
    check(sync1());
    DWORD tr;
    char*inbuff=(char*)inb;
    while(size)
    {
        tr=Min(size,neo_block_size);
        check_write(neodrv->write(inbuff,tr));
        size-=tr;
        inbuff+=tr;
        if(size&&(!prg->doWork(tr)))
        {
            cancelio(1);
            return 0;
        }
    }
    return 1;
}

// #########################################################################

int neocontrol::w_data1(const void*inb)
{
    if(!prg->doWork(0))return 0;
    check(sync0());
    memcpy(inbuffer,neocontrolcodes[25].code,neocontrolcodes[25].size);
    DWORD c=neo_block_size/4;
    memset(outbuffer+0,0,12);
    store_dword_le(c,outbuffer+12);
    check(neodrv->deviceio(inbuffer,neocontrolcodes[25].size,outbuffer,0x10));
    check(sync1());
    check_write(neodrv->write(inb,neo_block_size));
    prg->doWork(neo_block_size);
    return 1;
}

// #########################################################################

int neocontrol::w_data2(const void*inb,DWORD a,DWORD b,DWORD offset,DWORD size)
{
    if(!prg->doWork(0))return 0;
    check(sync0());
    memcpy(inbuffer,neocontrolcodes[32].code,neocontrolcodes[32].size);
    DWORD c=offset/2;
    DWORD d=size/4;
    store_dword_le(a,outbuffer+0);
    store_dword_le(b,outbuffer+4);
    store_dword_le(c,outbuffer+8);
    store_dword_le(d,outbuffer+12);
    check(neodrv->deviceio(inbuffer,neocontrolcodes[32].size,outbuffer,0x10));
    check(sync1());
    DWORD tr;
    char*inbuff=(char*)inb;
    while(size)
    {
        tr=Min(size,neo_block_size);
        check_write(neodrv->write(inbuff,tr));
        size-=tr;
        inbuff+=tr;
        if(size&&(!prg->doWork(tr)))
        {
            cancelio(1);
            return 0;
        }
    }
    return 1;
}

// #########################################################################

int neocontrol::edgeFixCmd(membuf&ocmd,DWORD&size,DWORD dir)
{
    const char*cmd0="\x00\x60\x00\xd0\x00\x20\x00\xd0";
    int loops=(getsize(neo_flash,block_size)/finfo->smallblock_size)-1;
    size=loops*(5*6)+6;
    if(ocmd.resize(size))
    {
        BYTE*ucmd=(BYTE*)(char*)ocmd;
        DWORD i=0;
        DWORD addr;
        if(dir==0)
        {
            addr=finfo->smallblock_size;
            if(finfo->nSelect_low!=2)
            {
                addr++;
            }
        }
        else
        {
            addr=getsize(neo_flash,total_size)-finfo->smallblock_size;
            if(finfo->nSelect_high!=2)
            {
                addr++;
            }
        }
        while(loops)
        {
            for(DWORD j=0;j<8;)
            {
                ucmd[i++]=0x05;
                ucmd[i++]=_2byte(addr>>16);
                ucmd[i++]=_2byte(addr>>8);
                ucmd[i++]=_2byte(addr);
                ucmd[i++]=cmd0[j++];
                ucmd[i++]=cmd0[j++];
            }
            for(DWORD j=0;j<3;j++)
            {
                ucmd[i++]=0x10;
                ucmd[i++]=0xff;
            }
            if(dir==0)
            {
                addr+=finfo->smallblock_size;
            }
            else
            {
                addr-=finfo->smallblock_size;
            }
            loops--;
        }
        ucmd[i++]=0x10;
        ucmd[i++]=0x00;
        ucmd[i++]=0x00;
        ucmd[i++]=0x00;
        ucmd[i++]=0x00;
        ucmd[i++]=0xff;
        return 1;
    }
    return 0;
}

// #########################################################################

int neocontrol::asicReset()
{
    return w_cmd0(neocontrolcodes[4].code,neocontrolcodes[4].size);
}

// #########################################################################

int neocontrol::cancelio(int dir)
{
    memcpy(inbuffer,neocontrolcodes[29].code,neocontrolcodes[29].size);
    check(neodrv->deviceio(inbuffer,neocontrolcodes[29].size,outbuffer,0));
    if(dir==0)
    {
        check(neodrv->read(outbuffer,64));
    }
    else
    {
        check(neodrv->write(outbuffer,0));
    }
    Sleep(100);
    return 1;
}

// #########################################################################

int neocontrol::open(neodriver*driver)
{
    close();
    if(driver==0)
    {
        return 0;
    }
    neodrv=driver;
    state=state_loading;

    DWORD i;
    memset(cartid,0,0x40);

    check(asicReset());

    check(w_cmd0(neocontrolcodes[5].code,neocontrolcodes[5].size));
    check(r_res0(outbuffer,0x0e));
    memcpy(cartid+0x00,outbuffer,0x10);
    cartType=0;
    for(i=0;i<0x0e;i++)
    {
        if(outbuffer[i]&0x80)
        {
            cartType=i;
            break;
        }
    }
    store_word_le((WORD)cartType,cartid+0x30);

    check(w_cmd0(neocontrolcodes[6].code,neocontrolcodes[6].size));
    check(r_res0(outbuffer,0x08));
    memcpy(cartid+0x10,outbuffer,0x10);
    WORD w;
    load_word_le(outbuffer,w);
    if(w==0x2000)
    {
        for(i=0;i<4;i++)
        {
            load_word_le(outbuffer+2*i,w);
            if(w!=0x2000&&w!=0x9322&&w!=0xCE88&&w!=0xA000&&w!=0x9800)
            {
                w=0;
                store_word_le(w,outbuffer+2*i);
            }
        }
    }
    else
    {
        for(i=0;i<4;i++)
        {
            load_word_le(outbuffer+2*i,w);
            if(w!=0x8900&&w!=0xC388)
            {
                w=0;
                store_word_le(w,outbuffer+2*i);
            }
        }
    }
    i=0;
    minfo=0;
    while(menu_infos[i].id)
    {
        if(memcmp(outbuffer,menu_infos[i].id,0x08)==0)
        {
            if(menu_infos[i].write2select_menu.code!=0&&menu_infos[i].write2write_menu.code!=0)
            {
                minfo=&menu_infos[i];
                break;
            }
        }
        i++;
    }
    if(minfo==0)
    {
        return 0;
    }
    menuType=i+1;
    store_word_le((WORD)menuType,cartid+0x32);

    check(w_cmd0(neocontrolcodes[33].code,neocontrolcodes[33].size));
    check(r_res0(outbuffer,0x10));
    memcpy(cartid+0x20,outbuffer,0x10);
    i=0;
    finfo=0;
    while(flash_infos[i].id)
    {
        if(memcmp(outbuffer,flash_infos[i].id,0x10)==0)
        {
            if(flash_infos[i].write2select_flash.code!=0&&flash_infos[i].write2write_flash.code!=0)
            {
                finfo=&flash_infos[i];
                break;
            }
        }
        i++;
    }
    if(finfo==0)
    {
        return 0;
    }
    flashType=i+1;
    store_word_le((WORD)flashType,cartid+0x34);

    state=state_ready;
//menu reading (wtf?)
//    check(sync0());
//    check(w_cmd0(minfo->write2select_menu.code,minfo->write2select_menu.size));
//    check(r_res1(largebuffer,0,0xA0000,0x820));
//    SaveFile("eeprom.bin",largebuffer,0x820);

//menu reading
//    check(sync0());
//    check(w_cmd0(minfo->write2select_menu.code,minfo->write2select_menu.size));
//    check(r_res1(largebuffer,0,0,neo_block_size));
//    SaveFile("mdmenu.bin",largebuffer,neo_block_size);

    return state;
}

// #########################################################################

int neocontrol::isConnected()
{
    if(state!=state_ready)return 0;
    check(sync0());
    return 1;
}

// #########################################################################

void neocontrol::setProgress(progress_indicator*sprg)
{
    if(state!=state_ready)return;
    prg=sprg;
}

// #########################################################################

void neocontrol::getcartid(void*data)
{
    memcpy(data,cartid,0x40);
}

// #########################################################################

int neocontrol::read(DWORD param,DWORD offset,void*lpOutBuffer,DWORD nOutBufferSize)
{
    if(state!=state_ready)return 0;
    if(((offset+nOutBufferSize)>getsize(param,total_size))||
       (offset%getsize(param,block_size))||
       (nOutBufferSize%getsize(param,block_size)))
    {
        printf("\n   ***   error: invalid read attempt in line %i of %s\n",__LINE__,__FILE__);
        return 0;
    }
    if(param==neo_menu)
    {
        check(sync0());
        check(w_cmd0(minfo->write2select_menu.code,minfo->write2select_menu.size));
        check(r_res1(lpOutBuffer,0,offset,nOutBufferSize));
        return 1;
    }
    if(param==neo_flash)
    {
        check(sync0());
        DWORD bank=offset/getsize(neo_flash,bank_size);
        DWORD bankEnd=(bank+1)*getsize(neo_flash,bank_size);
        DWORD tRead=((offset+nOutBufferSize)>bankEnd)?bankEnd-offset:nOutBufferSize;
        membuf fcmd;
        check(setFlashBankRead(fcmd,finfo,bank));
        check(w_cmd0(fcmd,finfo->write2select_flash.size));
        check(r_res1(lpOutBuffer,0,offset-(bank*getsize(neo_flash,bank_size)),tRead));
        nOutBufferSize-=tRead;
        if(nOutBufferSize>0)
        {
            char*nb=(char*)lpOutBuffer;
            return read(param,bankEnd,nb+tRead,nOutBufferSize);
        }
        return 1;
    }
    if(param==neo_sram)
    {
        int res=0;
//        check(w_cmd0(minfo->write2select_menu.code,minfo->write2select_menu.size));
        membuf fcmd;
        check(setFlashBankRead(fcmd,finfo,0));
        check(w_cmd0(fcmd,finfo->write2select_flash.size));

        DWORD bank=offset/getsize(neo_sram,bank_size);
        DWORD bank_count=nOutBufferSize/getsize(neo_sram,bank_size);
        char*buff=(char*)lpOutBuffer;
        membuf scmd;
        while(bank<8&&bank_count>0)
        {
            res=1;
            check(setSramBank(scmd,bank));
            check(w_cmd0(scmd,neocontrolcodes[10].size));
            check(r_res2(buff));
            buff+=getsize(neo_sram,bank_size);
            bank++;
            bank_count--;
        }
        return res;
    }
    if(param==neo_psram)
    {
//        check(sync0());
//        check(w_cmd0(neocontrolcodes[19].code,neocontrolcodes[19].size));
//        check(r_res3(lpOutBuffer,0,offset,nOutBufferSize));
//        return 1;
    }
    //if(cartType==n64myth_cart)
    {
        if(param==neo_n64menu)
        {
            offset+=16*MB;
            check(sync0());
            check(w_cmd0(neocontrolcodes[30].code,neocontrolcodes[30].size));
            check(r_res1(lpOutBuffer,0,offset,nOutBufferSize));
            char tmpbuff[0x10];
            check(r_res1(tmpbuff,0,0,4));
            //return memcmp(tmpbuff,"\x01\x00\x00\x00",4)==0;
            return 1;
        }
        if(param==neo_n64sram)
        {
            offset+=24*MB;
            check(sync0());
            check(w_cmd0(neocontrolcodes[30].code,neocontrolcodes[30].size));
            check(r_res3(lpOutBuffer,0,offset,nOutBufferSize));
            char tmpbuff[0x10];
            check(r_res1(tmpbuff,0,0,4));
            //return memcmp(tmpbuff,"\x01\x00\x00\x00",4)==0;
            return 1;
        }
    }
    return 0;
}

// #########################################################################

int neocontrol::write(DWORD param,DWORD offset,const void*lpInBuffer,DWORD nInBufferSize)
{
    if(state!=state_ready)return 0;
    if(((offset+nInBufferSize)>getsize(param,total_size))||
       (offset%getsize(param,block_size))||
       (nInBufferSize%getsize(param,block_size)))
    {
        printf("\n   ***   error: invalid write attempt in line %i of %s\n",__LINE__,__FILE__);
        return 0;
    }
    if(param==neo_menu)
    {
        check(sync0());
        check(w_cmd0(minfo->write2select_menu.code,minfo->write2select_menu.size));
        check(w_cmd1(minfo->write2write_menu.code,minfo->write2write_menu.size));
        check(w_data0(lpInBuffer,0,0,offset,nInBufferSize));
        return 1;
    }
    if(param==neo_flash)
    {
        check(sync0());
        check(asicReset());

        DWORD nDies=finfo->nDies;
        if(nDies>=4)
        {
            nDies=2;
        }
        DWORD crossSize=nDies*getsize(neo_flash,block_size);
        if(offset%crossSize)
        {
            DWORD noffset=offset-(offset%crossSize);
            DWORD splus=offset-noffset;
            membuf ndata;
            if(ndata.resize(nInBufferSize+splus))
            {
                prg->incWork(splus*2);
                read(neo_flash,noffset,ndata,crossSize);
                memcpy(ndata+splus,lpInBuffer,nInBufferSize);
                return write(neo_flash,noffset,ndata,nInBufferSize+splus);
            }
            return 0;
        }
        DWORD rightEdge=offset+nInBufferSize;
        if((rightEdge%crossSize)&&((crossSize-rightEdge%crossSize)>=(128*KB)))
        {
            DWORD nrightEdge=rightEdge+crossSize-(rightEdge%crossSize);
            DWORD splus=nrightEdge-rightEdge;
            membuf ndata;
            if(ndata.resize(nInBufferSize+splus))
            {
                prg->incWork(splus*2);
                read(neo_flash,nrightEdge-crossSize,ndata+nInBufferSize+splus-crossSize,crossSize);
                memcpy(ndata,lpInBuffer,nInBufferSize);
                return write(neo_flash,offset,ndata,nInBufferSize+splus);
            }
            return 0;
        }
        DWORD bank=offset/getsize(neo_flash,bank_size);
        DWORD bankEnd=(bank+1)*getsize(neo_flash,bank_size);
        DWORD tWrite=((offset+nInBufferSize)>bankEnd)?bankEnd-offset:nInBufferSize;
        membuf fcmd;
        check(setFlashBankRead(fcmd,finfo,bank));
        check(w_cmd0(fcmd,finfo->write2select_flash.size));
        if(finfo->nSelect_low&2)
        {
            if(offset<=((finfo->nDies&3)*getsize(neo_flash,block_size)))
            {
                DWORD size=0;
                check(edgeFixCmd(fcmd,size,0));
                check(w_cmd0(fcmd,size));
            }
        }
        if(finfo->nSelect_high&2)
        {
            DWORD rightEdge=getsize(neo_flash,total_size)-(finfo->nDies&3)*getsize(neo_flash,block_size);
            if((offset>=rightEdge)||((offset+nInBufferSize)>rightEdge))
            {
                DWORD size=0;
                check(edgeFixCmd(fcmd,size,1));
                check(w_cmd0(fcmd,size));
            }
        }
        check(setFlashBankWrite(fcmd,finfo,bank));
        check(w_cmd1(fcmd,finfo->write2write_flash.size));

        check(w_data0(lpInBuffer,0,0,offset-(bank*getsize(neo_flash,bank_size)),tWrite));
        nInBufferSize-=tWrite;
        if(nInBufferSize>0)
        {
            char*nb=(char*)lpInBuffer;
            return write(param,bankEnd,nb+tWrite,nInBufferSize);
        }
        return 1;
    }
    if(param==neo_sram)
    {
        int res=0;
        check(sync0());
        //check(w_cmd0(neocontrolcodes[24].code,neocontrolcodes[24].size));
//        check(w_cmd0(minfo->write2select_menu.code,minfo->write2select_menu.size));
        membuf fcmd;
        check(setFlashBankRead(fcmd,finfo,0));
        check(w_cmd0(fcmd,finfo->write2select_flash.size));

        DWORD bank=offset/getsize(neo_sram,bank_size);
        DWORD bank_count=nInBufferSize/getsize(neo_sram,bank_size);
        char*buff=(char*)lpInBuffer;
        membuf scmd;
        while(bank<8&&bank_count>0)
        {
            res=1;
            check(setSramBank(scmd,bank));
            check(w_cmd0(scmd,neocontrolcodes[10].size));
            check(w_data1(buff));
            buff+=getsize(neo_sram,bank_size);
            bank++;
            bank_count--;
        }
        return res;
    }
    if(param==neo_psram)
    {
        //later
    }
    //if(cartType==n64myth_cart)
    {
        if(param==neo_n64menu)
        {
            offset+=16*MB;
            check(sync0());
            check(w_cmd0(neocontrolcodes[30].code,neocontrolcodes[30].size));
            check(w_cmd1(neocontrolcodes[31].code,neocontrolcodes[31].size));
            check(w_data0(lpInBuffer,0,0,offset,nInBufferSize));
            char tmpbuff[0x10];
            check(r_res1(tmpbuff,0,0,4));
            //return memcmp(tmpbuff,"\x01\x00\x00\x00",4)==0;
            return 1;
        }
        if(param==neo_n64sram)
        {
            offset+=24*MB;
            check(sync0());
            check(w_cmd0(neocontrolcodes[30].code,neocontrolcodes[30].size));
            check(w_data2(lpInBuffer,0,0,offset,nInBufferSize));
            char tmpbuff[0x10];
            check(r_res1(tmpbuff,0,0,4));
            //return memcmp(tmpbuff,"\x01\x00\x00\x00",4)==0;
            return 1;
        }
    }
    return 0;
}

// #########################################################################

DWORD neocontrol::getsize(DWORD param,DWORD bs)
{
    if(state!=state_ready)return 0;
    if(bs==total_size)
    {
        if(param==neo_menu)
        {
            return minfo->menu_size;
        }
        if(param==neo_flash)
        {
            return finfo->flash_size;
        }
        if(param==neo_sram)
        {
            return minfo->sram_size;
        }
        if(param==neo_psram)
        {
        }
        //if(cartType==n64myth_cart)
        {
            if(param==neo_n64menu)
            {
                return 2*MB;
            }
            if(param==neo_n64sram)
            {
                return 4*64*KB;
            }
        }
    }
    if(bs==bank_size)
    {
        if(param==neo_menu)
        {
            return minfo->menu_size;
        }
        if(param==neo_flash)
        {
            return 32*MB;
        }
        if(param==neo_sram)
        {
            return neo_sram_block_size;
        }
        if(param==neo_psram)
        {
        }
        //if(cartType==n64myth_cart)
        {
            if(param==neo_n64menu)
            {
                return 2*MB;
            }
            if(param==neo_n64sram)
            {
                return 64*KB;
            }
        }
    }
    if(bs==block_size)
    {
        if(param==neo_menu)
        {
            return minfo->normalblock_size;
        }
        if(param==neo_flash)
        {
            return finfo->normalblock_size;
        }
        if(param==neo_sram)
        {
            return neo_sram_block_size;
        }
        if(param==neo_psram)
        {
        }
        //if(cartType==n64myth_cart)
        {
            if(param==neo_n64menu)
            {
                return 128*KB;
            }
            if(param==neo_n64sram)
            {
                return 64*KB;
            }
        }
    }
    if(bs==ideal_block_size)
    {
        if(param==neo_flash)
        {
            DWORD nDies=finfo->nDies;
            if(nDies>=4)
            {
                nDies=2;
            }
            if(flashType==9)//update needed whenever flash_infos are updated
            {
                nDies=2;
            }
            return (nDies?nDies:1)*getsize(param,block_size);
        }
        return getsize(param,block_size);
    }
    return 0;
}

// #########################################################################

int neocontrol::getFlashAsicType()
{
    if(state!=state_ready)return 0;
    if(finfo->write2select_flash.size>0x47)
    {
        if(memcmp(finfo->write2select_flash.code+0x44,"\x00\xda\xae\x44",4)==0)
        {
            return 1;
        }
        if(memcmp(finfo->write2select_flash.code+0x44,"\x00\xda\x8e\x44",4)==0)
        {
            return 2;
        }
        if(memcmp(finfo->write2select_flash.code+0x44,"\x00\xda\x0e\x44",4)==0)
        {
            return 3;
        }
    }
    return 0;
}

// #########################################################################

void neocontrol::close()
{
    if(state==state_undefined)return;
    if(neodrv)
    {
        sync0();
    }
    state=state_undefined;
}

// #########################################################################
