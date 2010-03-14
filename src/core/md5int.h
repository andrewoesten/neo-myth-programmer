#ifndef md5_g
#define md5_g
// #########################################################################

#include <neoutil.h>

// #########################################################################

typedef struct
{
    BYTE buffer[64];
    DWORD state[4];
    DWORD count[2];
} MD5_CTX;

// #########################################################################

void MD5Init(MD5_CTX*);
void MD5Update(MD5_CTX*,const void*,const DWORD);
void MD5Final(void*,MD5_CTX*);

// #########################################################################
#endif
