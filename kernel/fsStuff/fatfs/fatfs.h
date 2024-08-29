#pragma once

#include "ff.h"
#include "../vfs/vfs.h"

namespace fatfs{
    int Init(int DiskNum,const MKFS_PARM* opt);
    int fatfs::open(const char *pathname, int flags);
    int fatfs::open(const char *pathname, int flags, unsigned int mode);
}