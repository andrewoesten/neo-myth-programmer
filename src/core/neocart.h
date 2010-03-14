#ifndef neocart_g
#define neocart_g
// #########################################################################

#include <neocontrol.h>
#include <neosram.h>

// #########################################################################

#define MAX_ENTRIES 401

// #########################################################################

struct selEntry
{
    int id;
    int dirty;
    int deleted;
    membuf romdata;
    BYTE type;                 /* 0 = MD game, 1 = 32X game, 2 = SMS game, 128 = directory */
    BYTE run;                  /* run mode: 6 = MD/32X game, 8 = CD BIOS, 0x13 = SMS */
    BYTE bbank;                /* backup ram bank number */
    BYTE bsize;                /* backup ram size in 8KB units */
    int offset;                /* offset in media of data associated with entry */
    int length;                /* amount of data associated with entry */
    int n64_cic;
    int n64_saveType;
    int n64_modeA;
    char name[24+16/*bug?*/];  /* entry name string (null terminated) */
};

// #########################################################################

const int menu_offset=0x00B000;
const int default_menu_size=64*KB;
const int badblocks_offset=0x00B0000;
const int badblocks_size=64*KB;
const int table_block_size=128*KB;
const int save_block_size=8*KB;

// #########################################################################

class neocart
{
    int state;
    neocontrol*neoctrl;
    membuf neomenu;
    neosram sram;
    neosram n64sram;
    selEntry*gSelections;
    DWORD*gSelections_index;
    int gMaxEntry;
    int gRomId;
    progress_indicator*prg;
    int menu_size;
public:
    neocart();
    ~neocart();
    int open(neocontrol*nc);
    void setProgress(progress_indicator*sprg);
    selEntry*getrominfo(int index);
    selEntry*getrominfo_byid(int romid);
    int getfreespace();
    int addrom(const char*name,void*data,int fs);
    void replace_menu(void*data,int fs);
    int delrom(char*name);
    int delrom(int romid);
    int backupsram(char*name);
    int backupsram(int romid,char*fname);
    int backuprom(char*name);
    int backuprom(int romid,char*fname);
    int initsram(int romid);
    int linksram(int romid,void*data,int ss);
    int burn();
    int format();
    int format(int romid);
    void close();
};

// #########################################################################

int getn64SramSize(int n64_saveType);

// #########################################################################
#endif
