#include "SN64PLUG_MM_Interface.h"
#include <malloc.h>
#ifdef WIN32
#include <windows.h>
#endif

typedef struct SN64PLUG_Session SN64PLUG_Session;

struct SN64PLUG_Session
{
    char filename[96 + 1];//filename only...path is removed
    char md5[32 + 1];
    char goodname[32 + 1];

    unsigned int saveAlloc;
    unsigned int cicType;
    unsigned int crc1;
    unsigned int crc2;

    short processed;
};

static SN64PLUG_Session singleSession;

inline void ___internal__InvalidateSession()
{
    singleSession.processed = 0;

    singleSession.filename[0] = singleSession.filename[96] =
    singleSession.md5[0] = singleSession.md5[32] = '\0';
    singleSession.goodname[0] = singleSession.goodname[32] = '\0';

    singleSession.saveAlloc = singleSession.crc1 = singleSession.crc2 =
    singleSession.cicType = 0;
}

void SN64PLUG_Init(const char* workDirectory,const char* pathSepDelim)
{
    UTIL_CopyString(workDirectoryBuffer+0,workDirectory,"\0");
    pathSeparatorBuffer[0] = pathSepDelim[0];
    pathSeparatorBuffer[1] = '\0';
}

void SN64PLUG_Begin()
{
    ___internal__InvalidateSession();
}

void SN64PLUG_End()
{
    ___internal__InvalidateSession();
}

int SN64PLUG_ProcessImage(const char* filename,void* data,const int size)
{
    char* p = NULL;
    int fnLen,bytes;
    unsigned int endianess;

    if(!UTIL_IsMultipleOf(size,4))
        return 0;

    if(!data)
        return 0;

    if(!filename)
        return 0;

    //Remove path from filename for LEX
    p = UTIL_StringFindLastCharConst(filename,pathSeparatorBuffer[0]);

    if(p)
        UTIL_CopyString(singleSession.filename,p + 1,"\0");
    else
    {
        //filename only?
        fnLen = UTIL_StringLength(filename);

        if(!fnLen)
            return 0;

        if(fnLen > 96)
            UTIL_CopySafe(singleSession.filename,filename,96);
        else
            UTIL_CopyString(singleSession.filename,filename,"\0");
    }

    UTIL_CopyString(filenameBuffer,singleSession.filename,"\0");

    MEMSTREAM_Bind(data,size);

    endianess = UTIL_GetLong((const char*)data);
    //MEMSTREAM_Read(&endianess,4);

    MEMSTREAM_Seek(0,MEMSTREAM_SEEK_SET);

    bytes = size;

    MD5_BeginAdditiveCalculation();

    while(!MEMSTREAM_IsEndOfStream())
    {
        p = (char*)MEMSTREAM_GetOffsetPointer();

        MD5_Push(p,CHUNK_32K);

        switch(endianess)
        {
            case 0x80371240:
                break;//z64

            case 0x37804012:
            {
                UTIL_Byteswap16(p,CHUNK_32K);
                break;//V64
            }

            case 0x40123780:
            {
                UTIL_Byteswap32(p,CHUNK_32K);
                break;//N64
            }
        }

        MEMSTREAM_Seek(CHUNK_32K,MEMSTREAM_SEEK_CUR);
    }

    MD5_EndAdditiveCalculation();

    MEMSTREAM_Seek(0,MEMSTREAM_SEEK_SET);

    //Get header and remove whitespace
    RomHeader head;
    MEMSTREAM_Read(&head,sizeof(RomHeader));

    UTIL_SetMemorySafe(chunkBuffer,'\0',20);

    UTIL_CopyString(&chunkBuffer[0],head.name,"\0");
    chunkBuffer[19] = '\0';

    UTIL_Trim(head.name,chunkBuffer);
    head.name[19] = '\0';

    //head->countryCode &= 0xff;
    singleSession.crc1 = (head.crc1 = ((head.crc1 << 8)&0xff) + (head.crc1));
    singleSession.crc2 = (head.crc2 = ((head.crc2 << 8)&0xff) + (head.crc2));

    singleSession.cicType = N64IMG_DetectCICType(&head);

    singleSession.saveAlloc = DB_GetSaveType();

    singleSession.processed = 1;

    UTIL_Byteswap16((char*)data,size);

    MEMSTREAM_UnBind();

    return 1;
}

int SN64PLUG_Get(int* result,const short what)
{
    if(!singleSession.processed)
        return 0;

    switch(what)
    {
        case SN64PLUG_OP_SAVETYPE:
            *result = singleSession.saveAlloc;
        break;

        case SN64PLUG_OP_CIC:
            *result = singleSession.cicType;
        break;

        case SN64PLUG_OP_CRC:
            result[0] = singleSession.crc1;
            result[1] = singleSession.crc2;
        break;
    }

    return 1;
}

void SN64PLUG_startup()
{
    int i,l;
    char*currPath;
#ifdef WIN32
    currPath=(char*)malloc(16*1024);
    if(currPath)
    {
        if(GetModuleFileNameA(0,currPath,16*1024-1))
        {
            l=lstrlenA(currPath);
            for(i=0;i<l;i++)
            {
                if(currPath[i]=='/')currPath[i]='\\';
            }
            for(i=l-1;i>=0;i--)
            {
                if(currPath[i]=='\\')
                {
                    currPath[i+1]=0;
                    break;
                }
            }
            SN64PLUG_Init(currPath,"\\\0");
        }
        free(currPath);
    }
#else
    //http://www.gamedev.net/community/forums/topic.asp?topic_id=459511
    SN64PLUG_Init("~/.neoMythProgrammer/","/\0");
#endif
}

//int SN64PLUG_Import(void* data,const int size){}
//int SN64PLUG_Set(void* value,const short what){}
