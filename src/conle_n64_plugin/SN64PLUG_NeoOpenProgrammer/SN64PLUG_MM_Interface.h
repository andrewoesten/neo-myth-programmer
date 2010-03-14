//SN64 Plugin interface for Madmonkey's NEO MD/N64 Opensource programmer
//By conleon1988@gmail.com

#ifndef SN64PLUG_MM_Interface_H
#define SN64PLUG_MM_Interface_H
#include "../SuperN64_Engine/utility.h"
#include "../SuperN64_Engine/sharedBuffers.h"
#include "../SuperN64_Engine/io.h"
#include "../SuperN64_Engine/md5_IO.h"
#include "../SuperN64_Engine/n64_image.h"
#include "../SuperN64_Engine/checksum.h"
#include "../SuperN64_Engine/database.h"
#include "../SuperN64_Engine/patch_IO.h"
#include "../NEO_SN64Engine_DynaStatsShared.h"
#include "../SuperN64_Engine/mstream.h"

enum
{
    SN64PLUG_OP_SAVETYPE = 0xA1,
    SN64PLUG_OP_CIC,
    SN64PLUG_OP_CRC,
    SN64PLUG_OP_GOODNAME,
    SN64PLUG_OP_MD5
};

#ifdef __cplusplus
extern "C" {
#endif

void SN64PLUG_Init(const char* workDirectory,const char* pathSepDelim);
int SN64PLUG_ProcessImage(const char* filename,void* data,const int size);
int SN64PLUG_Get(int* result,const short what);
void SN64PLUG_startup();
//int SN64PLUG_Import(void* data,const int size);
//int SN64PLUG_Set(void* value,const short what);
void SN64PLUG_Begin();
void SN64PLUG_End();

#ifdef __cplusplus
}
#endif

#endif
