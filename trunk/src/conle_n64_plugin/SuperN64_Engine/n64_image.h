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

#ifndef N64_IMAGE_H_
#define N64_IMAGE_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "NEO_Types.h"
typedef struct RomHeader RomHeader;
//typedef __int64 QuadWord;

struct RomHeader
{
   unsigned char init_PI_BSB_DOM1_LAT_REG;  /* 0x00 */
   unsigned char init_PI_BSB_DOM1_PGS_REG;  /* 0x01 */
   unsigned char init_PI_BSB_DOM1_PWD_REG;  /* 0x02 */
   unsigned char init_PI_BSB_DOM1_PGS_REG2; /* 0x03 */
   unsigned int clockRate;                  /* 0x04 */
   unsigned int pc;                         /* 0x08 */
   unsigned int version;                    /* 0x0C */
   unsigned int crc1;                       /* 0x10 */
   unsigned int crc2;                       /* 0x14 */
   unsigned int pad[2];                 /* 0x18 */
   unsigned char name[20];                   /* 0x20 */
   unsigned int pad1;                    /* 0x34 */
   unsigned int manufacturerID;            /* 0x38 */
   unsigned short cartID;             /* 0x3C - Game serial number  */
   unsigned short countryCode;             /* 0x3E */
   unsigned int bootCode[1008];            /* 0x40 */
};

int N64IMG_IsCompatible();
int N64IMG_MakeValid();
NEON64_BootChip N64IMG_DetectCICType(RomHeader* head);
int N64IMG_FetchHeader(RomHeader* head);
int N64IMG_WriteHeader(RomHeader* head);
int N64IMG_WritePifRomCountryCode(short reg);
short N64IMG_FetchPifRomCountryCode();
//void N64IMG_CalculateCICCRC64(RomHeader* head,QuadWord* crc);
NEON64_RomFormat N64IMG_DetectFormatFromBuffer(const char* buf4Bytes);
#ifdef __cplusplus
}
#endif
#endif // N64_IMAGE_H_
