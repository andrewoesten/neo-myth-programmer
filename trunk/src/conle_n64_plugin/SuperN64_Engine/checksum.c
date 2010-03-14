
#include "utility.h"
#include "checksum.h"
#include "io.h"
#include "sharedBuffers.h"
#include "../NEO_SN64Engine_DynaStatsShared.h"
//Too messy code to bother re-writting it ATM.
//Got it from ->
//http://n64dev.svn.sourceforge.net/viewvc/n64dev/trunk/n64dev/util/n64tools/nifty.c?revision=8
//Just had to remove/replace a couple of code & functions, & to provide intrface function to the plugin
//Also changed the IO code with my custom IO interface

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define min2(a, b) ( (a)<(b) ? (a) : (b) )

#define CKSUM_START 0x1000
#define CKSUM_LENGTH 0x100000L
#define CKSUM_HDRPOS 0x10
#define CKSUM_END (CKSUM_START + CKSUM_LENGTH)

#define CKSUM_STARTVAL 0xf8ca4ddc

#define ROL(i, b) (((i)<<(b)) | ((i)>>(32-(b))))


/*
#define B2L(b, s) ( (((b)[0^(s)] & 0xffL) << 24) | \
                           (((b)[1^(s)] & 0xffL) << 16) | \
                           (((b)[2^(s)] & 0xffL) <<  8) | \
                           (((b)[3^(s)] & 0xffL)) )

#define L2B(l, b, s)  (b)[0^(s)] = ((l)>>24)&0xff; \
                             (b)[1^(s)] = ((l)>>16)&0xff; \
                             (b)[2^(s)] = ((l)>> 8)&0xff; \
                             (b)[3^(s)] = ((l)    )&0xff;
*/

#define HEADER_MAGIC 0x80371240

//int swapped = 0;

int CRC_PatchAuto()
{
    return CRC_Patch(chunkBuffer,CHUNK_32K);
}

int CRC_Patch(char* sharedBuffer,const int sharedBufferLength)
{

  unsigned long sum1 = 0, sum2 = 0;
  unsigned long flen = 0;
  unsigned long i = 0;
  unsigned long c1 = 0, k1 = 0, k2 = 0;
  unsigned long t1, t2, t3, t4, t5, t6;
  unsigned int n;
  long clen = CKSUM_LENGTH;
  long rlen = flen - CKSUM_START;


  NEODSB_WriteTaskName("Patching CRC\0");
  t1 = t2 = t3 = t4 = t5 = t6 = CKSUM_STARTVAL;

  IO_Seek(0,IO_SEEK_SET);

  if(IO_Read(sharedBuffer,1,12) != 12 )
    return 0;

/*
  if ( B2L(sharedBuffer, 0) == HEADER_MAGIC ) {
    swapped = 0;
  } else if ( B2L(sharedBuffer, 1) == HEADER_MAGIC ) {
    swapped = 1;
  } else {
    fprintf(stderr, "I don't know the format of this file\n");
    exit(1);
  }
*/
  IO_Seek(0,IO_SEEK_END);
  flen = IO_Tell();

  if (flen < CKSUM_END) {
    if (flen < CKSUM_START) {
        return 0;
      //fprintf(stderr, "File is too short to be an N64 image\n");
      //exit(1);
    } else if ( (flen & 3) != 0 ) {
        return 0;
      //fprintf(stderr, "File isn't a multiple of 4 bytes long\n");
      //exit(1);
    }
  }

  IO_Seek(CKSUM_START, IO_SEEK_SET);
  {
      int prg = 0;

    for (;;) {
      if (rlen > 0)
      {
          NEODSB_SyncThread();
        n = IO_Read(sharedBuffer, 1, min2(sharedBufferLength, clen));

        NEODSB_WriteTaskProgress((int)(((float)(prg) / (float)rlen) * 100));
        prg += n;

    if ( (n & 0x03) != 0) {
      n += IO_Read(sharedBuffer+n, 1, 4-(n&3));
    }
      } else {
    n = min2(sharedBufferLength, clen);
      }

      if ( (n == 0) || ((n&3) != 0) ) {
    if ( (clen != 0) || (n != 0) ){
      //fprintf(stderr, "A short has been read, cksum may be wrong\n");
    }
    break;
      }

      for ( i = 0 ; i < n ; i += 4 ) {

    c1 = UTIL_GetLong(sharedBuffer + i);

    k1 = t6 + c1;
    if (k1 < t6) t4++;
    t6 = k1;
    t3 ^= c1;
    k2 = c1 & 0x1f;
    k1 =ROL(c1, k2);
    t5 += k1;
    if (c1 < t2) {
      t2 ^= k1;
    } else {
      t2 ^= t6 ^ c1;
    }
    t1 += c1 ^ t5;
      }
      if (rlen > 0) {
    rlen -= n;
    if (rlen <= 0) UTIL_SetMemorySafe(sharedBuffer, '\0', sharedBufferLength);
      }
      clen -= n;
    }

    sum1 = t6 ^ t4 ^ t3;
    sum2 = t5 ^ t2 ^ t1;
  }

   UTIL_PutLong(sharedBuffer + 0,sum1);
   UTIL_PutLong(sharedBuffer + 4,sum2);

  IO_Seek(CKSUM_HDRPOS, IO_SEEK_SET);
  IO_Write(sharedBuffer + 0, 1, 8);
  IO_Seek(0,IO_SEEK_SET);
  NEODSB_WriteTaskProgress(100);

  return 1;
}


