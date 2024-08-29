#pragma once

#include "ff.h"
#include "../vfs/vfs.h"

namespace fatfs{
    int Init(int DiskNum,const MKFS_PARM* opt);
    int open(const char *pathname, int flags);
    int open_(const char *pathname, int flags, unsigned int mode);
}