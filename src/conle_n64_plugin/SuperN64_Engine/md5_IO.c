/*
    Copyright (c) 2010 - 2011 conleon1988 (conleon1988@gmail.com)

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without
    restriction, including without limitation the rights to use,
    copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following
    conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
    OTHER DEALINGS IN THE SOFTWARE.
*/

#include "md5_IO.h"
#include "md5.h"
#include "io.h"
#include "utility.h"
#include "sharedBuffers.h"
#include "../NEO_SN64Engine_DynaStatsShared.h"

static md5_state_t bindedState;
static md5_byte_t bindedDigest[16];

void MD5_BeginAdditiveCalculation()
{
    NEODSB_WriteTaskName("MD5_BeginAdditiveCalculation()\0");
    MD5CodeBuffer[0] = '\0';
    md5_init(&bindedState);
    UTIL_SetMemorySafe((char*)&bindedDigest,'\0',16);
}

void MD5_Push(const char* in,const int size)
{
    md5_append(&bindedState,(const md5_byte_t*)in,size);
}

void MD5_EndAdditiveCalculation()
{
    int i;

    NEODSB_WriteTaskName("MD5_EndAdditiveCalculation()\0");
    md5_finish(&bindedState, bindedDigest);

    for (i = 0; i < MD5_CODE_SIZE ; ++i)
        sprintf(MD5CodeBuffer + i * 2 , "%02X", bindedDigest[i]);

    MD5CodeBuffer[32] = '\0';

}

//Auto
int MD5_LoadToSharedBuffer()
{
    int prg;
    int i;
    int read;
    int len;
    md5_state_t state;
    md5_byte_t digest[16];

    MD5CodeBuffer[0] = '\0';


    NEODSB_WriteTaskName("MD5_LoadToSharedBuffer()\0");

    IO_Seek(0,IO_SEEK_SET);
    IO_Seek(0,IO_SEEK_END);
    len = IO_Tell();
    IO_Seek(0,IO_SEEK_SET);

    md5_init(&state);

    prg = 0;

    while(!IO_Eof())
    {
        read = IO_Read(chunkBuffer,1,CHUNK_32K);

        NEODSB_SyncThread();

        if(read != CHUNK_32K)
        {
            //Some error
        }

        md5_append(&state,(const md5_byte_t*)chunkBuffer,read);

        NEODSB_WriteTaskProgress((int)(((float)(prg) / (float)len) * 100));
        prg += CHUNK_32K;
        NEODSB_SyncThread();
    }

    md5_finish(&state, digest);

    for (i = 0; i < MD5_CODE_SIZE ; ++i)
        sprintf(MD5CodeBuffer + i * 2 , "%02X", digest[i]);

    MD5CodeBuffer[32] = '\0';

    IO_Seek(0,IO_SEEK_SET);

    return 1;
}
