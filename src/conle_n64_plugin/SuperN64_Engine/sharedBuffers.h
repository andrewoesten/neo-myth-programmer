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

#ifndef SHAREDBUFFERS_H_
#define SHAREDBUFFERS_H_
#ifdef __cplusplus
extern "C" {
#endif
#include "memoryDef.h"
#define MD5_CODE_SIZE (32)
#define CHUNK_32K (32768)
#define WORKDIR_SIZE (256)
#define GOODN64_BUFFER_SIZE (64)
#define FILENAME_BUFFER_SIZE (512)

extern char MD5CodeBuffer[];
extern char chunkBuffer[];//for block read/writes
extern char workDirectoryBuffer[];
extern char goodN64NameBuffer[];
extern char filenameBuffer[];
extern char pathSeparatorBuffer[];

void SBFS_Clear();
void SBFS_ClearFast();
#ifdef __cplusplus
}
#endif
#endif // SHAREDBUFFERS_H_
