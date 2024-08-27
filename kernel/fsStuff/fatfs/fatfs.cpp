#include "fatfs.h"
#include <libm/cstrTools.h>
#include <libm/cstr.h>
#include <libm/stubs.h>


static FIL files[OPEN_MAX];
static FATFS fs;
static int Tfd;
static BYTE work[FF_MAX_SS];

int fatfs::Init(int DiskNum,const MKFS_PARM* opt){
    Tfd = 3;
    if(DiskNum > 9 || DiskNum < 0)
        return -1;
    char p[3];
    p[0] = to_string(DiskNum);
    p[1] = ":";
    p[3] = "\0";
    if(f_mkfs(p,opt,work, sizeof(work)) != FR_OK)
        return -1;
    else if(f_mount(&fs, p, 0) != FR_OK)
        return -1;
    else
        return 0;
}

int fatfs::open(const char* path, int mode){
    if(Tfd > 20 || Tfd <= 3){
        return (int)-1;
    }
    Tfd += 1;
    files[Tfd] = {0};
    FRESULT res = f_open(&files[Tfd], path, mode);
    if(res != FR_OK){
        return (int)-1;
    }
    return Tfd;
}

FILE* fatfs::fopen(const char* name, const char* mode){
    int fd;
    FILE *fp;
    for(fp = _iob; fp < _iob + OPEN_MAX; fp++)
        if((fp->flag & (_READ | _WRITE)) == 0)
            break;
    if(fp >= _iob + OPEN_MAX)
        return NULL;

    if(StrEquals(mode,"w"))
        fd = open(name, (FA_CREATE_ALWAYS | FA_WRITE));
    else if(StrEquals(mode,"a"))
        fd = open(name, (FA_OPEN_APPEND | FA_WRITE));
    else if(StrEquals(mode,"r"))
        fd = open(name, FA_READ);
    else if(StrEquals(mode,"r+"))
        fd = open(name, (FA_READ | FA_WRITE));
    else if(StrEquals(mode,"w+"))
        fd = open(name, (FA_CREATE_ALWAYS | FA_WRITE | FA_READ));
    else if(StrEquals(mode,"a+"))
        fd = open(name, (FA_OPEN_APPEND | FA_WRITE | FA_READ));
    else if(StrEquals(mode,"wx"))
        fd = open(name, (FA_CREATE_NEW | FA_WRITE));
    else if(StrEquals(name,"w+x"))
        fd = open(name, (FA_CREATE_NEW | FA_WRITE | FA_READ));
    else
        return NULL;

    if(fd = -1)
        return NULL;
    fp->fd = fd;
    fp->cnt = 0;
    fp->base = NULL;
    fp->flag = (*mode == 'r') ? _READ : _WRITE;

    return fp;
}

int fatfs::fclose(FILE *stream){
    UINT bw;
    FRESULT res = f_write(&files[stream->fd],stream->base,sizeof(stream->base),&bw);
    if(bw < sizeof(stream->base) || res != FR_OK)
        return EOF;
    _Free(stream->base);
    f_close(&files[stream->fd]);

    return 0;
}