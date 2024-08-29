#include "fatfs.h"
#include "../../scheduler/scheduler.h"
#include <libm/cstrTools.h>
#include <libm/cstr.h>
#include <libm/stubs.h>


static FIL files[OPEN_MAX];
static FATFS fs;
#define Tfd Scheduler::CurrentRunningTask->Curfd
static BYTE work[FF_MAX_SS];
UINT br, bw;         /* File read/write count */

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

#define GET_BIT(value,bit) ((value)&(1<<(bit)))    //读取指定位
#define CPL_BIT(value,bit) ((value)^=(1<<(bit)))   //取反指定位

#define SET0_BIT(value,bit) ((value)&=~(1<<(bit))) //把某个位置0
#define SET1_BIT(value,bit) ((value)|= (1<<(bit))) //把某个位置1

int fatfs::open(const char *pathname, int flags){
    if(bf == 0){
        return (int)-1;
    }
    
    if(Tfd > 20 || Tfd < 3){
        return (int)-1;
    }
    Tfd += 1;
    files[Tfd] = {0};
    FRESULT res;
    
    if(GET_BIT(flags,15) == 1)
        res = f_open(&files[Tfd], path, FA_READ);
    else if(GET_BIT(flags,14) == 1)
        res = f_open(&files[Tfd], path, FA_WRITE);
    else if(GET_BIT(flags,13) == 1)
        res = f_open(&files[Tfd], path, FA_READ | FA_WRITE);
    else if(GET_BIT(flags,12) == 1)
        res = f_open(&files[Tfd], path, FA_CREATE_NEW);
    else if(GET_BIT(flags,8) == 1)
        res = f_open(&files[Tfd], path, FA_OPEN_APPEND);
    

    if(res != FR_OK){
        return (int)-1;
    }
    return Tfd;
}
int fatfs::open_(const char *pathname, int flags, unsigned int mode){
    
    if(bf == 0){
        return (int)-1;
    }
    
    if(Tfd > 20 || Tfd < 3){
        return (int)-1;
    }
    Tfd += 1;
    files[Tfd] = {0};
    FRESULT res;
    FRESULT res2 = f_chmod(pathname, mode, AM_RDO | AM_ARC);
    
    if(GET_BIT(flags,15) == 1)
        res = f_open(&files[Tfd], path, FA_READ);
    else if(GET_BIT(flags,14) == 1)
        res = f_open(&files[Tfd], path, FA_WRITE);
    else if(GET_BIT(flags,13) == 1)
        res = f_open(&files[Tfd], path, FA_READ | FA_WRITE);
    else if(GET_BIT(flags,12) == 1)
        res = f_open(&files[Tfd], path, FA_CREATE_NEW);
    else if(GET_BIT(flags,8) == 1)
        res = f_open(&files[Tfd], path, FA_OPEN_APPEND);
    

    if(res2 != FR_OK || res != FR_OK){
        return (int)-1;
    }
    return Tfd;
}

int write(int fd, const void *buf, size_t count){
    FRESULT fr;
    fr = f_write(&files[fd], buf, count, &bw);
    if(fr != FR_OK || bw == 0){
        return -1;
    }
    return 0;
}

int read(int fd, void *buf, size_t count){
    FRESULT fr;
    fr = f_read(&files[fd], buf, count, &br);
    if(fr != FR_OK || br == 0){
        return -1;
    }   
    return 0;

}