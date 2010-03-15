// #########################################################################

#include <neocart.h>
#include <romdetect.h>
#include <plugins.h>

// #########################################################################

struct menuEntry
{
    BYTE meValid;              /* 0x00 = valid, 0xFF = invalid (end of entries) */
    BYTE meType;               /* 0 = MD game, 1 = 32X game, 2 = SMS game */
    BYTE meROMHi;              /* LSN = GBA flash ROM high (A27-A24), MSN = GBA flash ROM size */
    BYTE meROMLo;              /* GBA flash ROM low (A23-A16) */
    BYTE meSRAM;               /* LSN = GBA SRAM size, MSN = GBA SRAM bank */
    BYTE mePad1;               /* reserved */
    BYTE meRun;                /* run mode: 6 = MD/32X game, 8 = CD BIOS, 0x13 = SMS */
    BYTE mePad2;               /* reserved */
    char meName[24];                    /* entry name string (null terminated) */
};

// #########################################################################

const short int fsz_tbl[16] = { 0,1,2,0,4,5,6,0,8,16,24,32,40,0,0,0 };

// #########################################################################

struct alloctable
{
    int offset;
    int size;
    int bblock;
};

// #########################################################################

int getTableSize(int size)
{
    size=Fix(size,table_block_size)/(table_block_size);
    int rhi=0;
    if(size<=(5*MB))
    {
        for(int i=0;i<16;i++)
        {
            if(fsz_tbl[i]>=size)
            {
                rhi=i;
                break;
            }
        }
    }
    if(fsz_tbl[rhi]==0)
    {
//        printf("\n   ***   error: invalid size recieved in line %i of %s\n",__LINE__,__FILE__);
    }
    return rhi;
}

// #########################################################################

int getn64SramSize(int n64_saveType)
{
    if(n64_saveType==5)return st_n64eep4k;
    if(n64_saveType==6)return st_n64eep16k;
    if(n64_saveType==1)return st_n64sram;
    if(n64_saveType==4)return st_n64flash;
    return 0;
}

// #########################################################################

int gs_sortbyoffset(void*data,DWORD a,DWORD b)
{
    selEntry*gSelections=(selEntry*)data;
    if(gSelections[a].offset>gSelections[b].offset)
    {
        return 1;
    }
    return 0;
}

// #########################################################################

int gs_sortbyname(void*data,DWORD a,DWORD b)
{
    selEntry*gSelections=(selEntry*)data;
    if(strcasecmp(gSelections[a].name,gSelections[b].name)>0)
    {
        return 1;
    }
    return 0;
}

// #########################################################################

int at_sortbyoffset(void*data,DWORD a,DWORD b)
{
    alloctable*allt=(alloctable*)data;
    if(allt[a].offset>allt[b].offset)
    {
        return 1;
    }
    return 0;
}

// #########################################################################

int selectRunMode(int type,int run,int size,int eeprom,int ssize)
{
    if(run==5)
    {
        if(type==0)
        {
            run=6;
        }
        if(type==1)
        {
            run=3;
        }
        if(eeprom)
        {
            return 5;
        }
    }
    switch(run)
    {
        case 1:
        case 2:
        case 4:
        case 6:
            if(size>4*MB)
            {
                return 4;
            }
            if(eeprom)
            {
                return 5;
            }
            if(ssize==0)
            {
                return 6;
            }
            else
            {
                if(size<=2*MB)
                {
                    return 1;
                }
                else
                {
                    return 2;
                }
            }
        case 3:
            if(eeprom)
            {
                return 5;
            }
    }
    return run;
}

// #########################################################################

neocart::neocart()
{
    state=state_undefined;
    gSelections=0;
    gSelections_index=0;
    prg=0;
}

// #########################################################################

neocart::~neocart()
{
    close();
}

// #########################################################################

int neocart::open(neocontrol*nc)
{
    close();
    if(nc==0)
    {
        return 0;
    }
    neoctrl=nc;
    state=state_loading;

    if(!neomenu.resize(2*MB))
    {
        return 0;
    }
    if(!sram.open(neoctrl,neo_sram))
    {
        return 0;
    }
    if(!n64sram.open(neoctrl,neo_n64sram))
    {
        //ignore return 0;
    }
    gSelections=new selEntry[MAX_ENTRIES*3];
    if(!gSelections)
    {
        return 0;
    }
    gSelections_index=new DWORD[MAX_ENTRIES*3];
    if(!gSelections_index)
    {
        return 0;
    }
    menu_size=default_menu_size;

    neoctrl->read(neo_menu,0,neomenu,menu_size);
    neoctrl->read(neo_menu,badblocks_offset,neomenu+badblocks_offset,badblocks_size);

    gMaxEntry=0;
    menuEntry*p=(menuEntry*)(neomenu+menu_offset);
    while(p->meValid==0&&p->meRun<65&&(p->meName[0]!=0||p->meType==5))
    {
        gSelections_index[gMaxEntry]=gMaxEntry;
        gSelections[gMaxEntry].id=gMaxEntry+1;
        gSelections[gMaxEntry].dirty=0;
        gSelections[gMaxEntry].deleted=0;
        gSelections[gMaxEntry].type=p->meType;
        gSelections[gMaxEntry].run=p->meRun;
        gSelections[gMaxEntry].bbank=(p->meSRAM&0xF0)>>4;
        gSelections[gMaxEntry].bsize=(p->meSRAM&0x0F)?1<<((p->meSRAM&0x0F)-1):0;
        gSelections[gMaxEntry].offset=((p->meROMHi&0x0F)<<25)|(p->meROMLo<<17);
        if(p->meType==5)//[n64]
        {
            WORD n64l;
            load_word_le(p->meName,n64l);
            gSelections[gMaxEntry].length=n64l;
            gSelections[gMaxEntry].length*=128*KB;
            memcpy(gSelections[gMaxEntry].name,p->meName+2,22);
            gSelections[gMaxEntry].name[21]=0;
        }
        else
        {
            gSelections[gMaxEntry].length=fsz_tbl[(p->meROMHi&0xF0)>>4]*table_block_size;
            memcpy(gSelections[gMaxEntry].name,p->meName,24);
            gSelections[gMaxEntry].name[23]=0;
        }

        gMaxEntry++;
        p++;
        if(gMaxEntry==MAX_ENTRIES)
        {
            break;
        }
    }
    gRomId=gMaxEntry+1;

    int readn64=0;//[n64]
    for(int i=0;i<gMaxEntry;i++)
    {
        if(gSelections[i].type==5)
        {
            readn64=1;
        }
    }
    if(readn64)
    {
        const int n64offset=31*64*KB;
        neoctrl->read(neo_menu,n64offset,neomenu+n64offset,64*KB);
        for(int i=0;i<gMaxEntry;i++)
        {
            if(gSelections[i].type==5)
            {
                BYTE*val=(BYTE*)(gSelections[i].offset==0?&neomenu[0x1ffff2]:&neomenu[0x1ffff4]);
                gSelections[i].n64_cic=val[0]&0x0f;
                gSelections[i].n64_saveType=val[0]/16;
                gSelections[i].n64_modeA=val[1];
            }
        }
    }

    printf("\n\n");
    printf("\tmenu size: %s\n",print_size(neoctrl->getsize(neo_menu,total_size)));
    printf("\tsram size: %s\n",print_size(neoctrl->getsize(neo_sram,total_size)));
    printf("\tflash size: %s\n",print_size(neoctrl->getsize(neo_flash,total_size)));
    printf("\n\n");
    if(gMaxEntry)
    {
        //quickSort(gSelections_index,0,gMaxEntry-1,gs_sortbyname,gSelections);
        quickSort(gSelections_index,0,gMaxEntry-1,gs_sortbyoffset,gSelections);
    }
    int totalUsed=0;
    int totalFree=0;
    for(int i=0;i<gMaxEntry;i++)
    {
        selEntry&gSelection=sorted(gSelections,i);
        if(i==0)
        {
            if(gSelection.offset!=0)
            {
                printf("  free space: %s\n\n",print_size(gSelection.offset));
                totalFree+=gSelection.offset;
            }
        }
        else
        {
            int gap=gSelection.offset-(sorted(gSelections,i-1).offset+sorted(gSelections,i-1).length);
            if(gap>0)
            {
                printf("  free space: %s\n\n",print_size(gap));
                totalFree+=gap;
            }
        }
        printf("\t%.4u\t\t%s\ntype=%i, run=%i, bbank=%.2u, bsize=%.4u, offset=0x%.8x, size=%s\n\n",gSelection.id,gSelection.name,gSelection.type,gSelection.run,gSelection.bbank,gSelection.bsize,gSelection.offset,print_size(gSelection.length));
        totalUsed+=gSelection.length;
    }
    int gap=neoctrl->getsize(neo_flash,total_size);
    if(gMaxEntry)
    {
        gap-=(sorted(gSelections,gMaxEntry-1).offset+sorted(gSelections,gMaxEntry-1).length);
    }
    if(gap>0)
    {
        printf(" free space: %s\n\n",print_size(gap));
        totalFree+=gap;
    }
    const WORD*bblocks=(WORD*)(neomenu+badblocks_offset);
    const int blocksize=neoctrl->getsize(neo_flash,block_size);
    const int numbblocks=neoctrl->getsize(neo_flash,total_size)/blocksize;
    int tbblocks=0;
    for(int i=0;i<numbblocks;i++)
    {
        tbblocks+=bblocks[i]==0?0:1;
    }
    totalFree-=tbblocks*blocksize;
    printf("\n\n");
    printf("\tused flash space: %s\n",print_size(totalUsed));
    printf("\tbad blocks: %s\n",print_size(tbblocks*blocksize));
    printf("\tfree flash space: %s\n",print_size(totalFree));
    printf("\tspace saved: %s\n",print_size(totalUsed+totalFree+tbblocks*blocksize-neoctrl->getsize(neo_flash,total_size)));
    printf("\n\n");
    state=state_ready;
    return state;
}

// #########################################################################

void neocart::setProgress(progress_indicator*sprg)
{
    if(state!=state_ready)return;
    prg=sprg;
}

// #########################################################################

selEntry*neocart::getrominfo(int index)
{
    if(index==0)
    {
        if(gMaxEntry)
        {
            quickSort(gSelections_index,0,gMaxEntry-1,gs_sortbyoffset,gSelections);
        }
    }
    int ind=0;
    for(int i=0;i<gMaxEntry;i++)
    {
        selEntry&gSelection=sorted(gSelections,i);
        if(gSelection.deleted==0)
        {
            if(ind==index)
            {
                return &gSelection;
            }
            ind++;
        }
    }
    return 0;
}

// #########################################################################

selEntry*neocart::getrominfo_byid(int romid)
{
    for(int i=0;i<gMaxEntry;i++)
    {
        if(gSelections[i].id==romid)
        {
            return &gSelections[i];
        }
    }
    return 0;
}

// #########################################################################

int neocart::getfreespace()
{
    int totalUsed=0;
    int totalFree=0;
    selEntry*gSelection,*gSelection_prev=0;
    for(int i=0;(gSelection=getrominfo(i));i++)
    {
        if(i==0)
        {
            totalFree+=gSelection->offset;
        }
        else
        {
            int gap=gSelection->offset-(gSelection_prev->offset+gSelection_prev->length);
            if(gap>0)
            {
                totalFree+=gap;
            }
        }
        totalUsed+=gSelection->length;
        gSelection_prev=gSelection;
    }
    int gap=neoctrl->getsize(neo_flash,total_size);
    if(gSelection_prev)
    {
        gap-=(gSelection_prev->offset+gSelection_prev->length);
    }
    if(gap>0)
    {
        totalFree+=gap;
    }
    const WORD*bblocks=(WORD*)(neomenu+badblocks_offset);
    const int blocksize=neoctrl->getsize(neo_flash,block_size);
    const int numbblocks=neoctrl->getsize(neo_flash,total_size)/blocksize;
    int tbblocks=0;
    for(int i=0;i<numbblocks;i++)
    {
        tbblocks+=bblocks[i]==0?0:1;
    }
    totalFree-=tbblocks*blocksize;
    totalFree=Max(totalFree,0);
    //printf("\tused flash space: %s\n",print_size(totalUsed));
    //printf("\tbad blocks: %s\n",print_size(tbblocks*blocksize));
    //printf("\tfree flash space: %s\n",print_size(totalFree));
    //printf("\tspace saved: %s\n",print_size(totalUsed+totalFree+tbblocks*blocksize-neoctrl->getsize(neo_flash,total_size)));
    return totalFree;
}

// #########################################################################

int neocart::addrom(const char*name,void*data,int fs)
{
    if(state!=state_ready)return 0;
//    if(fs>5*MB)
//    {
//        printf("\n   ***   error: invalid size recieved in line %i of %s\n",__LINE__,__FILE__);
//        fs=5*MB;
//    }

    int romtype=getRomType(data,fs);
    if(romtype==scd_multibios)
    {
        int offs=0;
        addrom("SegaCD MultiBIOS (Japan)",((BYTE*)data)+offs,128*KB);offs+=128*KB;
        addrom("SegaCD MultiBIOS (USA)",((BYTE*)data)+offs,128*KB);offs+=128*KB;
        addrom("SegaCD MultiBIOS (Europe)",((BYTE*)data)+offs,128*KB);offs+=128*KB;
        addrom("Columns (World)",((BYTE*)data)+offs,128*KB);offs+=128*KB;
        return 1;
    }

    alloctable*allt=new alloctable[gMaxEntry+1024+16];
    if(!allt)
    {
        return 0;
    }
    DWORD*allt_index=new DWORD[gMaxEntry+1024+16];
    if(!allt_index)
    {
        delete[] allt;
        return 0;
    }
    int allt_c=0;

    const int blocksize=neoctrl->getsize(neo_flash,ideal_block_size);
    const int neo_cart_size=neoctrl->getsize(neo_flash,total_size);
    const int fs_fix=Fix(fs,blocksize);

    allt[allt_c].offset=0;
    allt[allt_c].size=0;
    allt[allt_c].bblock=0;
    allt_c++;
    allt[allt_c].offset=neo_cart_size;
    allt[allt_c].size=0;
    allt[allt_c].bblock=1;
    allt_c++;

    for(int i=0;i<gMaxEntry;i++)
    {
        if(gSelections[i].deleted==0)
        {
            allt[allt_c].offset=gSelections[i].offset;
            allt[allt_c].size=gSelections[i].length;
            allt[allt_c].bblock=0;
            allt_c++;
        }
    }

    const WORD*bblocks=(WORD*)(neomenu+badblocks_offset);
    const int numbblocks=neo_cart_size/blocksize;

    if(romtype==n64_rom)//[n64]
    {
        //if there is a rom at first 32mb, it takes the rest of 32mb
        //if there is a rom at second 32mb, it takes the rest of the cart
        //bad blocks are ignored
        for(int i=0;i<gMaxEntry;i++)
        {
            if(gSelections[i].deleted==0)
            {
                if((gSelections[i].offset>=0)&&(gSelections[i].offset<32*MB))
                {
                    allt[allt_c].offset=gSelections[i].offset;
                    allt[allt_c].size=32*MB-gSelections[i].offset;
                    allt[allt_c].bblock=0;
                    allt_c++;
                }
                if(gSelections[i].offset>=32*MB)
                {
                    allt[allt_c].offset=gSelections[i].offset;
                    allt[allt_c].size=neo_cart_size-gSelections[i].offset;
                    allt[allt_c].bblock=0;
                    allt_c++;
                }
            }
        }
    }
    else
    {
        const int blocksize=neoctrl->getsize(neo_flash,block_size);
        for(int i=0;i<numbblocks;i++)
        {
            if(bblocks[i]!=0)
            {
                allt[allt_c].offset=i*blocksize;
                allt[allt_c].size=blocksize;
                allt[allt_c].bblock=1;
                allt_c++;
            }
        }
    }

    for(int i=0;i<allt_c;i++)
    {
        allt_index[i]=i;
    }
    quickSort(allt_index,0,allt_c-1,at_sortbyoffset,allt);
    for(int i=1;i<allt_c;i++)
    {
        alloctable&allt0=sorted(allt,i-1);
        alloctable&allt1=sorted(allt,i);
        if(!(((allt0.offset+allt0.size)<=allt1.offset)||((allt1.offset+allt1.size)<=allt0.offset)))
        {
            int rigthEdge=Max(allt0.offset+allt0.size,allt1.offset+allt1.size);
            allt0.size=rigthEdge-allt0.offset;
            allt1.size=rigthEdge-allt1.offset;
        }
    }

    int offset=-1;
    int mingap=neo_cart_size*2;

    for(int i=1;i<allt_c;i++)
    {
        alloctable&allt0=sorted(allt,i-1);
        alloctable&allt1=sorted(allt,i);
        const int alignment=Fix((romtype==sms_rom)?512*KB:getoption(romAlignment),blocksize);
        int leftEdge=Fix(allt0.offset+allt0.size,alignment);
        int gap=allt1.offset-leftEdge;
        int need_space=allt1.bblock?((fsz_tbl[getTableSize(fs_fix)]*table_block_size)?fsz_tbl[getTableSize(fs_fix)]*table_block_size:fs_fix):fs_fix;
        if(gap>=need_space)
        {
            if(gap<mingap)
            {
                offset=leftEdge;
                mingap=gap;
            }
        }
    }

    /*
    for(int i=0;i<allt_c;i++)
    {
        alloctable&allt0=sorted(allt,i);
        printf("offset=0x%.8x, size=0x%.8x\n",allt0.offset,allt0.size);
    }
    */

    delete[] allt;
    delete[] allt_index;

    if(offset==-1)
    {
        return -1;
    }

    if(romtype==smd_rom)
    {
        membuf romdata;
        int ns;
        if(decodeSmdRom(data,fs,romdata,0,ns))
        {
            fs=ns;
            romtype=getRomType(romdata,fs);
            if(!gSelections[gMaxEntry].romdata.resize(Fix(fs,blocksize)))
            {
                gSelections[gMaxEntry].romdata.free();
                return -1;
            }
            memcpy(gSelections[gMaxEntry].romdata,romdata,fs);
            //**/SaveFile("smd.bin",romdata,fs);
        }
        else
        {
            return -1;
        }
    }
    else
    {
        if(!gSelections[gMaxEntry].romdata.resize(fs_fix))
        {
            gSelections[gMaxEntry].romdata.free();
            return -1;
        }
        memcpy(gSelections[gMaxEntry].romdata,data,fs);
    }
    //s&k
    if(fs==2*MB)
    {
        if(compareHeaderString(gSelections[gMaxEntry].romdata,fs,0x180,"GM MK-1563 -00"))
        {
            membuf romdata;
            check(romdata.resize(2*MB+table_block_size));
            memset(romdata+2*MB,0,table_block_size);
            memcpy(romdata,gSelections[gMaxEntry].romdata,2*MB);
            gSelections[gMaxEntry].romdata.free();
            return addrom(name,romdata,2*MB+table_block_size);
        }
    }

    switch(romtype)
    {
    case md_rom:
        gSelections[gMaxEntry].type=0;
        gSelections[gMaxEntry].run=selectRunMode(0,6,fs,0,0);
        break;
    case sms_rom:
        gSelections[gMaxEntry].type=2;
        gSelections[gMaxEntry].run=0x13;
        break;
    case s32x_rom:
        gSelections[gMaxEntry].type=1;
        gSelections[gMaxEntry].run=selectRunMode(1,3,fs,0,0);
        break;
    case scd_bios:
        gSelections[gMaxEntry].type=0;
        gSelections[gMaxEntry].run=8;
        break;
    case scd_bram:
        gSelections[gMaxEntry].type=0;
        gSelections[gMaxEntry].run=9;
        break;
    case scd_bios_bram:
        gSelections[gMaxEntry].type=0;
        gSelections[gMaxEntry].run=10;
        break;
    case vgm_file:
        gSelections[gMaxEntry].type=4;
        gSelections[gMaxEntry].run=7;
        break;
    case n64_rom://[n64]
        gSelections[gMaxEntry].type=5;
        gSelections[gMaxEntry].run=0;
        gSelections[gMaxEntry].n64_cic=0;
        gSelections[gMaxEntry].n64_saveType=0;
        if(neoctrl->getsize(neo_menu,total_size)>(2*MB))
        {
	        gSelections[gMaxEntry].n64_modeA=offset==0?0:0x12;
		}
		else
		{
	        gSelections[gMaxEntry].n64_modeA=offset==0?0:0x14;
		}
#ifdef use_n64_plugin
        {
            SN64PLUG_Begin();
#if 0
            membuf romcopy;
            check(romcopy.resize(fs));
            memcpy(romcopy,gSelections[gMaxEntry].romdata,fs);

            if(SN64PLUG_ProcessImage(getFileName(name),(char*)(romcopy),fs))
#else
            if(SN64PLUG_ProcessImage(getFileName(name),(char*)(gSelections[gMaxEntry].romdata),fs))
#endif
            {
                int saveType,cic;
                if(SN64PLUG_Get(&cic,SN64PLUG_OP_CIC))
                {
                    if(cic==NEON64_CIC_6101)gSelections[gMaxEntry].n64_cic=1;
                    if(cic==NEON64_CIC_6102)gSelections[gMaxEntry].n64_cic=2;
                    if(cic==NEON64_CIC_6103)gSelections[gMaxEntry].n64_cic=3;
                    if(cic==NEON64_CIC_6105)gSelections[gMaxEntry].n64_cic=5;
                    if(cic==NEON64_CIC_6106)gSelections[gMaxEntry].n64_cic=6;
                }
                if(SN64PLUG_Get(&saveType,SN64PLUG_OP_SAVETYPE))
                {
                    if(saveType==NEON64_SAVE_EEPROM_4K)gSelections[gMaxEntry].n64_saveType=5;
                    if(saveType==NEON64_SAVE_EEPROM_16K)gSelections[gMaxEntry].n64_saveType=6;
                    if(saveType==NEON64_SAVE_SRAM_32K)gSelections[gMaxEntry].n64_saveType=1;
                    if(saveType==NEON64_SAVE_FLASHRAM_1M)gSelections[gMaxEntry].n64_saveType=4;
                }
            }
            SN64PLUG_End();
        }
#endif
        break;
    default:
        gSelections[gMaxEntry].type=0;
        gSelections[gMaxEntry].run=6;
        break;
    };

    gSelections_index[gMaxEntry]=gMaxEntry;
    gSelections[gMaxEntry].id=gRomId++;
    gSelections[gMaxEntry].dirty=1;
    gSelections[gMaxEntry].deleted=0;
    gSelections[gMaxEntry].bbank=0;
    gSelections[gMaxEntry].bsize=0;
    gSelections[gMaxEntry].offset=offset;
    gSelections[gMaxEntry].length=fs;
    name=getShortName(name);
    memcpy(gSelections[gMaxEntry].name,name,24);
    gSelections[gMaxEntry].name[23]=0;
    gMaxEntry++;

    int even,bsize=getSaveSize(romtype,gSelections[gMaxEntry-1].romdata,fs,even);//drop even
    if(bsize>0)
    {
        linksram(gRomId-1,0,bsize);
    }

    return gRomId-1;
}

// #########################################################################

void neocart::replace_menu(void*data,int fs)
{
    if(state!=state_ready)return;
    menu_size=Min(fs,0xa0000);
    menu_size=Fix(menu_size,neoctrl->getsize(neo_menu,block_size));
    memcpy(neomenu,data,Min(fs,menu_size));
}

// #########################################################################

int neocart::delrom(char*name)
{
    if(state!=state_ready)return 0;
    int retval=0;
    if(strcmp(name,"sms")==0)
    {
        for(int i=0;i<gMaxEntry;i++)
        {
            if(gSelections[i].type==2)
            {
                retval+=delrom(gSelections[i].id);
            }
        }
        return retval;
    }
    for(int i=0;i<gMaxEntry;i++)
    {
        if(testPathMask(gSelections[i].name,name)==0)
        {
            retval+=delrom(gSelections[i].id);
        }
    }
    return retval;
}

// #########################################################################

int neocart::delrom(int romid)
{
    if(state!=state_ready)return 0;
    if(romid<=0||romid>=gRomId)
    {
        return 0;
    }
    for(int i=0;i<gMaxEntry;i++)
    {
        if(gSelections[i].id==romid&&gSelections[i].deleted==0)
        {
            gSelections[i].deleted=1;
            gSelections[i].dirty=0;
            gSelections[i].romdata.free();
            return 1;
        }
    }
    return 0;
}

// #########################################################################

int neocart::backupsram(char*name)
{
    if(state!=state_ready)return 0;
    int retval=0;
    for(int i=0;i<gMaxEntry;i++)
    {
        if(testPathMask(gSelections[i].name,name)==0)
        {
            retval+=backupsram(gSelections[i].id,0);
        }
    }
    return retval;
}

// #########################################################################

int neocart::backupsram(int romid,char*fname)
{
    if(state!=state_ready)return 0;
    for(int i=0;i<gMaxEntry;i++)
    {
        if(gSelections[i].id==romid&&gSelections[i].type==5)//[n64]
        {
            int offset=gSelections[i].offset==0?0:128*KB;
            int stype=getn64SramSize(gSelections[i].n64_saveType);
            int size=stype;
            if(size==0)
            {
                return 0;
            }
            if(size<=st_n64eep16k)
            {
                size*=16;
            }
            membuf sramdata;
            check(sramdata.resize(size));
            check(n64sram.read(sramdata,offset,size))
            {
                membuf sramdata_w;
                convertSave2n64(stype,(BYTE*)(char*)sramdata,sramdata_w,size);
                memcpy(sramdata,sramdata_w,size);
            }
            if(fname==0)
            {
                char bname[1024];
                if(stype<=st_n64eep16k)
                {
                    sprintf(bname,"%s.eep",gSelections[i].name);
                }
                else
                {
                    sprintf(bname,"%s.sra",gSelections[i].name);
                }
                SaveFile(bname,sramdata,size);
            }
            else
            {
                SaveFile(fname,sramdata,size);
            }
            return 1;
        }
        if(gSelections[i].id==romid&&gSelections[i].bsize>0)
        {
            int bbank=gSelections[i].bbank;
            int bsize=gSelections[i].bsize;
            int rs=bsize*save_block_size;
            membuf sramdata;
            check(sramdata.resize(rs));
            check(sram.read(sramdata,bbank*bsize*save_block_size,rs))
            if(gSelections[i].type!=2&&gSelections[i].run<8)
            {
                membuf sramdata_w;
                check(sramdata_w.resize(rs*2));
                for(int j=0;j<rs;j++)
                {
                    sramdata_w[j*2+0]=0;
                    sramdata_w[j*2+1]=sramdata[j];
                }
                rs*=2;
                check(sramdata.resize(rs));
                memcpy(sramdata,sramdata_w,rs);
            }
            if(fname==0)
            {
                char bname[1024];
                if(gSelections[i].type==2)
                {
                    sprintf(bname,"%s.ssm",gSelections[i].name);
                }
                else
                {
                    sprintf(bname,"%s.srm",gSelections[i].name);
                }
                SaveFile(bname,sramdata,rs);
            }
            else
            {
                SaveFile(fname,sramdata,rs);
            }
            return 1;
        }
    }
    return 0;
}

// #########################################################################

int neocart::backuprom(char*name)
{
    if(state!=state_ready)return 0;
    int retval=0;
    for(int i=0;i<gMaxEntry;i++)
    {
        if(testPathMask(gSelections[i].name,name)==0)
        {
            retval+=backuprom(gSelections[i].id,0);
        }
    }
    return retval;
}

// #########################################################################

int neocart::backuprom(int romid,char*fname)
{
    if(state!=state_ready)return 0;
    const int blocksize=neoctrl->getsize(neo_flash,block_size);
    for(int i=0;i<gMaxEntry;i++)
    {
        if(gSelections[i].id==romid&&gSelections[i].length>0)
        {
            int size=Fix(gSelections[i].length,blocksize);
            membuf romdata;
            check(romdata.resize(size));
            if(gSelections[i].dirty==0)
            {
                const int rblocksize=Fix(getoption(ioBlockSize),blocksize);
                int offs=0;
                while(size)
                {
                    int tr=Min(size,rblocksize);
                    check(neoctrl->read(neo_flash,gSelections[i].offset+offs,romdata+offs,tr));
                    size-=tr;
                    offs+=tr;
                }
                size=gSelections[i].length;
            }
            else
            {
                size=gSelections[i].length;
                memcpy(romdata,gSelections[i].romdata,size);
            }
            if(fname==0)
            {
                char rname[1024];
                sprintf(rname,"%s.bin",gSelections[i].name);
                SaveFile(rname,romdata,size);
            }
            else
            {
                SaveFile(fname,romdata,size);
            }
            return 1;
        }
    }
    return 0;
}

// #########################################################################

int neocart::initsram(int romid)
{
    selEntry*gSelection=getrominfo_byid(romid);
    if(gSelection&&gSelection->deleted==0)
    {
        if(gSelection->type==5)//[n64]
        {
            int offset=gSelection->offset==0?0:128*KB;
            int size=getn64SramSize(gSelection->n64_saveType);
            if(size==0)
            {
                return 0;
            }
            if(size<=st_n64eep16k)
            {
                size*=16;
            }
            membuf sramdata;
            check(sramdata.resize(size));
            memset(sramdata,0,size);
            check(n64sram.write(sramdata,offset,size));
            return 1;
        }
        if(gSelection->bsize)
        {
            int rs=gSelection->bsize*save_block_size;
            membuf sramdata;
            check(sramdata.resize(rs));
            memset(sramdata,0,rs);
            check(sram.write(sramdata,gSelection->bbank*gSelection->bsize*save_block_size,rs));
            return 1;
        }
    }
    return 0;
}

// #########################################################################

int neocart::linksram(int romid,void*data,int ss)
{
    if(state!=state_ready)return 0;
    selEntry*gSelection=0;
    for(int i=0;i<gMaxEntry;i++)
    {
        if(gSelections[i].id==romid&&gSelections[i].deleted==0)
        {
            gSelection=&gSelections[i];
            break;
        }
    }
    if(!gSelection)
    {
        return 0;
    }

    if(gSelection->type==5)//[n64]
    {
        int offset=gSelection->offset==0?0:128*KB;
        int stype=getn64SramSize(gSelection->n64_saveType);
        int ssize=stype;
        if(ssize==0)
        {
            return 0;
        }
        if(ssize<=st_n64eep16k)
        {
            ssize*=16;
        }
        membuf sramdata;
        check(sramdata.resize(ssize));
        memset(sramdata,0,ssize);
        convertSave2neo(stype,(BYTE*)data,sramdata,ss);
        check(n64sram.write(sramdata,offset,ssize));
        return 1;
    }

    int rs=Fix(ss,save_block_size);
    if(gSelection->type!=2&&gSelection->run<8)
    {
        rs/=2;
    }
    int bsize=save_block_size;
    while(bsize<rs)
    {
        bsize*=2;
    }
    bsize=bsize/save_block_size;
    int bbank;

    if(ss==1)
    {
        for(int i=0;i<gMaxEntry;i++)
        {
            if(gSelections[i].id!=romid&&gSelections[i].deleted==0)
            {
                if(gSelections[i].run==5)
                {
                    printf("   *** warning: eeprom save detected for %s, but there is already a eeprom rom on the cart\n",gSelection->name);
                    return 0;
                }
            }
        }
        gSelection->bbank=0;
        gSelection->bsize=0;
        gSelection->run=selectRunMode(gSelection->type,gSelection->run,gSelection->length,1,0);
        return 1;
    }

    if(gSelection->bsize>=bsize)
    {
        bbank=gSelection->bbank;
    }
    else
    {
        int sbank=-1;
        for(int b=0;b<16;b++)
        {
            if(((b+1)*bsize*save_block_size)>(int)neoctrl->getsize(neo_sram,total_size))
            {
                break;
            }
            int free=1;
            for(int i=0;i<gMaxEntry;i++)
            {
                if(gSelections[i].id!=romid&&gSelections[i].deleted==0)
                {
                    if(gSelections[i].bsize>0)
                    {
                        int sz0=bsize;
                        int ofs0=b*sz0;
                        int sz1=gSelections[i].bsize;
                        int ofs1=gSelections[i].bbank*sz1;
                        if(!(((ofs0+sz0)<=ofs1)||((ofs1+sz1)<=ofs0)))
                        {
                            free=0;
                        }
                    }
                }
            }
            if(free)
            {
                sbank=b;
                if(bsize==1)
                {
                    break;
                }
            }
        }
        if(sbank==-1)
        {
            printf("   *** warning: sram(%iKB) save detected for %s, but there is no more free sram\n",bsize*8,gSelection->name);
            return 0;
        }
        bbank=sbank;
    }

    gSelection->bbank=bbank;
    gSelection->bsize=bsize;
    gSelection->run=selectRunMode(gSelection->type,gSelection->run,gSelection->length,0,gSelection->bsize);
    if(!data)
    {
        return 1;
    }

    membuf sramdata;
    check(sramdata.resize(rs));
    if(gSelection->type!=2&&gSelection->run<8)
    {
        char*src=(char*)data;
        for(int i=0;i<ss;i+=2)
        {
            sramdata[i/2]=src[i+1];
        }
    }
    else
    {
        memcpy(sramdata,data,rs);
    }
    check(sram.write(sramdata,bbank*bsize*save_block_size,rs));

    return 1;
}

// #########################################################################

int neocart::burn()
{
    if(state!=state_ready)return 0;

    if(gMaxEntry)
    {
        quickSort(gSelections_index,0,gMaxEntry-1,gs_sortbyoffset,gSelections);
    }

    const int blocksize=neoctrl->getsize(neo_flash,ideal_block_size);

    progress_indicator*subprg=prg->subProgress();
    subprg->setText("...");
    prg->setWork(0);
    subprg->setWork(0);
    prg->reset();
    subprg->reset();
    for(int i=0;i<gMaxEntry;i++)
    {
        if(gSelections[i].dirty!=0&&gSelections[i].romdata!=0)
        {
            int size=Fix(gSelections[i].length,blocksize);
            prg->incWork(size);
        }
    }
    prg->incWork(menu_size);
    neoctrl->setProgress(subprg?subprg:prg);

    sram.burn();

    for(int i=0;i<gMaxEntry;i++)
    {
        selEntry&gSelection=sorted(gSelections,i);
        if(gSelection.dirty!=0&&gSelection.romdata!=0)
        {
            //SaveFile("debug.bin",gSelection.romdata,gSelection.length);
            printf("\t\twriting\t%s\ntype=%i, run=%i, bbank=%.2u, bsize=%.4u, offset=0x%.8x, size=%s\n\n",gSelection.name,gSelection.type,gSelection.run,gSelection.bbank,gSelection.bsize,gSelection.offset,print_size(gSelection.length));

            int wblocksize=Fix(getoption(ioBlockSize),blocksize);
            if(gSelection.type==5)//[n64]
            {
                wblocksize=blocksize;
            }
            int sucess=1;
            int size=Fix(gSelection.length,blocksize);
            int offs=0;
            subprg->setText(gSelection.name);
            subprg->setWork(size);
            subprg->reset();
            while(size)
            {
                int tw=Min(size,wblocksize);
                prg->wait();
                if(!prg->doWork(0))
                {
                    return 0;
                }
                if(neoctrl->write(neo_flash,gSelection.offset+offs,gSelection.romdata+offs,tw)==0)
                {
                    if(gSelection.type!=5)//[n64]
                    {
                        sucess=0;
                        size=tw;
                    }
                }
                size-=tw;
                offs+=tw;
            }
            /*wrong*/prg->doWork(subprg?Fix(gSelection.length,blocksize):0);
            char desc[1024];
            if(sucess)
            {
                sprintf(desc,"%s : ok",gSelection.name);
            }
            else
            {
                gSelection.deleted=1;
                sprintf(desc,"   *** error: write failed for %s",gSelection.name);
            }
            subprg->done(desc);
            printf(desc);
            printf("\n");
            gSelection.dirty=0;
            gSelection.romdata.free();
        }
    }

    if(gMaxEntry)
    {
        quickSort(gSelections_index,0,gMaxEntry-1,gs_sortbyname,gSelections);
    }

    menuEntry*p=(menuEntry*)(neomenu+menu_offset);
    for(int i=0;i<gMaxEntry;i++)
    {
        selEntry&gSelection=sorted(gSelections,i);
        if(gSelection.deleted)
        {
            gSelection.dirty=0;
            continue;
        }
        p->meValid=0;
        p->meType=gSelection.type;
        p->meRun=gSelection.run;
        char srams=0;
        int bsize=gSelection.bsize;
        if(bsize!=0)
        {
            srams=1;
            while((!(bsize&1))&&srams<0xF)
            {
                bsize=bsize>>1;
                srams++;
            }
        }
        p->meSRAM=((gSelection.bbank&0xF)<<4)|srams;
        p->meROMLo=(gSelection.offset>>17)&0xFF;


        if(gSelection.type==5)//[n64]
        {
            p->meROMHi=((gSelection.offset>>25)&0x0F);

            int dn64l=Fix(gSelection.length,128*KB);
            dn64l/=128*KB;
            WORD n64l=dn64l&0xffff;
            store_word_le(n64l,p->meName);

            memcpy(p->meName+2,gSelection.name,22);
        }
        else
        {
            int rhi=getTableSize(gSelection.length);
            p->meROMHi=((gSelection.offset>>25)&0x0F)|(rhi<<4);

            memcpy(p->meName,gSelection.name,24);
        }
        p++;
    }
    p->meValid=0xFF;

    subprg->setText("menu");
    subprg->setWork(menu_size);
    subprg->reset();
    int retval=0;
    printf("\t\twriting\tmenu\n");
    if(neoctrl->write(neo_menu,0,neomenu,menu_size))
    {
        //DWORD fs;
        //LoadFile("menu_820.bin",neomenu+0xA0000,fs);
        //neoctrl->write(neo_menu,0xA0000,neomenu,64*KB);
        retval=1;
    }

    int writen64=0;//[n64]
    for(int i=0;i<gMaxEntry;i++)
    {
        if(gSelections[i].type==5&&gSelections[i].deleted==0)
        {
            writen64=1;
        }
    }
    if(writen64)
    {
        subprg->incWork(64*KB);
        const int n64offset=31*64*KB;
        memset(neomenu+n64offset,0,64*KB);
        memcpy(neomenu+2*MB-32,"\xf6\xf6\xf6\xf6\xf6\xf6\xf6\xf6\xf6\xf6\xf6\xf6\xf6\xf6\xf6\xf6",16);
        if(neoctrl->getsize(neo_menu,total_size)>(2*MB))
        {
        	neomenu[2*MB-16+0]=0;
        	neomenu[2*MB-16+1]=0;
		}
		else
        {
        	neomenu[2*MB-16+0]=1;
        	neomenu[2*MB-16+1]=1;
		}
        for(int i=0;i<gMaxEntry;i++)
        {
            if(gSelections[i].type==5&&gSelections[i].deleted==0)
            {
                BYTE*val=(BYTE*)(gSelections[i].offset==0?&neomenu[2*MB-16+2]:&neomenu[2*MB-16+4]);
                val[0]=_2byte((gSelections[i].n64_cic&0xf)|((gSelections[i].n64_saveType<<4)&0xf0));
                val[1]=_2byte(gSelections[i].n64_modeA);
                val=(BYTE*)(gSelections[i].offset==0?&neomenu[0x1fc800]:&neomenu[0x1fc840]);
                val[0]=0xff;
                val[1]=0x05;
                const short int n64_tbl[16]={0,0,0,0,0,0,1,2,4,8,16,32,64,128,256,512};
                int size=0x0c;
                for(int j=0;j<16;j++)
                {
                    if(n64_tbl[j]>=(gSelections[i].length/(128*KB)))
                    {
                        size=j;
                        break;
                    }
                }
                val[2]=_2byte(size<<4);
                val[3]=gSelections[i].offset==0?0:0x80;
                memcpy(val+0x0c,gSelections[i].name,24);
            }
        }
        neoctrl->write(neo_menu,n64offset,neomenu+n64offset,64*KB);
        n64sram.burn();
    }

    subprg->done(0);
    prg->done(0);
    neoctrl->setProgress(0);

    return retval;
}

// #########################################################################

int neocart::format()
{
    if(state!=state_ready)return 0;

    gMaxEntry=0;
    menuEntry*p=(menuEntry*)(neomenu+menu_offset);
    p->meValid=0xFF;

    WORD*bblocks=(WORD*)(neomenu+badblocks_offset);
    const int blocksize=neoctrl->getsize(neo_flash,block_size);
    const int numbblocks=neoctrl->getsize(neo_flash,total_size)/blocksize;
    membuf wblock;
    check(wblock.resize(blocksize));
    srand(time(0));
    md5struct md5b;
    md5struct md5a;
    int tbblocks=0;

    progress_indicator*subprg=prg->subProgress();
    subprg->setText("...");
    prg->setWork(0);
    subprg->setWork(0);
    prg->reset();
    subprg->reset();
    prg->incWork(numbblocks*blocksize*2+badblocks_size);
    neoctrl->setProgress(subprg?subprg:prg);

    for(int i=0;i<numbblocks;i++)
    {
        bblocks[i]=1;
        for(int j=0;j<blocksize;j++)
        {
            wblock[j]=_2byte(rand()%0x100);
        }
        getmd5(wblock,blocksize,md5b);
        {
            char desc[128];
            sprintf(desc,"block %.4i",i+1);
            subprg->setText(desc);
            subprg->setWork(blocksize*2);
            subprg->reset();
            prg->wait();
            if(!prg->doWork(0))
            {
                return 0;
            }
        }
        if(neoctrl->write(neo_flash,i*blocksize,wblock,blocksize))
        {
            memset(wblock,0,blocksize);
            if(neoctrl->read(neo_flash,i*blocksize,wblock,blocksize))
            {
                getmd5(wblock,blocksize,md5a);
                if(memcmp(md5b,md5a,16)==0)
                {
                    bblocks[i]=0;
                }
            }
        }
        tbblocks+=bblocks[i];
        printf("\tblock %.4i of %.4i %s, total bad blocks: %.4i,\t%i complete\n",i+1,numbblocks,bblocks[i]?"failed":"passed",tbblocks,(i*100)/numbblocks);
        {
            prg->doWork(subprg?blocksize*2:0);
            char desc[128];
            sprintf(desc,"block %.4i %s",i+1,bblocks[i]?"failed":"passed");
            subprg->done(desc);
        }
    }

    int retval=numbblocks;

    subprg->setText("menu");
    subprg->setWork(menu_size+badblocks_size);
    subprg->reset();

    if(neoctrl->write(neo_menu,0,neomenu,menu_size))
    {
        if(neoctrl->write(neo_menu,badblocks_offset,neomenu+badblocks_offset,badblocks_size))
        {
            retval=tbblocks;
        }
    }

    subprg->done(0);
    prg->done(0);
    neoctrl->setProgress(0);

    return retval;
}

// #########################################################################

int neocart::format(int romid)
{
    if(state!=state_ready)return 0;
    if(romid<=0||romid>=gRomId)
    {
        return 0;
    }
    selEntry*gSelection=0;
    for(int i=0;i<gMaxEntry;i++)
    {
        if(gSelections[i].id==romid&&gSelections[i].deleted==0)
        {
            gSelection=&gSelections[i];
            gSelections[i].deleted=1;
            gSelections[i].dirty=0;
            gSelections[i].romdata.free();
        }
    }
    if(gSelection==0)
    {
        return 0;
    }
    WORD*bblocks=(WORD*)(neomenu+badblocks_offset);
    const int blocksize=neoctrl->getsize(neo_flash,block_size);
    const int numbblocks=gSelection->length/blocksize;
    const int shift=gSelection->offset/blocksize;
    membuf wblock;
    check(wblock.resize(blocksize));
    srand(time(0));
    md5struct md5b;
    md5struct md5a;
    int tbblocks=0;
    for(int i=shift;i<shift+numbblocks;i++)
    {
        bblocks[i]=1;
        for(int j=0;j<blocksize;j++)
        {
            wblock[i]=_2byte(rand()%0x100);
        }
        getmd5(wblock,blocksize,md5b);
        if(neoctrl->write(neo_flash,i*blocksize,wblock,blocksize))
        {
            memset(wblock,0,blocksize);
            for(int j=0;j<3;j++)
            {
                if(neoctrl->read(neo_flash,i*blocksize,wblock,blocksize))
                {
                    getmd5(wblock,blocksize,md5a);
                    if(memcmp(md5b,md5a,16)==0)
                    {
                        if(j==2)
                        {
                            bblocks[i]=0;
                        }
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }
        tbblocks+=bblocks[i];
        printf("\tblock %.4i of %.4i %s, total bad blocks: %.4i,\t%i complete\n",i+1,numbblocks,bblocks[i]?"failed":"passed",tbblocks,((i-shift)*100)/numbblocks);
    }
    return 1;
    if(neoctrl->write(neo_menu,badblocks_offset,neomenu+badblocks_offset,badblocks_size))
    {
        return tbblocks;
    }
    return numbblocks;
}

// #########################################################################

void neocart::close()
{
    if(state==state_undefined)return;
    sram.close();
    if(gSelections)
    {
        delete[] gSelections;
        gSelections=0;
    }
    if(gSelections_index)
    {
        delete[] gSelections_index;
        gSelections_index=0;
    }
    state=state_undefined;
}

// #########################################################################
