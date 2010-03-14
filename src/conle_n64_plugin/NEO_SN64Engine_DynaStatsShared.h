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

#ifndef __NEOSN64E_DS__
#define __NEOSN64E_DS__
#ifdef __cplusplus
extern "C" {
#endif
#include "BUILD_CFG.h"

#ifdef MADMONKEYS_OPEN_PROGRAMMER
#undef __IMPORT__SN64PLUG_CODE__
#else
#define __IMPORT__SN64PLUG_CODE__
#endif

#ifdef __IMPORT__SN64PLUG_CODE__
typedef struct NEON64DynamicStatsBlock NEON64DynamicStatsBlock;

struct NEON64DynamicStatsBlock
{
	char romGoodName[32 + 1];
	char currentTaskName[32 + 1];

	int progress;//100% means complete

	//these two must be used by the host app to decide whether to update info or not
	int romNameNeedsUpdate;
	int taskNameNeedsUpdate;
	int progressNeedsUpdate;

	//private data - can be accessed by the HOST APP too
	void* userData;//this will contain the rom header so dont bother
	void* userData2;//this will contain the MD5 so don't bother
};


void NEODSB_Bind(NEON64DynamicStatsBlock* dsb,void(*dsbUpdate)(NEON64DynamicStatsBlock* dsBlock),void (*syncFunc)());
void NEODSB_UnBind();
void NEODSB_WriteRomName(const char* name);
void NEODSB_WriteTaskName(const char* name);
void NEODSB_WriteTaskProgress(const int progress);
void NEODSB_SyncThread();
#else
#define NEODSB_Bind(_PA_,_PB_,_PC_)
#define NEODSB_UnBind()
#define NEODSB_WriteRomName(_PA_)
#define NEODSB_WriteTaskName(_PA_)
#define NEODSB_WriteTaskProgress(_PA_)
#define NEODSB_SyncThread()
#endif

#ifdef __cplusplus
}
#endif 
#endif
