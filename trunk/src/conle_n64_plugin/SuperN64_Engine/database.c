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

#include "database.h"
#include "md5.h"
#include "io.h"
#include "sharedBuffers.h"
#include "utility.h"
#include "lexer.h"
#include "../NEO_SN64Engine_DynaStatsShared.h"

NEON64_SaveType DB_IntenseSaveTypeScan()
{
    const char* p;

    NEODSB_WriteTaskName("DB_IntenseSaveTypeScan()\0");

    if(!LEX_PreCompileSymbolTable(filenameBuffer))
        return NEON64_SAVE_NONE;

    {
        p = UTIL_StringFindLastCharConst(filenameBuffer,pathSeparatorBuffer[0]);

        if(p)
            UTIL_CopyString(goodN64NameBuffer,p+1,".\0");
        else
            UTIL_CopyString(goodN64NameBuffer,filenameBuffer,".\0");
    }

    NEODSB_WriteRomName(goodN64NameBuffer);

    if(LEX_PhraseMakesSense(E4_db))
    {
        NEODSB_WriteTaskName("LEX_PhraseMakesSense E4\0");
        //printf("link = e4\n");
        return NEON64_SAVE_EEPROM_4K;
    }
    else if(LEX_PhraseMakesSense(E16_db))
    {
        NEODSB_WriteTaskName("LEX_PhraseMakesSense E16\0");
        //printf("link = e16\n");
        return NEON64_SAVE_EEPROM_16K;
    }
    else if(LEX_PhraseMakesSense(FR_db))
    {
        NEODSB_WriteTaskName("LEX_PhraseMakesSense FRAM\0");
        //printf("link = fr\n");
        return NEON64_SAVE_FLASHRAM_1M;
    }
    else if(LEX_PhraseMakesSense(SR_db))
    {
        NEODSB_WriteTaskName("LEX_PhraseMakesSense SRAM\0");
        //printf("link = sr\n");
        return NEON64_SAVE_SRAM_32K;
    }
    else if(LEX_PhraseMakesSense(CP_db))
    {
        NEODSB_WriteTaskName("LEX_PhraseMakesSense CP\0");
        //printf("link = cp\n");
        return NEON64_SAVE_CONTROLLERPACK;
    }
    /*if(res == NEON64_SAVE_NONE)
        UTIL_CopyString(goodN64NameBuffer,"Unable to detect Good N64 DB Name\0","\0");
    else*/


    return NEON64_SAVE_NONE;
}

NEON64_SaveType DB_GetSaveType()
{
    char* p;
    int r;
    IOHandle* saveH = IO_GetBindedStreamHandle();
    IOHandle dbh;
    NEON64_SaveType res = NEON64_SAVE_CARD;//safe result
    NEODSB_WriteTaskName("DB_GetSaveType()\0");
   /* if(DB_IntenseSaveTypeScan() == NEON64_SAVE_EEPROM_4K)
        printf("YES!!\n");
    else
        printf("Not found\n");*/

    UTIL_CopyString(chunkBuffer + 0,workDirectoryBuffer,"\0");
    UTIL_StringAppend(chunkBuffer,"plugInfoDB");
    UTIL_StringAppend(chunkBuffer,pathSeparatorBuffer);
    UTIL_StringAppend(chunkBuffer,MD5CodeBuffer);
    UTIL_StringAppend(chunkBuffer,".sn64plug\0");

    //printf("DBF = %s\n",chunkBuffer);

    IO_BindHandle(&dbh);

    if(!IO_Open(chunkBuffer,"rb"))
    {
        IO_BindHandle(saveH);
        return DB_IntenseSaveTypeScan();
    }

    IO_Seek(0,IO_SEEK_END);
    r = IO_Tell();
    IO_Seek(0,IO_SEEK_SET);

    if( r > CHUNK_32K >> 1)
    {
        IO_Close();
        IO_BindHandle(saveH);
        return res;
    }

    IO_Read(chunkBuffer,1,r);
    chunkBuffer[r] = '\0';
    IO_Close();

    //printf("%s\n",chunkBuffer);
    p = UTIL_StringFind(chunkBuffer,"RefMD5=");

    if(p)
    {
        if(UTIL_StringFind(chunkBuffer,"GoodName="))
            UTIL_SubString(goodN64NameBuffer,chunkBuffer,"GoodName=","\n");

        IO_BindHandle(saveH);

        if(UTIL_StringFind(chunkBuffer,"RefMD5="))
            UTIL_SubString(MD5CodeBuffer,p,"RefMD5=","\n");
        else
        {
            IO_BindHandle(saveH);
            return DB_IntenseSaveTypeScan();
        }

        DB_GetSaveType();
    }
    else
    {
        if(goodN64NameBuffer[0] == '\0')
        {
            if(UTIL_StringFind(chunkBuffer,"GoodName="))
                UTIL_SubString(goodN64NameBuffer,chunkBuffer,"GoodName=","\n");
        }

        p = UTIL_StringFind(chunkBuffer,"SaveType=");

        if(!p)
        {
            IO_BindHandle(saveH);
            return DB_IntenseSaveTypeScan();
        }
        else
        {
            if(UTIL_StringFind(p,"None"))
                res = NEON64_SAVE_NONE;
            else if(UTIL_StringFind(p,"Controller"))
                res = NEON64_SAVE_CONTROLLERPACK;
            else if(UTIL_StringFind(p,"Eeprom 4KB"))
                res = NEON64_SAVE_EEPROM_4K;
            else if(UTIL_StringFind(p,"Eeprom 16KB"))
                res = NEON64_SAVE_EEPROM_16K;
            else if(UTIL_StringFind(p,"SRAM"))
                res = NEON64_SAVE_SRAM_32K;
            else if(UTIL_StringFind(p,"Flash RAM"))
                res = NEON64_SAVE_FLASHRAM_1M;
        }

        IO_BindHandle(saveH);
        return res;
    }

    IO_BindHandle(saveH);

    return DB_IntenseSaveTypeScan();
}
