#pragma once

#include "../packdef.h"
#include <stdint.h>
#include <stddef.h>

#define CreateTFSBlockStructure(TFSsize)  \
PACK(typedef struct TFSB{                 \
    bool HaveNextBlock;                   \
    uint64_t NextBlockAddr;               \
    char data[TFSsize - 5];               \
})RBFSBlock;                              \

PACK(typedef struct RecursiveBlockFileSystem_FileCreateTime{
    unsigned int Hour : 5;
    unsigned int Minute : 6;
    unsigned int Second : 5; //秒/2
}) RBFSCT;

PACK(typedef struct RecursiveBlockFileSystem_FileCreateDate{
    unsigned int Year : 7; //年号-1980
    unsigned int Month : 4; //月份
    unsigned int Day : 5; //日期数
}) RBFSCD;

PACK(typedef struct TFSF{
    RBFSCT CreateTime;
    RBFSCD CreateDate;
    RBFSCD LastAccessDate;
    RBFSCT NearlyAccessTime;
    RBFSCD NearlyAccessDate;
    uint64_t FileSize;
    uint64_t StartBlockAddr;
    char FileAttr;
    short FileName[12];
    short Ext[6];
    bool ILSectorTS;
})RBFSFile;

enum ReadFault{
    NoAnyCanDo,
    DiskReadFault,
    EmptyFile,
    Success
};