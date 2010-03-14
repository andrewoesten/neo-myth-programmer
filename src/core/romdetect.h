#ifndef romdetect_g
#define romdetect_g
// #########################################################################

#include <neoutil.h>

// #########################################################################

enum
{
    unknown_rom,
    md_rom,
    smd_rom,
    sms_rom,
    s32x_rom,
    scd_bios,
    scd_bram,
    scd_bios_bram,
    scd_multibios,
    vgm_file,
    n64_rom,
    romtype_count
};

// #########################################################################

enum
{
    both_bytes,
    unknown_bytes,
    even_bytes,
    odd_bytes
};

// #########################################################################

const int st_n64eep4k=512;
const int st_n64eep16k=2*KB;
const int st_n64sram=32*KB;
const int st_n64flash=128*KB;

// #########################################################################

extern const char*romtype_str[romtype_count];

// #########################################################################

int compareHeaderString(void*data,int size,int offset,const char*str);
int decodeSmdRom(void*data,int size,membuf&out,int dn,int&ns);
int getRomType(void*data,int size);
int getSaveSize(int romtype,void*data,int size,int&even);
void convertSave2neo(int savetype,const BYTE*data_in,membuf&data_out,int&size);
void convertSave2n64(int savetype,const BYTE*data_in,membuf&data_out,int&size);

// #########################################################################
#endif
