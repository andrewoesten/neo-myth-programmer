#ifndef neocontrol_g
#define neocontrol_g
// #########################################################################

#include <neodriver.h>
#include <neocontrolcodes.h>

// #########################################################################

enum{
    neo_menu,
    neo_flash,
    neo_sram,
    neo_psram,
    neo_cache,
    neo_n64menu,
    neo_n64sram,
    neo_loc_count
};

// #########################################################################

enum{
    total_size,
    bank_size,
    block_size,
    ideal_block_size
};

// #########################################################################

class neocontrol
{
    DWORD state;
    neodriver*neodrv;
    char inbuffer[64],outbuffer[64];
    char cartid[64];
    int cartType;
    int menuType;
    int flashType;
    const menu_info*minfo;
    const flash_info*finfo;
    progress_indicator*prg;
    int sync0();
    int sync1();
    int w_cmd0(const void*cmd,DWORD size);
    int w_cmd1(const void*cmd,DWORD size);
    int r_res0(void*outp,DWORD size);
    int r_res1(void*outp,DWORD a,DWORD offset,DWORD size);
    int r_res2(void*outp);
    int r_res3(void*outp,DWORD a,DWORD offset,DWORD size);
    int w_data0(const void*inb,DWORD a,DWORD b,DWORD offset,DWORD size);
    int w_data1(const void*inb);
    int w_data2(const void*inb,DWORD a,DWORD b,DWORD offset,DWORD size);
    int edgeFixCmd(membuf&ocmd,DWORD&size,DWORD dir);
    int asicReset();
    int cancelio(int dir);
public:
    neocontrol();
    ~neocontrol();
    int open(neodriver*driver);
    int isConnected();
    void setProgress(progress_indicator*sprg);
    void getcartid(void*data);
    int read(DWORD param,DWORD offset,void*lpOutBuffer,DWORD nOutBufferSize);
    int write(DWORD param,DWORD offset,const void*lpInBuffer,DWORD nInBufferSize);
    DWORD getsize(DWORD param,DWORD bs);
    void close();
};

// #########################################################################

const int n64myth_cart=5;

// #########################################################################
#endif
