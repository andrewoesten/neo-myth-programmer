
#include "mstream.h"

static char* workMemAddr = ((void*)0);
static long int workMemLen = 0;
static long int workMemOffset = 0;

void MEMSTREAM_Bind(void* ownMemory,const long int ownMemorySize)
{
    MEMSTREAM_UnBind();

    if((!ownMemory) || (!ownMemorySize) )
        return;

    workMemAddr = ownMemory;
    workMemLen = ownMemorySize;
}

void MEMSTREAM_UnBind()
{
    workMemAddr = ((void*)0);
    workMemLen = workMemOffset = 0;
}

int MEMSTREAM_Seek(const long int offset,const int origin)
{
    switch(origin)
    {
        default: return 0;

        case MEMSTREAM_SEEK_SET:
        {
            workMemOffset = offset;
            return 1;
        }

        case MEMSTREAM_SEEK_CUR:
        {
            if(workMemOffset + offset > workMemLen)
                return 0;

            workMemOffset += offset;

            return 1;
        }

        case MEMSTREAM_SEEK_END:
        {
            workMemOffset = workMemLen;
            return 1;
        }
    }

    return 0;
}

int MEMSTREAM_Read(void* ptr,int size)
{
    char* p = (char*)ptr;

    if( (!ptr) || (!size) )
        return 0;

    while(size--)
        *(p++) = *(workMemAddr + (workMemOffset++));

    return 1;
}

int MEMSTREAM_Write(void* ptr,int size)
{
    char* p = (char*)ptr;

    if( (!ptr) || (!size) )
        return 0;

    while(size--)
        *(workMemAddr + (workMemOffset++)) = *(p++);

    return 1;
}

int MEMSTREAM_Putc(const char character)
{
    *(workMemAddr + (workMemOffset++)) = character;

    return 1;
}

int MEMSTREAM_Getc()
{
    return *(workMemAddr + (workMemOffset++));
}

int MEMSTREAM_Tell()
{
    return workMemOffset;
}

int MEMSTREAM_IsEndOfStream()
{
    return (workMemOffset >= workMemLen);
}

void* MEMSTREAM_GetOffsetPointer()
{
    return workMemAddr + workMemOffset;
}
