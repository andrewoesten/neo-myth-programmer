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

#include "sharedBuffers.h"
#include "utility.h"
//ALL SHARED BUFFERS USED BY THE PATCHING ENGINE
#include "../NEO_SN64Engine_DynaStatsShared.h"
char MD5CodeBuffer[MD5_CODE_SIZE + 1];
char chunkBuffer[CHUNK_32K + 1];
char workDirectoryBuffer[WORKDIR_SIZE + 1];
char goodN64NameBuffer[GOODN64_BUFFER_SIZE + 1];
char filenameBuffer[FILENAME_BUFFER_SIZE + 1];
char pathSeparatorBuffer[2] = {'\0','\0'};

void SBFS_Clear()
{
    NEODSB_WriteTaskName("SBFS_Clear()\0");
    UTIL_SetMemorySafe(MD5CodeBuffer,'\0',MD5_CODE_SIZE);
    UTIL_SetMemorySafe(chunkBuffer,'\0',CHUNK_32K);
    //UTIL_SetMemorySafe(workDirectoryBuffer,'\0',WORKDIR_SIZE);
    UTIL_SetMemorySafe(goodN64NameBuffer,'\0',GOODN64_BUFFER_SIZE);
    UTIL_SetMemorySafe(filenameBuffer,'\0',FILENAME_BUFFER_SIZE);
    pathSeparatorBuffer[0] = pathSeparatorBuffer[1] = '\0';
}

void SBFS_ClearFast()
{
    NEODSB_WriteTaskName("SBFS_ClearFast()\0");
    pathSeparatorBuffer[0] = pathSeparatorBuffer[1] = '\0';
    filenameBuffer[0] =
    filenameBuffer[FILENAME_BUFFER_SIZE] =
    goodN64NameBuffer[0] =
    goodN64NameBuffer[GOODN64_BUFFER_SIZE] =
    //workDirectoryBuffer[0] =
    //workDirectoryBuffer[WORKDIR_SIZE] =
    MD5CodeBuffer[0] =
    MD5CodeBuffer[MD5_CODE_SIZE] =
    chunkBuffer[0] =
    chunkBuffer[CHUNK_32K] = '\0';
}
