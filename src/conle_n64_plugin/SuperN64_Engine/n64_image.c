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

#include "n64_image.h"
#include "utility.h"
#include "sharedBuffers.h"
#include "io.h"
#include "NEO_Types.h"
#include "crc.h"
#include "../NEO_SN64Engine_DynaStatsShared.h"

NEON64_RomFormat N64IMG_DetectFormatFromBuffer(const char* buf4Bytes)
{
    switch( UTIL_GetLong(buf4Bytes + 0) )
    {
        case 0x80371240:
            return NEON64_FMT_Z64;

        case 0x37804012:
            return NEON64_FMT_V64;

        case 0x40123780:
            return NEON64_FMT_N64;
    }

    return NEON64_FMT_UNKNOWN;
}

//For automatic single file R/W ops
int N64IMG_IsCompatible()
{
    int r;

    NEODSB_WriteTaskName("N64IMG_IsCompatible()\0");
    IO_Seek(0,IO_SEEK_SET);
    IO_Seek(0,IO_SEEK_END);

    r = IO_Tell();

    IO_Seek(0,IO_SEEK_SET);

    if(r <= sizeof(RomHeader))
        return 0;

    if(!UTIL_IsMultipleOf(r,4))
        return 0;

    return N64IMG_MakeValid();
}

int N64IMG_MakeValid()
{
    int r;
    int d;
    int blocks;
    int prg;
    IO_Seek(0,IO_SEEK_SET);
    NEODSB_WriteTaskName("N64IMG_MakeValid()\0");

    if(IO_Read(chunkBuffer,1,4) != 4)
        return 0;

    switch( UTIL_GetLong(chunkBuffer + 0) )
    {
        //z64 binary - 0x80371240 [ABCD] -
        case 0x80371240:
        break;

        //v64 binary - 0x37804012 [BADC] (BS) -
        case 0x37804012:
        {
            prg = 0;

            IO_Seek(0,IO_SEEK_SET);
            IO_Seek(0,IO_SEEK_END);
            blocks = IO_Tell() / CHUNK_32K;
            IO_Seek(0,IO_SEEK_SET);

            while(blocks--)
            {
                NEODSB_SyncThread();

                d = IO_Tell();
                r = IO_Read(chunkBuffer,1,CHUNK_32K);
                IO_Seek(d,IO_SEEK_SET);

                UTIL_Byteswap16(chunkBuffer,CHUNK_32K);
                IO_Write(chunkBuffer,1,r);

                NEODSB_WriteTaskProgress((int)(((float)(prg) / (float)r) * 100));
                prg += CHUNK_32K;

            }

            break;
        }

        //n64 binary - 0x40123780 [DCBA] (WS) -
        case 0x40123780:
        {
            IO_Seek(0,IO_SEEK_SET);
            IO_Seek(0,IO_SEEK_END);
            blocks = IO_Tell() / CHUNK_32K;
            IO_Seek(0,IO_SEEK_SET);

            prg = 0;

            while(blocks--)
            {
                NEODSB_SyncThread();

                d = IO_Tell();
                r = IO_Read(chunkBuffer,1,CHUNK_32K);
                IO_Seek(d,IO_SEEK_SET);

                UTIL_Byteswap32(chunkBuffer,CHUNK_32K);
                IO_Write(chunkBuffer,1,r);

                NEODSB_WriteTaskProgress((int)(((float)(prg) / (float)r) * 100));
                prg += CHUNK_32K;
            }

            break;
        }

        //Unknown
        default:
        {
            IO_Seek(0,IO_SEEK_SET);
            return 0;
        }
    }

    IO_Seek(0,IO_SEEK_SET);

    if(IO_Read(chunkBuffer,1,4) != 4)
        return 0;

    IO_Seek(0,IO_SEEK_SET);

    return ( UTIL_GetLong(chunkBuffer + 0) == 0x80371240);
}

int N64IMG_WriteHeader(RomHeader* head)
{
    int r;

    if(!head)
        return 0;

    NEODSB_WriteTaskName("N64IMG_WriteHeader()\0");

    IO_Seek(0,IO_SEEK_SET);
    r = IO_Write(head,1,sizeof(RomHeader));
    IO_Seek(0,IO_SEEK_SET);

    return (r == sizeof(RomHeader));
}

int N64IMG_FetchHeader(RomHeader* head)
{
    int r;

    NEODSB_WriteTaskName("N64IMG_FetchHeader()\0");

    if(!head)
        return 0;

    IO_Seek(0,IO_SEEK_SET);

    r = IO_Read(head,1,sizeof(RomHeader));

    UTIL_SetMemorySafe(chunkBuffer,'\0',20);

    UTIL_CopyString(&chunkBuffer[0],head->name,"\0");
    chunkBuffer[19] = '\0';

    UTIL_Trim(head->name,chunkBuffer);
    head->name[19] = '\0';

    //head->countryCode &= 0xff;
    head->crc1 = ((head->crc1 << 8)&0xff) + (head->crc1);
    head->crc2 = ((head->crc2 << 8)&0xff) + (head->crc2);

    IO_Seek(0,IO_SEEK_SET);

    return ( r == sizeof(RomHeader) );
}


/*void N64IMG_CalculateCICCRC64(RomHeader* head,QuadWord* crc)
{
    //Based on PJ64 but not working!
    int i = 0;
    *crc = 0;

    for(; i < 0x1000; i += 4 )
        *crc += *(int*)(head->bootCode + i);
}
*/
NEON64_BootChip N64IMG_DetectCICType(RomHeader* head)
{
    unsigned int crc;

    NEODSB_WriteTaskName("N64IMG_DetectCICType()\0");
    crc = CRC_Calculate(0,(const unsigned char*)head->bootCode,1000);

   // printf("CRC = 0x%x\n",crc);

    /*
        Dumps from my stats tools.

        //Bootchip area checksum(1000bytes)

        //EU
        0x303faac9
        0xf3106101
        0xe7cd9d51
        0x7ae65c9
        0x86015f8f

        //US
        0xf0da3d50
        0xf3106101
        0xe7cd9d51
        0x7ae65c9
        0x86015f8f

        //JP
        0xf0da3d50
        0xf3106101
        0xe7cd9d51
        0x7ae65c9
        0x86015f8f
    */
    switch(crc)
    {
        case 0x303faac9:
        case 0xf0da3d50:
            return NEON64_CIC_6101;

        case 0xf3106101:
            return NEON64_CIC_6102;

        case 0xe7cd9d51:
            return NEON64_CIC_6103;

        case 0x7ae65c9:
            return NEON64_CIC_6105;

        case 0x86015f8f:
            return NEON64_CIC_6106;
    }

    return NEON64_CIC_6102;
    /* Based on pj64 and not working
    printf("CIC CRC = 0x%llx\n",crc);
    switch (crc)
    {
        case 0x000000D0027FDF31LL:
        case 0x000000CFFB631223LL:
            return 6101;
        break;

        case 0x000000D057C85244LL:
            return 6102;
        break;

        case 0x000000D6497E414BLL:
            return 6103;
        break;

        case 0x0000011A49F60E96LL:
            return 6105;
        break;

        case 0x000000D6D5BE5580LL:
            return 6106;
        break;

        default:
            return 6105;
        break;
    }

    return 0;*/
}

int N64IMG_WritePifRomCountryCode(short reg)
{
    int r;

    NEODSB_WriteTaskName("N64IMG_WritePifRomCountryCode()\0");

    IO_Seek(0x3d,IO_SEEK_SET);
    r = IO_Write((char*)&reg,1,1);
    IO_Seek(0,IO_SEEK_SET);

    return r;
}

short N64IMG_FetchPifRomCountryCode()
{
    short r = 0;

    NEODSB_WriteTaskName("N64IMG_FetchPifRomCountryCode()\0");

    IO_Seek(0x3e,IO_SEEK_SET);
    IO_Read((char*)&r,1,2);
    IO_Seek(0,IO_SEEK_SET);

    r &= 0xFF;

    return r;
}
