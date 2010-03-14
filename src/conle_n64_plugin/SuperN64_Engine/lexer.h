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

#ifndef LEXER_H_
#define LEXER_H_
#ifdef __cplusplus
extern "C" {
#endif 
#define __IMPORT_LEXER_C

#define LEX_SB_COUNT (16)
#define LEX_SB_LEN_PER_ITEM (32)
#define LEX_PHRASE_LEN (255)

extern int LEX_Result_Thresold;

typedef struct LEX_Stats LEX_Stats;

struct LEX_Stats
{
    char token[LEX_SB_LEN_PER_ITEM + 1];
    unsigned char __pad0__;
    unsigned char single;
    unsigned char __pad1__;
    short length;
};

int LEX_PreCompileSymbolTable(const char* name);
int LEX_PhraseMakesSense(const char* dbLink);
#ifdef __cplusplus
}
#endif 
#endif // LEXER_H_
