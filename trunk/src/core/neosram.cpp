// #########################################################################

#include <neosram.h>

// #########################################################################

neosram::neosram()
{
    state=state_undefined;
    bankstate=0;
}

// #########################################################################

neosram::~neosram()
{
    close();
}

// #########################################################################

int neosram::readbank(int nb)
{
    if(state!=state_ready)return 0;
    if((bankstate[nb]&bank_read)==0)
    {
        const int nread=5;
        const int nmatch=3;
        membuf nullbank;
        membuf sdata[nread];
        int offset=nb*banksize;
        int sucess=-1;
        check(nullbank.resize(banksize));
        memset(nullbank,0,banksize);
        for(int i=0;i<nread&&sucess==-1;i++)
        {
            check(sdata[i].resize(banksize));
            store_dword_le(i,sdata[i]+banksize-8);
            if(neoctrl->read(neoSramLocation,offset,sdata[i],banksize))
            {
                int match=0;
                if(memcmp(sdata[i],nullbank,banksize)!=0)
                {
                    for(int j=0;j<i&&sucess==-1;j++)
                    {
                        if(memcmp(sdata[j],sdata[i],banksize)==0)
                        {
                            match++;
                        }
                    }
                }
                if(match>=nmatch)
                {
                    sucess=i;
                }
            }
        }
        if(sucess==-1)
        {
            return 0;
        }
        memcpy(sram+offset,sdata[sucess],banksize);
        bankstate[nb]|=bank_read;
    }
    return 1;
}

// #########################################################################

int neosram::writebank(int nb)
{
    if(state!=state_ready)return 0;
    if(bankstate[nb]&bank_write)
    {
        const int nwrite=3;
        int offset=nb*banksize;
        int sucess=-1;
        membuf sdata;
        check(sdata.resize(banksize));
        memcpy(sdata,sram+offset,banksize);
        for(int i=0;i<nwrite&&sucess==-1;i++)
        {
            if(neoctrl->write(neoSramLocation,offset,sram+offset,banksize))
            {
                bankstate[nb]&=~bank_read;
                if(readbank(nb))
                {
                    if(memcmp(sram+offset,sdata,banksize)==0)
                    {
                        sucess=1;
                    }
                }
            }
            memcpy(sram+offset,sdata,banksize);
        }
        if(sucess==-1)
        {
            return 0;
        }
        bankstate[nb]&=~bank_write;
    }
    return 1;
}

// #########################################################################

int neosram::open(neocontrol*nc,int sloc)
{
    close();
    state=state_loading;
    neoctrl=nc;
    neoSramLocation=sloc;
    if(!neoctrl)
    {
        return 0;
    }
    banksize=neoctrl->getsize(neoSramLocation,bank_size);
    if(!banksize)
    {
        return 0;
    }
    numbanks=neoctrl->getsize(neoSramLocation,total_size)/banksize;
    if(!numbanks)
    {
        return 0;
    }
    sramsize=banksize*numbanks;
    bankstate=new int[numbanks];
    if(!bankstate)
    {
        return 0;
    }
    for(int i=0;i<numbanks;i++)
    {
        bankstate[i]=0;
    }
    if(!sram.resize(sramsize))
    {
        return 0;
    }
    memset(sram,0,sramsize);
    state=state_ready;
    return state;
}

// #########################################################################

int neosram::read(void*data,int offset,int size)
{
    if(state!=state_ready)return 0;
    if(offset<0||(offset+size)>sramsize)
    {
        return 0;
    }
    int b0=offset/banksize;
    int b1=((offset+size)/banksize)+1;
    for(int i=b0;i<b1;i++)
    {
        check(readbank(i));
    }
    memcpy(data,sram+offset,size);
    return 1;
}

// #########################################################################

int neosram::write(void*data,int offset,int size)
{
    if(state!=state_ready)return 0;
    if(offset<0||(offset+size)>sramsize)
    {
        return 0;
    }
    int b0=offset/banksize;
    int b1=((offset+size)/banksize)+1;
    for(int i=b0;i<b1;i++)
    {
        check(readbank(i));
        bankstate[i]|=bank_write;
    }
    memcpy(sram+offset,data,size);
    return 1;
}

// #########################################################################

int neosram::burn()
{
    if(state!=state_ready)return 0;
    for(int i=0;i<numbanks;i++)
    {
        check(writebank(i));
    }
    return 1;
}

// #########################################################################

void neosram::close()
{
    if(state==state_undefined)return;
    if(bankstate)
    {
        delete[] bankstate;
        bankstate=0;
    }
    sram.free();
    state=state_undefined;
}

// #########################################################################
