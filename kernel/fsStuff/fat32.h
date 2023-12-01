#include "fsStuff.h"
#include "../diskStuff/Disk_Interfaces/sata/sataDiskInterface.h"

typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;
/**
 * FAT文件系统的BPB结构
 */
typedef struct _bpb_t {
    uint8_t BS_jmpBoot[3];                 // 跳转指令
    uint8_t BS_OEMName[8];                 // OEM名称
    uint16_t BPB_BytsPerSec;               // 每扇区字节数
    uint8_t BPB_SecPerClus;                // 每簇扇区数
    uint16_t BPB_RsvdSecCnt;               // 保留区扇区数
    uint8_t BPB_NumFATs;                   // FAT表项数
    uint16_t BPB_RootEntCnt;               // 根目录项目数
    uint16_t BPB_TotSec16;                 // 总的扇区数
    uint8_t BPB_Media;                     // 媒体类型
    uint16_t BPB_FATSz16;                  // FAT表项大小
    uint16_t BPB_SecPerTrk;                // 每磁道扇区数
    uint16_t BPB_NumHeads;                 // 磁头数
    uint32_t BPB_HiddSec;                  // 隐藏扇区数
    uint32_t BPB_TotSec32;                 // 总的扇区数
} bpb_t;

/**
 * BPB中的FAT32结构
 */
typedef struct _fat32_hdr_t {
    uint32_t BPB_FATSz32;                  // FAT表的字节大小
    uint16_t BPB_ExtFlags;                 // 扩展标记
    uint16_t BPB_FSVer;                    // 版本号
    uint32_t BPB_RootClus;                 // 根目录的簇号
    uint16_t BPB_FsInfo;                   // fsInfo的扇区号
    uint16_t BPB_BkBootSec;                // 备份扇区
    uint8_t BPB_Reserved[12];
    uint8_t BS_DrvNum;                     // 设备号
    uint8_t BS_Reserved1;
    uint8_t BS_BootSig;                    // 扩展标记
    uint32_t BS_VolID;                     // 卷序列号
    uint8_t BS_VolLab[11];                 // 卷标名称
    uint8_t BS_FileSysType[8];             // 文件类型名称
} fat32_hdr_t;

bpb_t ReadBPB(AHCI::Port* port);