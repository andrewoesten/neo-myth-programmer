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

#ifndef _IO_H_
#define _IO_H_
#ifdef __cplusplus
extern "C" {
#endif 
#define _IO_BUILD_WITH_STDIO_
#undef _IO_BUILD_WITH_ONTHEFLY_MODE_SWITCHING_


#ifdef _IO_BUILD_WITH_STDIO_
#include <stdio.h>
typedef FILE IOHandle;

enum
{
    IO_SEEK_SET = SEEK_SET,
    IO_SEEK_CUR = SEEK_CUR,
    IO_SEEK_END = SEEK_END
};
#endif

int IO_BindHandle(IOHandle* handle);
int IO_Open(const char* filename,const char* modes);
#ifdef _IO_BUILD_WITH_ONTHEFLY_MODE_SWITCHING_
    const char* IO_GetFileName();
    int IO_SwitchMode(const char* newModes,const int saveStreamOffset);
#endif
int IO_Close();
int IO_Seek(long int offset,int origin);
int IO_Tell();
int IO_Read(void* ptr,int size,int count);
int IO_Write(void* ptr,int size,int count);
int IO_Putc(int character);
int IO_Getc();
int IO_Eof();
int IO_Puts(const char* str);
int IO_IsAlive();

char* IO_Gets(char* str,int num);
const char* IO_GetModeString();
void IO_Rewind();
void IO_Init();
void IO_Shutdown();


IOHandle* IO_GetBindedStreamHandle();

//Single
IOHandle* IO_OpenSingle(const char* filename,const char* modes);
int IO_CloseSingle(IOHandle* handle);
int IO_SeekSingle(long int offset,int origin,IOHandle* handle);
int IO_TellSingle(IOHandle* handle);
int IO_ReadSingle(void* ptr,int size,int count,IOHandle* handle);
int IO_WriteSingle(void* ptr,int size,int count,IOHandle* handle);
int IO_PutcSingle(int character,IOHandle* handle);
int IO_GetcSingle(IOHandle* handle);
int IO_EofSingle(IOHandle* handle);
int IO_PutsSingle(const char* str,IOHandle* handle);
char* IO_GetsSingle(char* str,int num,IOHandle* handle);
void IO_RewindSingle(IOHandle* handle);

#ifdef __cplusplus
}
#endif 
#endif // _IO_H_
