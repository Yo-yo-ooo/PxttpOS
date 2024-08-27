#pragma once

#include "ff.h"
#include "../vfs/vfs.h"



class fatfs{
public:
    int Init(int DiskNum);
    int open(const char* path, int mode);
    FILE* fopen(const char* name, const char* mode);
    int fclose(FILE *stream);
private:
    FILE _iob[OPEN_MAX];
    FIL files[OPEN_MAX];
    FATFS fs;
    int Tfd;
};