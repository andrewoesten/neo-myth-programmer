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

/*
    LIB_LEX V1.4
    Rom name Lexxer , Compiler & Analyzer V1.4 (c)2010 conleon1988@gmail.com for NEO MYTH SERIES PROJECT
    Combines words ands checks to see if they can make up a whole phrase in multiple combinations
*/

#include <stdio.h>
#include "lexer.h"
#ifdef __IMPORT_LEXER_C

#include "sharedBuffers.h"
#include "utility.h"
#include "../NEO_SN64Engine_DynaStatsShared.h"

static LEX_Stats LEX_SB[LEX_SB_COUNT + 1];
static int LEX_SB_Entries = -1;
static int LEX_SB_EntriesLength = 0;
static char LEX_Phrase[LEX_PHRASE_LEN + 1];

int LEX_Result_Thresold = 88;

void LEX_Internal_InvalidateAllFast()
{
    int i;
    LEX_Stats* p;

    NEODSB_WriteTaskName("LEX_Internal_InvalidateAllFast()\0");
    LEX_Phrase[0] = '\0';
    LEX_SB_EntriesLength = 0;

    for(i = 0; i < LEX_SB_COUNT; i++)
    {
        p = (LEX_SB + i);

        *(unsigned char*)(p + 0) = '\0';
        *(int*)(p + ((LEX_SB_LEN_PER_ITEM + 1)+1)) = 0;
    }
}

static int LEX_Internal_CompileSymbol(const char* word)
{
    //FILE* f;
    LEX_Stats* p;
    int q;
    int qA;

    NEODSB_WriteTaskName("LEX_Internal_CompileSymbol()\0");

    if(LEX_SB_Entries >= LEX_SB_COUNT)
        return 0;

    p = (LEX_SB + (LEX_SB_Entries++));

    q = qA = 0;

    //f = fopen("c:\\word.txt","at");

//  if(!f)
//      f = fopen("c:\\word.txt","wt");


    while(*word)
    {
        if(q > LEX_SB_LEN_PER_ITEM)
            break;

        switch(*word)
        {
            case '\'':
            case 'º':
            case 'é':
            case '&':
            case '!':
            {
                *(p->token + (q++)) = *word;
                ++word;
                continue;
            }
        }

        if(*word == '-')
        {
            ++qA;
            break;
        }
        else if(UTIL_IsSpace(*word))
        {
            ++qA;
            break;
        }
        else if(UTIL_IsAlpha(*word))
        {
            while(*word)
            {
                if(q > LEX_SB_LEN_PER_ITEM)
                    break;

                switch(*word)
                {
                    case '\'':
                    case 'º':
                    case 'é':
                    case '&':
                    case '!':
                    {
                        *(p->token + (q++)) = *word;
                        ++word;
                        continue;
                    }
                }

                if(!UTIL_IsAlpha(*word))
                    break;

                *(p->token + (q++)) = *word;

                ++word;
            }

            break;
        }
        else if(UTIL_IsNumerical(*word))
        {
            while(*word)
            {
                if(q > LEX_SB_LEN_PER_ITEM)
                    break;

                if(!UTIL_IsNumerical(*word))
                    break;

                *(p->token + (q++)) = *word;

                ++word;
            }

            break;
        }

        ++word;
    }

    *(p->token + q) = '\0';
    p->length = q;

    //fwrite(p->token,1,p->length,f);
    //fwrite("\n",1,1,f);
    //fclose(f);

    if( q > 1)
        p->single = 0;
    else
        p->single = 1;

    LEX_SB_EntriesLength += p->length;

    return q + qA;
}

static int LEX_Internal_PhraseLinkage()
{
    char* phrase = LEX_Phrase;
    int i;
    int rate;
    int succ;
    int l;
    int prg;
    char* addr;

    NEODSB_WriteTaskName("LEX_Internal_PhraseLinkage()\0");
    //printf("\n\nCALL LEX_Internal_PhraseLinkage\n\n");

    succ = 1;
    prg = 0;
    addr = phrase;

    for(i = 0; i < LEX_SB_Entries; i++)
    {
        if((LEX_SB + i)->single)
            addr = UTIL_StringFind(addr,(LEX_SB + i)->token);
        else
            addr = UTIL_StringFindAnyCase(addr,(LEX_SB + i)->token);

        if(!addr)
        {
            succ = 0;
            break;
        }

        addr += (LEX_SB + i)->length;
        prg += (LEX_SB + i)->length;
        NEODSB_WriteTaskProgress((int)(((float)prg / (float)LEX_SB_EntriesLength) * 100));
    }

    //Reverse
    //printf("\n\nCALL LEX_Internal_PhraseLinkage REVERSE\n\n");

    if(succ)
        return 1;


    l = UTIL_StringLengthConst(phrase);
    addr = phrase + l;
    prg = 0;

    for(i = (LEX_SB_Entries-1); i > -1 ; i--)
    {
        if((LEX_SB + i)->single)
            addr = UTIL_StringFindReverse(addr+l,(LEX_SB + i)->token+(LEX_SB + i)->length);
        else
            addr = UTIL_StringFindReverse(addr+l,(LEX_SB + i)->token+(LEX_SB + i)->length);

        if(!addr)
            break;

        addr -= (LEX_SB + i)->length;

        prg += (LEX_SB + i)->length;
        NEODSB_WriteTaskProgress((int)(((float)prg / (float)LEX_SB_EntriesLength) * 100));
    }

    if(succ)
        return 1;

    //"Blind" (slow)

    rate = 0;
    prg = 0;

    for(i = 0; i < LEX_SB_Entries; i++)
    {
        if((LEX_SB + i)->single)
        {
            if(!UTIL_StringFindConst(phrase,(LEX_SB + i)->token))
                break;

            rate += (int)( ((float)(LEX_SB + i)->length / (float)LEX_SB_EntriesLength) * 100 );
        }
        else
        {
            if(!UTIL_StringFindAnyCaseConst(phrase,(LEX_SB + i)->token))
                break;

            rate += (int)( ((float)(LEX_SB + i)->length / (float)LEX_SB_EntriesLength) * 100 );
        }

        prg += (LEX_SB + i)->length;
        NEODSB_WriteTaskProgress((int)(((float)prg / (float)LEX_SB_EntriesLength) * 100));
    }

    if(rate > LEX_Result_Thresold)
        return 1;

    return 0;
}

//Compile symbol table once
int LEX_PreCompileSymbolTable(const char* name)
{
    const char* f;
    const char* p;

    LEX_SB_EntriesLength = 0;

    NEODSB_WriteTaskName("LEX_PreCompileSymbolTable()\0");

    LEX_SB_Entries = 0;

    LEX_Internal_InvalidateAllFast();

    if(!name)
    {
        LEX_SB_Entries = -1;
        return 0;
    }

    p = UTIL_StringFindLastCharConst(name,pathSeparatorBuffer[0]);

    if(p)
        f = p + 1;
    else
        f = name;

    while(*f)
    {
        if(*f == '.')
            break;
        //else if(*f == '!')
            //break;
        else if(*f == '(')
            break;
        else if(*f == '[')
            break;
        else if(*f == ',')
        {
            ++f;
            continue;
        }
        else if(*f == '-')
        {
            ++f;
            continue;
        }
        else if(UTIL_IsSpace(*f))
        {
            while(*f)
            {
                if(!UTIL_IsSpace(*f))
                    break;

                ++f;
            }

            continue;
        }
        else if(*f == 'º')
        {
            f += LEX_Internal_CompileSymbol(f);
            continue;
        }
        else if(*f == '\'')
        {
            f += LEX_Internal_CompileSymbol(f);
            continue;
        }
        else if(*f == '&')
        {
            f += LEX_Internal_CompileSymbol(f);
            continue;
        }
        else if(*f == 'é')
        {
            f += LEX_Internal_CompileSymbol(f);
            continue;
        }
        else if(*f == '!')
        {
            f += LEX_Internal_CompileSymbol(f);
            continue;
        }
        else if(UTIL_IsAlpha(*f))
        {
            f += LEX_Internal_CompileSymbol(f);
            continue;
        }
        else if(UTIL_IsNumerical(*f))
        {
            f += LEX_Internal_CompileSymbol(f);
            continue;
        }

        ++f;
    }

    return 1;
}

int LEX_PhraseMakesSense(const char* dbLink)
{
    int n;

    NEODSB_WriteTaskName("EX_PhraseMakesSense()\0");

    if(LEX_SB_Entries < 0)
        return 0;

    while(*dbLink)
    {
        n = 0;

        while(*dbLink)
        {
            if( n > LEX_PHRASE_LEN )
                break;

            if(*dbLink == '\n')
                break;
            else if(*dbLink == '\r')
                break;
            //else if(*dbLink == '\t')
              //  break;

            LEX_Phrase[n++] = *dbLink;

            ++dbLink;
        }

        LEX_Phrase[n] = '\0';

        if(LEX_Internal_PhraseLinkage())
        {
            NEODSB_WriteTaskProgress(100);
            return 1;
        }

        ++dbLink;
    }

    NEODSB_WriteTaskProgress(100);

    return 0;
}

#endif
