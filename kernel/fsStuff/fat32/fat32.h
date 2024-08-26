#include "../packdef.h"
#include <stdint.h>
#include <stddef.h>
#include <libm/cstrTools.h>
#include <libm/stubs.h>
#include "../../diskStuff/Disk_Interfaces/generic/genericDiskInterface.h"
#include "../../diskStuff/Disk_Interfaces/sata/sataDiskInterface.h"
#include "./Inc/FAT32_struct.h"

enum Fat32_FOpenFlags{
    _READ = 01,
    _WRITE = 02,
    _UNBUF = 04,
    _EOF = 010,
    _ERR = 020
};

struct _iobuf {
    char *_ptr;
    int   _cnt;
    char *_base;
    int   _flag;
    int   _file;
    int   _charbuf;
    int   _bufsiz;
    char *_tmpfname;
};
typedef struct _iobuf FILE;

class Fat32{
public:
    Fat32(int DiskNum);
    bool CreateFile(char *name_ptr);
    bool DeleteFile(char *name_ptr);
    bool ReadFile(char*name_ptr,unsigned int offset,void *buffer,unsigned read_size);
    bool WriteFile(char*name_ptr,unsigned int offset,void *buffer,unsigned read_size);

    bool CreateDir(char *name_ptr);
    bool DeleteDir(char *name_ptr);

    int open(const char *pathname, int flag);
    int creat(char* name, int perms);

    FILE *fopen( const char *filename, const char *mode );
private:
    unsigned char buffer[10240];
    struct MEDIA_DISK media_ptr;
    struct FAT32_MEDIA media;
    unsigned int max_size = 0;
    int Tfd;
    FILE _iob[20];
};