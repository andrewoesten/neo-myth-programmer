// #########################################################################

#include <neoutil.h>
#include <sys/time.h>
#include <md5int.h>

// #########################################################################

static int go_enableKnownPatches=0;
static int go_savePatchedFiles=0;
static int go_ioBlockSize=1*MB;
static int go_file_ioBlockSize=8*MB;
static int go_romAlignment=64*KB;
static int go_displaySzMb=0;

#define getopm(o) case o: memcpy(data,&go_ ## o,sizeof(go_ ## o)); break
#define setopm(o) case o: memcpy(&go_ ## o,data,sizeof(go_ ## o)); break

void getoption(int option,void*data)
{
    switch(option)
    {
        getopm(enableKnownPatches);
        getopm(savePatchedFiles);
        getopm(ioBlockSize);
        getopm(file_ioBlockSize);
        getopm(romAlignment);
        getopm(displaySzMb);
    };
}

void setoption(int option,void*data)
{
    switch(option)
    {
        setopm(enableKnownPatches);
        setopm(savePatchedFiles);
        setopm(ioBlockSize);
        setopm(file_ioBlockSize);
        setopm(romAlignment);
        setopm(displaySzMb);
    };
}

#undef getopm
#undef setopm

int getoption(int option)
{
    int res;
    getoption(option,&res);
    return res;
}

// #########################################################################

#define _unique_local_name_1(suffix,line) unique_local_name_ ## suffix ## _ ## line
#define _unique_local_name_0(suffix,line) _unique_local_name_1(suffix,line)
#define unique_local_name(suffix) _unique_local_name_0(suffix,__LINE__)

#define ct_check(expr) extern void unique_local_name(_ct_check)(int a[((expr)!=0)*2-1])

//error here means that the fixed width integers types were not defined correctly
ct_check(sizeof(DWORD)==4);
ct_check(sizeof(WORD)==2);
ct_check(sizeof(BYTE)==1);
//change typedefs in neoutil.h(lines 10-12)

// #########################################################################

int SaveFile(const char*FileName,char*X,DWORD fs)
{
    int rv=0;
    FILE*hf=fopen(FileName,"wb");
    if(hf)
    {
        if(fwrite(X,1,fs,hf)==fs)
        {
            rv=1;
        }
        fclose(hf);
    }
    return rv;
}

// #########################################################################

void LoadFile(const char*FileName,char*X,DWORD&fs)
{
    fs=0;
    FILE*hf=fopen(FileName,"rb");
    if(hf)
    {
        fseek(hf,0,SEEK_END);
        fs=ftell(hf);
        if(X)
        {
            fseek(hf,0,SEEK_SET);
            if(fread(X,1,fs,hf)!=fs)
            {
                fs=0;
            }
        }
        fclose(hf);
    }
}

// #########################################################################

void LoadFile(const char*FileName,membuf&X,DWORD&fs)
{
    fs=0;
    FILE*hf=fopen(FileName,"rb");
    if(hf)
    {
        fseek(hf,0,SEEK_END);
        fs=ftell(hf);
        if(!X.resize(fs))
        {
            fs=0;
        }
        else
        {
            fseek(hf,0,SEEK_SET);
            if(fread(X,1,fs,hf)!=fs)
            {
                fs=0;
            }
        }
        fclose(hf);
    }
}

// #########################################################################

int file_exists(const char*FileName)
{
    DWORD fs;
    LoadFile(FileName,0,fs);
    return fs>0;
}

// #########################################################################

membuf::membuf()
{
    data=0;
    size=0;
}

membuf::membuf(int sz)
{
    data=0;
    size=0;
    resize(sz);
}

membuf::~membuf()
{
    free();
}

int membuf::resize(int sz)
{
    if(sz>128*MB)
    {
        return 0;
    }
    if((sz<size)&&((size-sz)<2*MB))
    {
        return 1;
    }
    char*ndata=new char[sz];
    if(ndata==0)
    {
        printf("\n   ***   error: out of memory\n");
        return 0;
    }
    if(data)
    {
        memcpy(ndata,data,Min(size,sz));
        free();
    }
    data=ndata;
    size=sz;
    return 1;
}

void membuf::free()
{
    if(data)
    {
        delete[] data;
        data=0;
    }
    size=0;
}

membuf::operator char* () const
{
    return data;
}

// #########################################################################

int atoiX(char*str,int ibase,int&err)
{
    err=-1;
    int base=ibase==0?10:ibase;
    int A=0;
    int i=0;
    while(str[i])
    {
        char c=char(str[i++]);
        if((i==1)&&(c=='-'))
        {
            return 0-atoiX(str+1,base,err);
        }
        if((c>='0')&&(c<='9'))
        {
            A*=base;
            A+=c-'0';
            continue;
        }
        c|=32;
        if((i==2)&&(c=='x')&&(str[0]=='0'))
        {
            return atoiX(str+2,16,err);
        }
        if((c>='a')&&(c<='f'))
        {
            if(base!=16)
            {
                if(ibase==0)
                {
                    return atoiX(str,16,err);
                }
                else
                {
                    return A;
                }
            }
            A*=base;
            A+=c-'a'+10;
            continue;
        }
        if(strcmp(str+i-1,"MB")==0)
        {
            err=0;
            return ((i==1)?1:A)*MB;
        }
        if(strcmp(str+i-1,"KB")==0)
        {
            err=0;
            return ((i==1)?1:A)*KB;
        }
        if(strcmp(str+i-1,"Mb")==0)
        {
            err=0;
            return ((i==1)?1:A)*(MB/8);
        }
        if(strcmp(str+i-1,"Kb")==0)
        {
            err=0;
            return ((i==1)?1:A)*(KB/8);
        }
        return A;
    }
    err=0;
    return A;
}

// #########################################################################

void load_word_le(void*dv,WORD&x)
{
    BYTE*data=(BYTE*)dv;
    x=0;
    x+=data[1];
    x=x<<8;
    x+=data[0];
}

void load_dword_le(void*dv,DWORD&x)
{
    BYTE*data=(BYTE*)dv;
    x=0;
    x+=data[3];
    x=x<<8;
    x+=data[2];
    x=x<<8;
    x+=data[1];
    x=x<<8;
    x+=data[0];
}

void store_word_le(WORD x,void*dv)
{
    BYTE*data=(BYTE*)dv;
    data[0]=_2byte(x);
    x=x>>8;
    data[1]=_2byte(x);
}

void store_dword_le(DWORD x,void*dv)
{
    BYTE*data=(BYTE*)dv;
    data[0]=_2byte(x);
    x=x>>8;
    data[1]=_2byte(x);
    x=x>>8;
    data[2]=_2byte(x);
    x=x>>8;
    data[3]=_2byte(x);
}

// #########################################################################

void load_word_be(void*dv,WORD&x)
{
    BYTE*data=(BYTE*)dv;
    x=0;
    x+=data[0];
    x=x<<8;
    x+=data[1];
}

void load_dword_be(void*dv,DWORD&x)
{
    BYTE*data=(BYTE*)dv;
    x=0;
    x+=data[0];
    x=x<<8;
    x+=data[1];
    x=x<<8;
    x+=data[2];
    x=x<<8;
    x+=data[3];
}

void store_word_be(WORD x,void*dv)
{
    BYTE*data=(BYTE*)dv;
    data[1]=_2byte(x);
    x=x>>8;
    data[0]=_2byte(x);
}

void store_dword_be(DWORD x,void*dv)
{
    BYTE*data=(BYTE*)dv;
    data[3]=_2byte(x);
    x=x>>8;
    data[2]=_2byte(x);
    x=x>>8;
    data[1]=_2byte(x);
    x=x>>8;
    data[0]=_2byte(x);
}

// #########################################################################

char*gettmpbuffer(DWORD size)
{
    const int strc=16;
    static int index=strc;
    static membuf buffers[strc];
    index=(index+1)%strc;
    buffers[index].resize(size);
    return buffers[index];
}

// #########################################################################

int print_size(char*buffer,DWORD size,DWORD block)
{
    int l=0;
    unsigned int sz=size/block;
    if(sz>0||block<2)
    {
        l=sprintf(buffer,"%.2u%s",sz,block==MB?"MB":block==KB?"KB":block==Mb?"Mb":block==Kb?"Kb":block==1?"B":"");
    }
    return l;
}

// #########################################################################

char*print_size(DWORD size,DWORD block)
{
    char*buffer=gettmpbuffer(64);
    int l=0;
    if(size/block>0)
    {
        l+=print_size(buffer+l,size,block);
        size=size%block;
        if(size==0)return buffer;
        l+=sprintf(buffer+l," ");
    }
    l+=print_size(buffer+l,size,1);
    return buffer;
}

// #########################################################################

char*print_size(DWORD size)
{
    char*buffer=gettmpbuffer(64);
    int l=0;
    if(getoption(displaySzMb))
    {
        if(size/Mb>0)
        {
            l+=print_size(buffer+l,size,Mb);
            size=size%Mb;
            if(size==0)return buffer;
            l+=sprintf(buffer+l," ");
        }
        if(size/Kb>0)
        {
            l+=print_size(buffer+l,size,Kb);
            size=size%Kb;
            if(size==0)return buffer;
            l+=sprintf(buffer+l," ");
        }
    }
    else
    {
        if(size/MB>0)
        {
            l+=print_size(buffer+l,size,MB);
            size=size%MB;
            if(size==0)return buffer;
            l+=sprintf(buffer+l," ");
        }
        if(size/KB>0)
        {
            l+=print_size(buffer+l,size,KB);
            size=size%KB;
            if(size==0)return buffer;
            l+=sprintf(buffer+l," ");
        }
    }
    l+=print_size(buffer+l,size,1);
    return buffer;
}

// #########################################################################

int testPathMask(const char*path,const char*mask)
{
    DWORD i=0,j=0;
    char p,m;
    while(path[i]&&mask[j])
    {
        p=path[i]|32;
        m=mask[j]|32;
        if(m=='*')
        {
            if(testPathMask(path+i,mask+j+1)==0)
            {
                return 0;
            }
            i++;
            if(path[i]==0)
            {
                j++;
            }
        }
        else
        {
            if(m=='?')
            {
                i++;
                j++;
            }
            else
            {
                if(p!=m)
                {
                    break;
                }
                i++;
                j++;
            }
        }
    }
    p=path[i]?path[i]|32:0;
    m=mask[j]?mask[j]|32:0;
    return p-m;
}

// #########################################################################

inline void swap_dwords(DWORD&A,DWORD&B)
{
    DWORD C=A;A=B;B=C;
}

DWORD partition(DWORD*index,DWORD p,DWORD r,sortcallback sortf,void*data)
{
    DWORD left=p;
    for(DWORD i=p+1;i<=r;i++)
    {
        if(sortf(data,index[p],index[i])>0)
        {
            left++;
            swap_dwords(index[i],index[left]);
        }
    }
    swap_dwords(index[p],index[left]);
    return left;
}

void quickSort(DWORD*index,DWORD p,DWORD r,sortcallback sortf,void*data)
{
    if(p<r)
    {
        DWORD q=partition(index,p,r,sortf,data);
        if(q)
        {
            quickSort(index,p,q-1,sortf,data);
        }
        quickSort(index,q+1,r,sortf,data);
    }
}

// #########################################################################

static MD5_CTX g_md5ctx;

void getmd5(const void*data,const int size,md5struct&md5)
{
    MD5_CTX md5ctx;
    MD5Init(&md5ctx);
    MD5Update(&md5ctx,data,size);
    memcpy(&g_md5ctx,&md5ctx,sizeof(md5ctx));
    MD5Final(&md5,&md5ctx);
}

void updatemd5(const void*data,const int size,md5struct&md5)
{
    MD5_CTX md5ctx;
    memcpy(&md5ctx,&g_md5ctx,sizeof(md5ctx));
    MD5Update(&md5ctx,data,size);
    memcpy(&g_md5ctx,&md5ctx,sizeof(md5ctx));
    MD5Final(&md5,&md5ctx);
}

// #########################################################################

const char*getFileName(const char*fname)
{
    DWORD l=strlen(fname);
    while(l&&fname[l]!=dDelim0&&fname[l]!=dDelim1)
    {
        l--;
    }
    l=(fname[l]==dDelim0||fname[l]==dDelim1)?l+1:0;
    return fname+l;
}

// #########################################################################

char*getShortName(const char*fname)
{
    char*buffer=gettmpbuffer(4*KB);
    DWORD l;
    l=strlen(fname);
    while(l&&fname[l]!=dDelim0&&fname[l]!=dDelim1)
    {
        l--;
    }
    l=(fname[l]==dDelim0||fname[l]==dDelim1)?l+1:0;
    while(fname[l]==' ')
    {
        l++;
    }
    strcpy(buffer,fname+l);
    l=strlen(buffer);
    while(l&&buffer[l]!='.')
    {
        l--;
    }
    if(l>0)
    {
        buffer[l]=0;
    }
    l=strlen(buffer);
    while(l&&buffer[l-1]==' ')
    {
        l--;
    }
    //---------------
    if(l>23)
    {
        l=23;
        while(l&&buffer[l]!='('&&buffer[l]!='[')
        {
            l--;
        }
        if(l>0)
        {
            while(l&&buffer[l-1]==' ')
            {
                l--;
            }
            buffer[l]=0;
        }
    }
    return buffer;
}

// #########################################################################

progress_indicator::progress_indicator()
{
    progress=0;
    total=0;
    ipause=0;
    istop=0;
    textDesc.resize(128);
    textDesc[0]=0;
    glbprg=0;
    subprg=0;
}

progress_indicator::~progress_indicator()
{
    textDesc.free();
}

void progress_indicator::update()
{
    if(this==0)return;
    timeval ts;
    gettimeofday(&ts,0);
    DWORD lticks=(DWORD)(ts.tv_sec*1000+(ts.tv_usec/1000));
    DWORD eltime=lticks-ticks;
    if(eltime>300)
    {
        ticks=lticks;
        char*td=textDesc;
        int ltotal=total?total:100;
        int lprogress=getProgress();
        onUpdate(td[0]?td:0,lprogress,ltotal);
        textDesc[0]=0;
        glbprg->update();
    }
}

void progress_indicator::onUpdate(char*desc,int pr,int rng)
{
}

void progress_indicator::setText(const char*desc)
{
    if(this==0)return;
    if(desc)
    {
        textDesc.resize(strlen(desc)+16);
        strcpy(textDesc,desc);
    }
    else
    {
        textDesc[0]=0;
    }
}

void progress_indicator::reset()
{
    if(this==0)return;
    ticks-=10000;
    progress=0;
    ipause=0;
    istop=0;
    update();
}

int progress_indicator::doWork(int d)
{
    if(this==0)return 1;
    progress+=d;
    update();
    return istop?0:1;
}

void progress_indicator::incWork(int d)
{
    if(this==0)return;
    total+=d;
}

void progress_indicator::setWork(int w)
{
    if(this==0)return;
    total=w;
}

void progress_indicator::pause(int p)
{
    if(this==0)return;
    ipause=p?1:0;
    subprg->pause(p);
}

void progress_indicator::stop()
{
    if(this==0)return;
    istop=1;
    subprg->stop();
}

void progress_indicator::wait()
{
    if(this==0)return;
    while(ipause&&(!istop))
    {
        Sleep(100);
    }
}

void progress_indicator::done(const char*desc)
{
    if(this==0)return;
    ticks-=10000;
    int dw=total-progress;
    setText(desc);
    doWork(dw);
    wait();
}

void progress_indicator::setGlbProgress(progress_indicator*prg)
{
    if(this==0)return;
    glbprg=prg;
}

void progress_indicator::setSubProgress(progress_indicator*prg)
{
    if(this==0)return;
    subprg->setGlbProgress(0);
    subprg=prg;
    subprg->setGlbProgress(this);
}

progress_indicator*progress_indicator::subProgress()
{
    if(this==0)return 0;
    return subprg;
}

int progress_indicator::getProgress()
{
    if(this==0)return 0;
    return Min(progress+subprg->getProgress(),total);
}

// #########################################################################
