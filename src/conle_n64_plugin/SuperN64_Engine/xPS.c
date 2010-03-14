//Modified interface + removed code by conleon1988@gmail.com for  NEON64 Myth FC.
/***************************************************************************
 *                                                                         *
 *  xpsapply.c                                                             *
 *                                                                         *
 *  This program can be used to apply                                      *
 *                                                                         *
 *  - IPS (International Patching System)                                  *
 *  - APS (Advanced Patching System)                                       *
 *                                                                         *
 *  Version 1.00                                                           *
 *  Ravemax/Dextrose                                                       *
 *                                                                         *
 *  ---------------------------------------------------------------------  *
 *                                                                         *
 *  You can customize xPSApply and you are allowed to spread the modified  *
 *  versions, but please give me a credit (don't be a lamer).              *
 *                                                                         *
 ***************************************************************************/

#include "io.h"
#include "xPS.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

typedef unsigned char    u8;
typedef unsigned short   u16;
typedef unsigned long    u32;
/*
typedef char   bool;

#ifndef FALSE
   #define FALSE   0
#endif

#ifndef TRUE
   #define TRUE    (!FALSE)
#endif


const char *title_str =
   "\nxPS-Apply v1.00 (c) Ravemax of Dextrose\n";

const char *usage_str =
   "Usage: XPSAPPLY rom patch [-<option(s)>]\n\n"
   "<Options>\n"
   "  b: Make a backup before patching\n"
   "  l: Create a logfile\n";

const char *logfile_hdr_str =
   "Created with xPS-Apply\n"
   " \n"
   "[  Offset:     ³  RLE:  ³  Bytes:  ]\n"
   "ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄ¿\n";

 "ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ\n"
   " \n"
   "Modifications: %ld\n";const char *logfile_end_str =

const char *log_ext_str =
   ".LOG";

const char *backup_ext_str =
   ".ORG";
*/

static IOHandle /*FILE*/ *rom_file,*patch_file;//,*log_file;
//bool opt_backup = FALSE,opt_log = FALSE;
//long mod_cnt = 0;

#define ERROR_OPTION            0
#define ERROR_NOT_FOUND         1
#define ERROR_NO_PATCH          2
#define ERROR_NO_APS            3
#define ERROR_WRONG_ENDIANESS   4
#define ERROR_NOT_RIGHT_ROM     5

/*
void error_exit(char num,char *param)
{
  const char *error_base_str =
     "- Error  : %s %s !\n\n";

  const char *error_msg_str[] = {
     "contents a unknown option",
     "not found",
     "is no known patch-type (IPS or APS)",
     "is not a N64-specific APS-file",
     "has the wrong endianess (byte-swap)",
     "is not the right ROM"
  };

  printf(error_base_str,param,error_msg_str[num]);
  exit(num+2);
}

void parse_cmdline(char start_from,int argc,char *argv[])
{
   char *cptr;

   while (1)
   {
      argc--;
      if (argc < start_from)
         break;
      cptr = argv[argc]+1;

      while (*cptr)
      {
         switch(tolower(*cptr))
         {
            case 'b' :
               opt_backup = TRUE;
               break;
            case 'l' :
               opt_log = TRUE;
               break;
            case '?' :
               puts(usage_str);
               exit(1);
            default :
               error_exit(ERROR_OPTION,argv[argc]);
         }
         cptr++;
      }
   }
}
*/

/*bool*/int open_patch(char *fname)
{
   const char patch_id[2][5] = {
      "PATCH",
      "APS10"
   };

   char id_chk[5];

   patch_file = fopen(fname,"rb");
   if (patch_file == NULL)
   {
	   return 0;
      //printf("Couldn't open ROM: ");
      //perror("fopen");
      //exit(2);
   }
   IO_ReadSingle(id_chk,1,5,patch_file);
   if (!memcmp(id_chk,patch_id[0],5))
      return(1);
   else if (!memcmp(id_chk,patch_id[1],5))
      return(0);
  // else
     // error_exit(ERROR_NO_PATCH,fname);

   return 0;
}

/*
FILE *open_with_ext(char *fname,const char *ext)
{
   char ctemp[13];

   strcpy(ctemp,fname);
   if (strchr(ctemp,'.'))
      memcpy(strchr(ctemp,'.'),ext,strlen(ext)+1);
   else
      strcat(ctemp,ext);

   return(fopen(ctemp,"wb"));
}

void create_backup(char *fname)
{
   char *buffer;
   size_t numread;
   FILE *bakf;

   puts("- Create backup");

   buffer = (char *)malloc(32768);
   bakf = open_with_ext(fname,backup_ext_str);
   while (numread = IO_ReadSingle(buffer,1,32768,rom_file))
      IO_WriteSingle(buffer,1,numread,bakf);
   fclose(bakf);

   free(buffer);
   IO_SeekSingle(rom_file,0,IO_SEEK_SET);   // Some compiler doesnt support "rewind()"
}
*/
long fsize(IOHandle *file)
{
   long fs,fpos = IO_TellSingle(file);

   IO_SeekSingle(0,IO_SEEK_END,file);
   fs = IO_TellSingle(file);
   IO_SeekSingle(fpos,IO_SEEK_SET,file);
   return(fs);
}

void swap_bytes(u8 *data,size_t size)
{
   u8 temp;

   while (size > 1)
   {
      temp = *data;
      *data = data[1];
      data[1] = temp;

      data += 2;
      size -= 2;
   }
}

void validate_patch(char *rom_name,char *patch_name)
{
   u8 cpuf[51];
   int z64_endianess;
   long destsize;

   if ((IO_GetcSingle(patch_file) != 1) || (IO_GetcSingle(patch_file) != 0))
      return;//error_exit(ERROR_NO_APS,patch_name);
   IO_ReadSingle(cpuf,1,50,patch_file);
   cpuf[50] = 0;
   printf("- Info   : %s\n",cpuf);

   z64_endianess = IO_GetcSingle(patch_file);
   *cpuf = IO_GetcSingle(rom_file);

   if ((z64_endianess  && (*cpuf != 0x80)) ||
       (!z64_endianess && (*cpuf != 0x37)))
      return;//error_exit(ERROR_WRONG_ENDIANESS,rom_name);


   IO_ReadSingle(cpuf,1,11,patch_file);
   IO_SeekSingle(0x3C,IO_SEEK_SET,rom_file);   // Cart-ID+country-code
   IO_ReadSingle(cpuf+20,1,3,rom_file);
   if (!z64_endianess)
      cpuf[22] = IO_GetcSingle(rom_file);

   IO_SeekSingle(0x10,IO_SEEK_SET,rom_file);   // CRCs
   IO_ReadSingle(cpuf+23,1,8,rom_file);

   if (!z64_endianess)
   {
      swap_bytes(&cpuf[20],2);
      swap_bytes(&cpuf[23],8);
   }
   if (memcmp(cpuf,cpuf+20,11))
       return;//error_exit(ERROR_NOT_RIGHT_ROM,rom_name);

   IO_SeekSingle(74,IO_SEEK_SET,patch_file);   // Skip future-expansion
   IO_ReadSingle(&destsize,4,1,patch_file);
   if (destsize != fsize(rom_file))
      return;//error_exit(ERROR_NOT_RIGHT_ROM,rom_name);

   IO_SeekSingle(0,IO_SEEK_SET,rom_file);
}

#define UNCOMPRESSED   0
#define IPS_RLE        1
#define APS_RLE        2

void make_changes(long offset,u16 num,char type)
{
   u8 modval;
   u16 loop;

   IO_SeekSingle(offset,IO_SEEK_SET,rom_file);
   if (type == UNCOMPRESSED)
   {
      for (loop = num; loop; loop--)
         IO_PutcSingle(IO_GetcSingle(patch_file),rom_file);
   }
   else
   {
      if (type == IPS_RLE)
         modval = IO_GetcSingle(patch_file);
      else
      {
         modval = (u8)num;
         num = ((u8 *)&num)[1];
      }

      for (loop = num; loop; loop--)
         IO_PutcSingle(modval,rom_file);
   }

  // mod_cnt++;
   //printf("- Offset : 0x%lX",offset);
  // if (opt_log)
    //  fprintf(log_file,"³  0x%.8lX  ³  %s   ³  %5d   ³\n",
      //        offset,(type) ? "Yes" : "No ",num);
}

void handle_ips(void)
{
   #define BYTE3_TO_LONG(bp)   \
      (((long)(bp)[0] << 16) | ((long)(bp)[1] <<  8) | (long)(bp)[2])

   #define BYTE2_TO_WORD(bp)   \
      (((bp)[0] << 8) | (bp)[1])

   const u8 end_of_ips[3] = "EOF";

   u8 puffer[5];
   long offset;
   u16 count;

   while (1)
   {
      IO_ReadSingle(puffer,1,3,patch_file);
      if (!memcmp(puffer,end_of_ips,3))
         break;
      offset = BYTE3_TO_LONG(puffer);
      IO_ReadSingle(puffer,2,1,patch_file);
      count = BYTE2_TO_WORD(puffer);
      if (count)
         make_changes(offset,count,UNCOMPRESSED);
      else
      {
	 IO_ReadSingle(puffer,1,2,patch_file);
         make_changes(offset,BYTE2_TO_WORD(puffer),IPS_RLE);
      }
   }
}

void handle_aps(void)
{
   long offset,romsize;
   u8 count;
   u16 num;

   romsize = fsize(rom_file);
   while (IO_ReadSingle(&offset,4,1,patch_file))
   {
      if (romsize < offset)
         break;

      if ((count = IO_GetcSingle(patch_file)))
         make_changes(offset,(u16)count,UNCOMPRESSED);
      else
      {
         IO_ReadSingle(&num,1,2,patch_file);
         make_changes(offset,(u16)num,APS_RLE);
      }
   }
}


int SN64PLUG_xPS_IPS_Interface(IOHandle* src,IOHandle* dst)
{
	rom_file = src;
	patch_file = dst;

	handle_ips();
	return 0;
}

int SN64PLUG_xPS_APS_Interface(IOHandle* src,IOHandle* dst)
{
	rom_file = src;
	patch_file = dst;

	handle_aps();
	return 0;
}

/*
int main(int argc,char *argv[])
{
   puts(title_str);



   if (argc < 3)
   {
      puts(usage_str);
      return(1);
   }

   parse_cmdline(3,argc,argv);



   rom_file = fopen(argv[1],"rb+");
   if (rom_file == NULL)
   {
      printf("Couldn't open ROM: ");
      perror("fopen");
      exit(2);
   }

   if (opt_backup)
      create_backup(argv[1]);

   if (opt_log)
   {
      log_file = open_with_ext(argv[1],log_ext_str);
      fputs(logfile_hdr_str,log_file);
   }

   if (open_patch(argv[2]))
      handle_ips();
   else
   {
      validate_patch(argv[1],argv[2]);
      handle_aps();
   }


   if (opt_log)
      fprintf(log_file,logfile_end_str,mod_cnt);

   puts("\n\nDone ;-)\n");
   exit(0);
}
*/
