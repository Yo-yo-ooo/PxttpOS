#pragma once

#include "ff.h"
#include "../vfs/vfs.h"

namespace fatfs{
    int Init(int DiskNum,const MKFS_PARM* opt);
    int open(const char* path, int mode);
    FILE* fopen(const char* name, const char* mode);
    int fclose(FILE *stream);
}