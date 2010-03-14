#ifndef neosram_g
#define neosram_g
// #########################################################################

#include <neocontrol.h>

// #########################################################################

#define bank_read 1
#define bank_write 2

// #########################################################################

class neosram
{
    int state;
    membuf sram;
    int banksize;
    int numbanks;
    int*bankstate;
    int sramsize;
    neocontrol*neoctrl;
    int neoSramLocation;
    int readbank(int nb);
    int writebank(int nb);
public:
    neosram();
    ~neosram();
    int open(neocontrol*nc,int sloc);
    int read(void*data,int offset,int size);
    int write(void*data,int offset,int size);
    int burn();
    void close();
};

// #########################################################################
#endif
