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
#include "xPS.h"
#include "patch_IO.h"
#include "utility.h"
#include "sharedBuffers.h"

/*
int PIO_UpdateHeader(RomHeader* head)
{
    int r;

    IO_Seek(0,IO_SEEK_SET);
    r = IO_Write(head,1,sizeof(RomHeader));
    IO_Seek(0,IO_SEEK_SET);

    return r == sizeof(RomHeader);
}

int PIO_ImportIPS(IOHandle* src,IOHandle* patch)
{
    IOHandle* active = IO_GetBindedStreamHandle();

    IO_BindHandle(patch);
    IO_Seek(0,IO_SEEK_SET);

    IO_BindHandle(src);
    IO_Seek(0,IO_SEEK_SET);

    if(active)
        IO_BindHandle(active);

    return 1;
}

int PIO_ImportAPS(IOHandle* src,IOHandle* patch)
{
    IOHandle* active = IO_GetBindedStreamHandle();

    IO_BindHandle(patch);
    IO_Seek(0,IO_SEEK_SET);

    IO_BindHandle(src);
    IO_Seek(0,IO_SEEK_SET);

    if(active)
        IO_BindHandle(active);

    return 1;
}
*/

int PIO_Import(IOHandle* src,IOHandle* patch)
{
    //IOHandle* active = IO_GetBindedStreamHandle();

    if(!src)
        return 0;

    if(!patch)
        return 0;

    //if(!IO_IsAlive())
      //  return 0;

   /* IO_BindHandle(patch);
    IO_Seek(0,IO_SEEK_SET);

    IO_BindHandle(src);
    IO_Seek(0,IO_SEEK_SET);

    UTIL_SetMemorySafe(chunkBuffer,'\0',5);*/

    if(IO_ReadSingle(chunkBuffer,1,5,patch) != 5)
    {
        IO_SeekSingle(0,IO_SEEK_SET,patch);

        return 0;
    }

    chunkBuffer[5] = '\0';

    if(UTIL_StringFind(chunkBuffer,"PATCH"))
		return SN64PLUG_xPS_IPS_Interface(src,patch);
    else if(UTIL_StringFind(chunkBuffer,"APS10"))
		return SN64PLUG_xPS_APS_Interface(src,patch);

    return 0;
}
