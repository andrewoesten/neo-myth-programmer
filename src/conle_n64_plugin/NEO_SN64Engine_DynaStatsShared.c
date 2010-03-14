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

#include "NEO_SN64Engine_DynaStatsShared.h"
#ifdef __IMPORT__SN64PLUG_CODE__
#include "SuperN64_Engine/utility.h"

static NEON64DynamicStatsBlock* bindedDSB = NULL;
static void(*bindedDSBUpdateFunc)(NEON64DynamicStatsBlock* dsBlock) = NULL;
static void (*NEODSB_SYNC_FUNC)() = NULL;

void NEODSB_Bind(NEON64DynamicStatsBlock* dsb,void(*dsbUpdate)(NEON64DynamicStatsBlock* dsBlock),void (*syncFunc)())
{
    bindedDSB = dsb;
    NEODSB_SYNC_FUNC = syncFunc;
    bindedDSBUpdateFunc = dsbUpdate;

    if(bindedDSB)
    {
        bindedDSB->taskNameNeedsUpdate =
        bindedDSB->romNameNeedsUpdate =
        bindedDSB->progressNeedsUpdate = 0;

        bindedDSB->progress = 0;
        bindedDSB->romGoodName[0] =
        bindedDSB->currentTaskName[0] = '\0';
    }

}

void NEODSB_UnBind()
{
    bindedDSB = NULL;
    NEODSB_SYNC_FUNC = NULL;
    bindedDSBUpdateFunc = NULL;
}


void NEODSB_WriteRomName(const char* name)
{
    const char* p;

    if(!bindedDSB)
        return;

    if(!bindedDSBUpdateFunc)
        return;

    if(!name)
        return;

    switch(name[0])
    {
        case '\0':
        case ' ':
        {
            bindedDSB->romNameNeedsUpdate = 0;
            return;
        }
    }

    if(name[0] == bindedDSB->romGoodName[0])
    {
        if(name[1] == bindedDSB->romGoodName[1])
        {
            bindedDSB->romNameNeedsUpdate = 0;
            return;
        }
    }

    p = UTIL_StringFindLastConst(name,pathSeparatorBuffer[0]);

    if(p)
        UTIL_CopyString(bindedDSB->romGoodName,p+1,"\0");
    else
        UTIL_CopyString(bindedDSB->romGoodName,name,"\0");

    bindedDSB->romNameNeedsUpdate = 1;
    bindedDSBUpdateFunc(bindedDSB);
    bindedDSB->romNameNeedsUpdate = 0;
}

void NEODSB_WriteTaskName(const char* name)
{
    if(!bindedDSB)
        return;

    if(!bindedDSBUpdateFunc)
        return;

    if(!name)
        return;

    switch(name[0])
    {
        case '\0':
        case ' ':
        {
            bindedDSB->taskNameNeedsUpdate = 0;
            return;
        }
    }

    if(name[0] == bindedDSB->currentTaskName[0])
    {
        if(name[1] == bindedDSB->currentTaskName[1])
        {
            bindedDSB->taskNameNeedsUpdate = 0;
            return;
        }
    }

    UTIL_CopyString(bindedDSB->currentTaskName,name,"\0");

    bindedDSB->taskNameNeedsUpdate = 1;
    bindedDSBUpdateFunc(bindedDSB);
    bindedDSB->taskNameNeedsUpdate = 0;
}

void NEODSB_WriteTaskProgress(const int progress)
{
    if(!bindedDSB)
        return;

    if(!bindedDSBUpdateFunc)
        return;

    if(progress == bindedDSB->progress)
    {
        bindedDSB->progressNeedsUpdate = 0;
        return;
    }

    bindedDSB->progress = progress;
    bindedDSB->progressNeedsUpdate = 1;
    bindedDSBUpdateFunc(bindedDSB);
    bindedDSB->progressNeedsUpdate = 0;
}

void NEODSB_SyncThread()
{
    if(!NEODSB_SYNC_FUNC)
        return;

    NEODSB_SYNC_FUNC();
}
#endif
