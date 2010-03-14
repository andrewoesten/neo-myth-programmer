// #########################################################################

#include <neoutil.h>
#include <neodriver.h>
#include <neocontrol.h>
#include <neocart.h>
#include <plugins.h>

// #########################################################################

neocontrol*neoctrl;
neocart*ncart;

// #########################################################################

const char *neoloc[neo_loc_count]=
{
    "menu",
    "flash",
    "sram",
    "psram (doesn't work)",
    "cache (doesn't work)",
    "n64menu",
    "n64sram"
};

// #########################################################################

const char*help_info=
    "\n"
    "Usage: %s [<command>] [<file_names>/mask]\n"
    "\n"
    "<Commands>\n"
    "  --cartid,-id                    : save cartid.bin file with id data\n"
    "  --delete,-d <number/mask>       : delete rom from flash\n"
    "  --backup-sram,-bs <number/mask> : save sram to disk\n"
    "  --backup-rom,-br <number/mask>  : save rom to disk\n"
    "  --sram,-s <filename>            : load sram from disk\n"
    "  --menu,-m <filename>            : load boot rom from disk\n"
    "  --format                        : format cart and mark bad blocks\n"
    "  --auto-patch,-ap                : enable known patches for specific\n"
    "                                    non-working roms(ssf2, cdbios region check)\n"
    "  --save-patched,-sp              : save patched files to disk\n"
    "  --alignment,-al                 : rom alignment (default: 512KB)\n"
    "\n"
    "<raw io>\n"
    "  --iotarget,-iot <menu/flash/sram/n64menu/n64sram> : set io target\n"
    "                                    (default: flash)\n"
    "  --offset,-of <value>            : set io offset (default: 0)\n"
    "  --size,-sz <value>              : set io size (default: total size)\n"
    "  --read,-r <filename>            : read to file\n"
    "  --write,-w <filename>           : write from file\n"
    "\n"
    "<number/mask>\n"
    "  either rom number or mask, i.e. -d \"[bios]*\"\n"
    "\n";

// #########################################################################

void print_help(const char*fname)
{
    static int h=0;
    if(h==0)
    {
        h=1;
        printf(help_info,fname);
    }
}

// #########################################################################

enum
{
    raw_read,
    raw_write
};

// #########################################################################

int raw_io_block(neocontrol*neoctrl,int op,int offset,int size,int location,char*data)
{
    const int blocksize=neoctrl->getsize(location,block_size);
    const int ioblocksize=Fix(getoption(ioBlockSize),blocksize);
    const int nblocks=ioblocksize/blocksize;
    int sucess=1;
    int offs=0;
    int rsize=size;
    while(rsize)
    {
        int t=Min(rsize,ioblocksize);
        int r=-1;
        if(op==raw_read)
        {
            r=neoctrl->read(location,offset+offs,data+offs,t);
            printf("\t%s: read\t%i of\t%i\n",neoloc[location],size-rsize+t,size);
        }
        if(op==raw_write)
        {
            r=neoctrl->write(location,offset+offs,data+offs,t);
            printf("\t%s: write\t%i of\t%i\n",neoloc[location],size-rsize+t,size);
        }
        if(r==0)
        {
            if(nblocks>1)
            {
                for(int i=0;i<nblocks;i++)
                {
                    if(op==raw_read)
                    {
                        if(neoctrl->read(location,offset+offs+i*blocksize,data+offs+i*blocksize,blocksize)==0)
                        {
                            memset(data+offs+i*blocksize,0xff,blocksize);
                        }
                        printf("\t%s: read\t%i of\t%i\n",neoloc[location],size-rsize+(i+1)*blocksize,size);
                    }
                    if(op==raw_write)
                    {
                        neoctrl->write(location,offset+offs+i*blocksize,data+offs+i*blocksize,blocksize);
                        printf("\t%s: write\t%i of\t%i\n",neoloc[location],size-rsize+(i+1)*blocksize,size);
                    }
                }
            }
        }
        rsize-=t;
        offs+=t;
    }
    return sucess;
}

// #########################################################################

int raw_io(neocontrol*neoctrl,int op,int raw_offset,int raw_size,int raw_location,char*fname)
{
    int blocksize=neoctrl->getsize(raw_location,block_size);
    check(neoctrl->getsize(raw_location,block_size));
    int totalsize=neoctrl->getsize(raw_location,total_size);
    check(neoctrl->getsize(raw_location,total_size));
    raw_offset=Max(0,raw_offset);
    raw_size=Max(0,raw_size);
    if((raw_size==0)||((raw_size+raw_offset)>totalsize))
    {
        raw_size=totalsize-raw_offset;
    }
    if(raw_size<1)
    {
        printf("invalid location\n");
        return 0;
    }
    int offset=raw_offset-raw_offset%blocksize;
    int size=Fix(raw_size+raw_offset-offset,blocksize);
    size=Min(size,totalsize-offset);
    membuf buffer;
    if(op==raw_read)
    {
        check(buffer.resize(size));
        raw_io_block(neoctrl,op,offset,size,raw_location,buffer);
        check(SaveFile(fname,buffer+raw_offset-offset,raw_size));
        return 1;
    }
    if(op==raw_write)
    {
        DWORD fs;
        LoadFile(fname,buffer,fs);
        if(fs==0)
        {
            printf("cannot load file: %s\n",fname);
            return 0;
        }
        raw_size=Min(raw_size,(int)fs);
        size=Fix(raw_size+raw_offset-offset,blocksize);
        size=Min(size,totalsize-offset);
        membuf rbuffer;
        check(rbuffer.resize(size));
        if(raw_offset!=offset)
        {
            neoctrl->read(raw_location,offset,rbuffer,blocksize);
        }
        if((raw_offset+raw_size)!=(offset+size))
        {
            if(size!=blocksize)
            {
                neoctrl->read(raw_location,offset+size-blocksize,rbuffer+size-blocksize,blocksize);
            }
        }
        memcpy(rbuffer+raw_offset-offset,buffer,raw_size);
        raw_io_block(neoctrl,op,offset,size,raw_location,rbuffer);
        return 1;
    }
    return 0;
}

// #########################################################################

int processCommandLine(int argc,char* argv[])
{
    /*
    printf("\n");
    for(int i=0;i<argc;i++)
    {
        printf("\targv[%i]==\"%s\"\n",i,argv[i]);
    }
    printf("\n");
    printf("\ttestPathMask(\"%s\",\"%s\")==%i\n",argv[1],argv[2],testPathMask(argv[1],argv[2])==0);
    //*/

    membuf buffer;
    DWORD fs;
    DWORD nerr=0;
    DWORD write=0;
    int lastromid=-1;
    int raw_offset[neo_loc_count];
    memset(raw_offset,0,sizeof(raw_offset));
    int raw_size[neo_loc_count];
    memset(raw_size,0,sizeof(raw_size));
    int raw_location=neo_flash;
    for(int i=1;i<argc&&nerr==0;i++)
    {
        if(file_exists(argv[i]))
        {
            LoadFile(argv[i],buffer,fs);
            if(fs==0)
            {
                printf("cannot load file: %s\n",argv[i]);
                nerr=1;
                break;
            }
            int romid=ncart->addrom(argv[i],buffer,fs);
            if(romid<0)
            {
                printf("no more space on device: %s\n",argv[i]);
            }
            else
            {
                write=1;
                lastromid=romid;
            }
            continue;
        }
        if(strcmp(argv[i],"--help")==0||strcmp(argv[i],"-h")==0)
        {
            continue;
        }
        if(strcmp(argv[i],"--cartid")==0||strcmp(argv[i],"-id")==0)
        {
            continue;
        }
        if(strcmp(argv[i],"--delete")==0||strcmp(argv[i],"-d")==0)
        {
            i++;
            if(i>=argc)
            {
                printf("no parameter: %s\n",argv[i-1]);
                nerr=1;
                break;
            }
            int err;
            int romid=atoiX(argv[i],0,err);
            err=err?ncart->delrom(argv[i]):ncart->delrom(romid);
            if(err>0)
            {
                write=1;
            }
            else
            {
                printf("no rom found: %s\n",argv[i]);
                nerr=1;
                break;
            }
            continue;
        }
        if(strcmp(argv[i],"--backup-sram")==0||strcmp(argv[i],"-bs")==0)
        {
            i++;
            if(i>=argc)
            {
                printf("no parameter: %s\n",argv[i-1]);
                nerr=1;
                break;
            }
            int err;
            int romid=atoiX(argv[i],0,err);
            err=err?ncart->backupsram(argv[i]):ncart->backupsram(romid,0);
            if(err<=0)
            {
                printf("no rom found: %s\n",argv[i]);
                nerr=1;
                break;
            }
            continue;
        }
        if(strcmp(argv[i],"--backup-rom")==0||strcmp(argv[i],"-br")==0)
        {
            i++;
            if(i>=argc)
            {
                printf("no parameter: %s\n",argv[i-1]);
                nerr=1;
                break;
            }
            int err;
            int romid=atoiX(argv[i],0,err);
            err=err?ncart->backuprom(argv[i]):ncart->backuprom(romid,0);
            if(err<=0)
            {
                printf("no rom found: %s\n",argv[i]);
                nerr=1;
                break;
            }
            continue;
        }
        if(strcmp(argv[i],"--sram")==0||strcmp(argv[i],"-s")==0)
        {
            i++;
            if(i>=argc)
            {
                printf("no parameter: %s\n",argv[i-1]);
                nerr=1;
                break;
            }
            if(lastromid>=0)
            {
                LoadFile(argv[i],buffer,fs);
                if(fs==0)
                {
                    printf("cannot load file: %s\n",argv[i]);
                    nerr=1;
                    break;
                }
                if(ncart->linksram(lastromid,buffer,fs))
                {
                    write=1;
                }
                else
                {
                    printf("not enough sram: %s\n",argv[i]);
                }
            }
            else
            {
                printf("no rom loaded: %s %s\n",argv[i-1],argv[i]);
                nerr=1;
                break;
            }
            continue;
        }
        if(strcmp(argv[i],"--menu")==0||strcmp(argv[i],"-m")==0)
        {
            i++;
            if(i>=argc)
            {
                printf("no parameter: %s\n",argv[i-1]);
                nerr=1;
                break;
            }
            LoadFile(argv[i],buffer,fs);
            if(fs==0)
            {
                printf("cannot load file: %s\n",argv[i]);
                nerr=1;
                break;
            }
            ncart->replace_menu(buffer,fs);
            write=1;
            continue;
        }
        if(strcmp(argv[i],"--format")==0)
        {
            printf("formatting cart...\n");
            int tbblocks=ncart->format();
            printf("\ttotal bad blocks: %i\n",tbblocks);
            write=1;
            continue;
        }
        if(strcmp(argv[i],"--auto-patch")==0||strcmp(argv[i],"-ap")==0)
        {
            int o_enableKnownPatches=1;
            setoption(enableKnownPatches,&o_enableKnownPatches);
            continue;
        }
        if(strcmp(argv[i],"--save-patched")==0||strcmp(argv[i],"-sp")==0)
        {
            int o_savePatchedFiles=1;
            setoption(savePatchedFiles,&o_savePatchedFiles);
            continue;
        }
        if(strcmp(argv[i],"--alignment")==0||strcmp(argv[i],"-al")==0)
        {
            i++;
            if(i>=argc)
            {
                printf("no parameter: %s\n",argv[i-1]);
                nerr=1;
                break;
            }
            int err;
            int o_romAlignment=atoiX(argv[i],0,err);
            if(err==0)
            {
                setoption(romAlignment,&o_romAlignment);
                continue;
            }
        }
        if(strcmp(argv[i],"--iotarget")==0||strcmp(argv[i],"-iot")==0)
        {
            i++;
            if(i>=argc)
            {
                printf("no parameter: %s\n",argv[i-1]);
                nerr=1;
                break;
            }
            int err=1;
            int location;
            for(int j=0;j<neo_loc_count;j++)
            {
                if(strcmp(argv[i],neoloc[j])==0)
                {
                    err=0;
                    location=j;
                    break;
                }
            }
            if(err==0)
            {
                raw_location=location;
                continue;
            }
        }
        if(strcmp(argv[i],"--offset")==0||strcmp(argv[i],"-of")==0)
        {
            i++;
            if(i>=argc)
            {
                printf("no parameter: %s\n",argv[i-1]);
                nerr=1;
                break;
            }
            int err;
            int offset=atoiX(argv[i],0,err);
            if(err==0)
            {
                raw_offset[raw_location]=offset;
                continue;
            }
        }
        if(strcmp(argv[i],"--size")==0||strcmp(argv[i],"-sz")==0)
        {
            i++;
            if(i>=argc)
            {
                printf("no parameter: %s\n",argv[i-1]);
                nerr=1;
                break;
            }
            int err;
            int size=atoiX(argv[i],0,err);
            if(err==0)
            {
                raw_size[raw_location]=size;
                continue;
            }
        }
        if(strcmp(argv[i],"--read")==0||strcmp(argv[i],"-r")==0)
        {
            i++;
            if(i>=argc)
            {
                printf("no parameter: %s\n",argv[i-1]);
                nerr=1;
                break;
            }
            raw_io(neoctrl,raw_read,raw_offset[raw_location],raw_size[raw_location],raw_location,argv[i]);
            continue;
        }
        if(strcmp(argv[i],"--write")==0||strcmp(argv[i],"-w")==0)
        {
            i++;
            if(i>=argc)
            {
                printf("no parameter: %s\n",argv[i-1]);
                nerr=1;
                break;
            }
            raw_io(neoctrl,raw_write,raw_offset[raw_location],raw_size[raw_location],raw_location,argv[i]);
            continue;
        }
        printf("invalid parameter: %s\n",argv[i]);
        nerr=1;
    }
    if(write&&nerr==0)
    {
        printf("writing...\n");
        ncart->burn();
    }
    return 0;
/*
    LoadLibrary("..\\dllhook\\dll\\dll.dll");
    neocontrol neoctrl;
    if(neoctrl.open())
    {
        DWORD fs;
        printf("myth found\n");

        neoctrl.read(neo_menu,0,buffer,128*1024);
        SaveFile("mdmenu.bin",buffer,128*1024);

        neoctrl.read(neo_flash,0,buffer,2*1024*1024);
        SaveFile("aladdin.bin",buffer,2*1024*1024);

        neoctrl.read(neo_flash,2*1024*1024,buffer,1*1024*1024);
        SaveFile("dragoon.bin",buffer,1*1024*1024);

        neoctrl.read(neo_sram,0,buffer,4*neo_block_size);
        SaveFile("sram.bin",buffer,4*neo_block_size);

        neoctrl.read(neo_psram,0,buffer,8*1024*1024);
        SaveFile("psram.bin",buffer,8*1024*1024);

        LoadFile("mdmenu.bin",buffer,fs);
        LoadFile("menu_820.bin",buffer+0xA0000,fs);
        neoctrl.write(neo_menu,0,buffer,0xC0000);

        LoadFile("mickey.bin",buffer,fs);
        neoctrl.write(neo_flash,0,buffer,fs);

        LoadFile("sram.bin",buffer,fs);
        neoctrl.write(neo_sram,0,buffer,4*neo_block_size);

        neoctrl.read(neo_flash,32*1024*1024,buffer,4*1024*1024);
        SaveFile("sonic3d.bin",buffer,4*1024*1024);

    }
    else
    {
        printf("myth not found\n");
    }
    neoctrl.close();
    printf("end\n");
    return 0;
*/
}

// #########################################################################

int main(int argc,char* argv[])
{
    init_plugins();

    int success=0;

    for(int i=1;i<argc;i++)
    {
        if(strcmp(argv[i],"--help")==0||strcmp(argv[i],"-h")==0)
        {
            print_help(argv[0]);
            break;
        }
    }

    neodriver*neodrv=getdriver();
    if(!neodrv)
    {
        printf("device is not connected\n");
        return -1;
    }

    neoctrl=new neocontrol;
    if(neoctrl)
    {
        success=neoctrl->open(neodrv);
        if(!success)
        {
            printf("device is connected, but not recognized\n");
        }
        for(int i=1;i<argc;i++)
        {
            if(strcmp(argv[i],"--cartid")==0||strcmp(argv[i],"-id")==0)
            {
                membuf buffer;
                if(buffer.resize(0x40))
                {
                    neoctrl->getcartid(buffer);
                    SaveFile("cartid.bin",buffer,0x40);
                }
                break;
            }
        }
    }

    ncart=0;
    if(success)
    {
        success=0;
        ncart=new neocart;
        if(ncart)
        {
            success=ncart->open(neoctrl);
            if(!success)
            {
                printf("device is connected, but not recognized (this shouldn't be shown ever, unless there is a bug)\n");
            }
        }
    }

    int res=success?processCommandLine(argc,argv):-1;

    if(ncart)
    {
        ncart->close();
        delete ncart;
    }
    if(neoctrl)
    {
        neoctrl->close();
        delete neoctrl;
    }
    if(neodrv)
    {
        neodrv->close();
        delete neodrv;
    }
    printf("end\n");
    return res;
}

// #########################################################################
