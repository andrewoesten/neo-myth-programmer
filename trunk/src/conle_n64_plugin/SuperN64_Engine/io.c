

/*
	LIB IO V1.2

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

#include "io.h"

#ifdef _IO_BUILD_WITH_ONTHEFLY_MODE_SWITCHING_
#include "utility.h"
#endif

IOHandle* bindedIOHandle;
#ifdef _IO_BUILD_WITH_ONTHEFLY_MODE_SWITCHING_
    char openFileBuffer[512];
#endif

char modeString[16];

//Todo : Make a more generic copy function
void IO_InternalCopyModeString(const char* s)
{
    //dont include other library!
    int i;

    for(i = 0; i < 16; i++)
    {
        if( *(s + i) )
            *(modeString + i)= *(s + i);
        else
            break;
    }

    modeString[i] = '\0';
}

int IO_BindHandle(IOHandle* handle)
{
    return ( (handle = (bindedIOHandle) ) != NULL);
}

int IO_Open(const char* filename,const char* modes)
{
    #ifdef _IO_BUILD_WITH_STDIO_
    #ifdef _IO_BUILD_WITH_ONTHEFLY_MODE_SWITCHING_
    int r;
	
	if(!bindedIOHandle)
		return 0;

	r = ((bindedIOHandle = fopen(filename,modes)) != NULL);

    if(r)
    {
        int l = UTIL_StringLengthConst(filename);

        UTIL_CopySafe(openFileBuffer,filename,l);

        *(openFileBuffer + l) = '\0';
    }
    else
        *(openFileBuffer + 0) = '\0';

    IO_InternalCopyModeString(modes);

    return r;
    #else
    IO_InternalCopyModeString(modes);

    return ((bindedIOHandle = fopen(filename,modes)) != NULL);
    #endif
    #else

    #endif
}

#ifdef _IO_BUILD_WITH_ONTHEFLY_MODE_SWITCHING_
const char* IO_GetFileName()
{
    return openFileBuffer;
}

int IO_SwitchMode(const char* newModes,const int saveStreamOffset)
{
    int r = 0;
    int addr = 0;

    if(!bindedIOHandle)
        return 0;

    if(*(openFileBuffer + 0) == '\0')
        return 0;

    IO_InternalCopyModeString(newModes);

    if(saveStreamOffset)
        addr = IO_Tell();

    IO_Close();

    r = IO_Open(openFileBuffer,newModes);

    if(r && saveStreamOffset)
        return IO_Seek(addr,IO_SEEK_SET);

    return r;
}
#endif

int IO_Seek(long int offset,int origin)
{
    #ifdef _IO_BUILD_WITH_STDIO_
    return (fseek(bindedIOHandle,offset,origin) == 0);
    #else
    #endif
}

int IO_Tell()
{
    #ifdef _IO_BUILD_WITH_STDIO_
    return ftell(bindedIOHandle);
    #else
    #endif
}

int IO_Read(void* ptr,int size,int count)
{
    #ifdef _IO_BUILD_WITH_STDIO_
    return fread(ptr,size,count,bindedIOHandle);
    #else
    #endif
}

int IO_Write(void* ptr,int size,int count)
{
    #ifdef _IO_BUILD_WITH_STDIO_
    return fwrite(ptr,size,count,bindedIOHandle);
    #else
    #endif
}

int IO_Putc(int character)
{
    #ifdef _IO_BUILD_WITH_STDIO_
    return fputc(character,bindedIOHandle);
    #else
    #endif
}

int IO_Getc()
{
    #ifdef _IO_BUILD_WITH_STDIO_
    return fgetc(bindedIOHandle);
    #else
    #endif
}

int IO_Eof()
{
    #ifdef _IO_BUILD_WITH_STDIO_
    return feof(bindedIOHandle);
    #else
    #endif
}

int IO_Puts(const char* str)
{
    #ifdef _IO_BUILD_WITH_STDIO_
    return fputs(str,bindedIOHandle);
    #else
    #endif
}

char* IO_Gets(char* str,int num)
{
    #ifdef _IO_BUILD_WITH_STDIO_
    return fgets(str,num,bindedIOHandle);
    #else
    #endif
}

void IO_Rewind()
{
    #ifdef _IO_BUILD_WITH_STDIO_
    rewind(bindedIOHandle);
    #else
    #endif
}

int IO_Close()
{
	int r;
    modeString[0] = '\0';

    #ifdef _IO_BUILD_WITH_STDIO_

	if(bindedIOHandle)
		r = fclose(bindedIOHandle);

    bindedIOHandle = NULL;

    return r;
    #else
    #endif
}

int IO_IsAlive()
{
    return (bindedIOHandle != NULL);
}

void IO_Init()
{
    modeString[0] = modeString[15] = '\0';

    bindedIOHandle = NULL;

    #ifdef _IO_BUILD_WITH_ONTHEFLY_MODE_SWITCHING_
    *(openFileBuffer + 0) = '\0';
    #endif
}

void IO_Shutdown()
{
    IO_Close();
	IO_Init();
}

IOHandle* IO_GetBindedStreamHandle()
{
    return bindedIOHandle;
}

//Single
IOHandle* IO_OpenSingle(const char* filename,const char* modes)
{
   #ifdef _IO_BUILD_WITH_STDIO_
    return fopen(filename,modes);
    #else
    #endif
}

int IO_CloseSingle(IOHandle* handle)
{
   #ifdef _IO_BUILD_WITH_STDIO_
    return fclose(handle);
    #else
    #endif
}


int IO_SeekSingle(long int offset,int origin,IOHandle* handle)
{
   #ifdef _IO_BUILD_WITH_STDIO_
    return fseek(handle,offset,origin);
    #else
    #endif
}

int IO_TellSingle(IOHandle* handle)
{
   #ifdef _IO_BUILD_WITH_STDIO_
    return ftell(handle);
    #else
    #endif
}


int IO_ReadSingle(void* ptr,int size,int count,IOHandle* handle)
{
   #ifdef _IO_BUILD_WITH_STDIO_
    return fread(ptr,size,count,handle);
    #else
    #endif
}


int IO_WriteSingle(void* ptr,int size,int count,IOHandle* handle)
{
   #ifdef _IO_BUILD_WITH_STDIO_
    return fwrite(ptr,size,count,handle);
    #else
    #endif
}

int IO_PutcSingle(int character,IOHandle* handle)
{
   #ifdef _IO_BUILD_WITH_STDIO_
    return fputc(character,handle);
    #else
    #endif
}


int IO_GetcSingle(IOHandle* handle)
{
   #ifdef _IO_BUILD_WITH_STDIO_
    return fgetc(handle);
    #else
    #endif
}

int IO_EofSingle(IOHandle* handle)
{
   #ifdef _IO_BUILD_WITH_STDIO_
    return feof(handle);
    #else
    #endif
}


int IO_PutsSingle(const char* str,IOHandle* handle)
{
   #ifdef _IO_BUILD_WITH_STDIO_
    return fputs(str,handle);
    #else
    #endif
}

char* IO_GetsSingle(char* str,int num,IOHandle* handle)
{
   #ifdef _IO_BUILD_WITH_STDIO_
    return fgets(str,num,handle);
    #else
    #endif
}

void IO_RewindSingle(IOHandle* handle)
{
   #ifdef _IO_BUILD_WITH_STDIO_
    rewind(handle);
    #else
    #endif
}

