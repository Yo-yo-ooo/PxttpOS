#include "../packdef.h"
#include <stdint.h>
#include <stddef.h>
#include <libm/cstrTools.h>
#include <libm/stubs.h>
#include "../../diskStuff/Disk_Interfaces/generic/genericDiskInterface.h"
#include "../../diskStuff/Disk_Interfaces/sata/sataDiskInterface.h"
#include "./Inc/FAT32_struct.h"

class Fat32{
public:
    Fat32(int DiskNum);
    bool CreateFile(char *name_ptr);
    bool DeleteFile(char *name_ptr);
    bool ReadFile(char*name_ptr,unsigned int offset,void *buffer,unsigned read_size);
    bool WriteFile(char*name_ptr,unsigned int offset,void *buffer,unsigned read_size);

    bool CreateDir(char *name_ptr);
    bool DeleteDir(char *name_ptr);
private:
    struct MEDIA_DISK media_ptr;
    struct FAT32_MEDIA media;
    unsigned int max_size = 256 * 1024 * 1024;
};