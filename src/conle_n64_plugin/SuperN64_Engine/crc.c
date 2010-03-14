#include "crc.h"
#include "../NEO_SN64Engine_DynaStatsShared.h"
//SHARED FROM DEDALUS PSP
static int crc_table_empty = 1;
static unsigned int crc_table[256];
void make_crc_table();

void make_crc_table()
{
  unsigned int c, n;
  int k;
  unsigned int poly;            /* polynomial exclusive-or pattern */
  /* terms of polynomial defining this crc (except x^32): */
  static const unsigned char p[] = {0,1,2,4,5,7,8,10,11,12,16,22,23,26};

  /* make exclusive-or pattern from polynomial (0xedb88320L) */
  poly = 0L;
  for (n = 0; n < sizeof(p)/sizeof(unsigned char); n++)
    poly |= 1L << (31 - p[n]);

  for (n = 0; n < 256; n++)
  {
    c = n;
    for (k = 0; k < 8; k++)
      c = c & 1 ? poly ^ (c >> 1) : c >> 1;
    crc_table[n] = c;
  }
  crc_table_empty = 0;
}

/* ========================================================================= */
#define DO1(buf) crc = crc_table[((int)crc ^ (*buf++)) & 0xff] ^ (crc >> 8);
#define DO2(buf)  DO1(buf); DO1(buf);
#define DO4(buf)  DO2(buf); DO2(buf);
#define DO8(buf)  DO4(buf); DO4(buf);

/* ========================================================================= */
unsigned int CRC_Calculate(unsigned int crc, const unsigned char* buf, unsigned int len)
{
    int prg = 0;
    const float sLen = (float)len;

    if (buf == (void*)0) return 0L;

    if (crc_table_empty)
      make_crc_table();

    NEODSB_WriteTaskName("CRC_Calculate CIC\0");
    crc = crc ^ 0xffffffffL;
    while (len >= 8)
    {
        NEODSB_SyncThread();
        NEODSB_WriteTaskProgress((int)(((float)(prg) / sLen) * 100));
        prg += 8;

      DO8(buf);
      len -= 8;
    }
    if (len) do {
      DO1(buf);
    } while (--len);

    NEODSB_WriteTaskProgress(100);
    return crc ^ 0xffffffffL;
}
