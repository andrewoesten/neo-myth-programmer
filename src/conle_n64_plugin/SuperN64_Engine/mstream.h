#ifndef __MSTREAM__H__
#define __MSTREAM__H__

enum
{
    MEMSTREAM_SEEK_SET = 0xC0,
    MEMSTREAM_SEEK_CUR,
    MEMSTREAM_SEEK_END
};

void MEMSTREAM_Bind(void* ownMemory,const long int ownMemorySize);
void MEMSTREAM_UnBind();

int MEMSTREAM_Seek(const long int offset,const int origin);
int MEMSTREAM_Read(void* ptr,int size);
int MEMSTREAM_Write(void* ptr,int size);
int MEMSTREAM_Putc(const char character);
int MEMSTREAM_Getc();
int MEMSTREAM_Tell();
int MEMSTREAM_IsEndOfStream();

void* MEMSTREAM_GetOffsetPointer();
#endif

