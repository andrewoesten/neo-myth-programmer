#ifndef neoutil_g
#define neoutil_g
// #########################################################################

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0501
#include <windows.h>
#else
#include <unistd.h>
#if 1//have stdint
#include <stdint.h>
typedef uint32_t DWORD;
typedef uint16_t WORD;
typedef uint8_t BYTE;
#else
typedef unsigned int DWORD;
typedef unsigned short WORD;
typedef unsigned char BYTE;
#endif
#endif
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// #########################################################################

const char dDelim0='/';
#ifdef WIN32
const char dDelim1='\\';
#else
const char dDelim1='/';
#endif

// #########################################################################

#define Min(x,y) (((x)<(y))?(x):(y))
#define Max(x,y) (((x)>(y))?(x):(y))
#define Fix(X,A) (((X)%(A))?((X)-((X)%(A))+(A)):(X))
#define _2byte(x) ((BYTE)((x)&0xff))
#define _2str(x) #x
#define sorted(a,b) a[a ## _index[b]]
#define check(x) if((x)==0){printf("\n   ***   error: %s failed in line %i of %s\n",_2str(x),__LINE__,__FILE__); return 0;}
#define KB 1024
#define MB (KB*KB)
#define Kb (KB/8)
#define Mb (MB/8)

// #########################################################################

typedef int (*sortcallback)(void*data,DWORD a,DWORD b);

// #########################################################################

enum
{
    state_undefined,
    state_loading,
    state_ready
};

// #########################################################################

class membuf
{
    char*data;
    int size;
public:
    membuf();
    membuf(int sz);
    ~membuf();
    int resize(int sz);
    void free();
    operator char* () const;
};

// #########################################################################

typedef BYTE md5struct[16];

// #########################################################################

enum
{
    enableKnownPatches,
    savePatchedFiles,
    ioBlockSize,
    file_ioBlockSize,
    romAlignment,
    displaySzMb
};

// #########################################################################

class progress_indicator
{
private:
    int progress;
    int total;
    int ipause;
    int istop;
    DWORD ticks;
    membuf textDesc;
    progress_indicator*glbprg;
    progress_indicator*subprg;
    void update();
    void setGlbProgress(progress_indicator*prg);
protected:
    virtual void onUpdate(char*desc,int pr,int rng);
public:
    progress_indicator();
    virtual ~progress_indicator();
    void setText(const char*desc);
    void reset();
    int doWork(int d);
    void incWork(int d);
    void setWork(int d);
    void pause(int p);
    void stop();
    void wait();
    void done(const char*desc);
    void setSubProgress(progress_indicator*prg);
    progress_indicator*subProgress();
    int getProgress();
};

// #########################################################################

void getoption(int option,void*data);
int getoption(int option);
void setoption(int option,void*data);
int SaveFile(const char*FileName,char*X,DWORD fs);
void LoadFile(const char*FileName,char*X,DWORD&fs);
void LoadFile(const char*FileName,membuf&X,DWORD&fs);
#ifndef WIN32
void Sleep(DWORD msec);
#endif
int file_exists(const char*FileName);
int atoiX(char*str,int ibase,int&err);
void load_word_le(void*dv,WORD&x);
void load_dword_le(void*dv,DWORD&x);
void store_word_le(WORD x,void*dv);
void store_dword_le(DWORD x,void*dv);
void load_word_be(void*dv,WORD&x);
void load_dword_be(void*dv,DWORD&x);
void store_word_be(WORD x,void*dv);
void store_dword_be(DWORD x,void*dv);
char*gettmpbuffer(DWORD size);
int print_size(char*buffer,DWORD size,DWORD block);
char*print_size(DWORD size,DWORD block);
char*print_size(DWORD size);
int testPathMask(const char*path,const char*mask);
void quickSort(DWORD*index,DWORD p,DWORD r,sortcallback sortf,void*data);
void getmd5(const void*data,const int size,md5struct&md5);
void updatemd5(const void*data,const int size,md5struct&md5);
const char*getFileName(const char*fname);
char*getShortName(const char*fname);

// #########################################################################
#endif
