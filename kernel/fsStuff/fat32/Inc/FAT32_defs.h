//
// Created by user on 2021/9/24.
//

#ifndef FATSYSTEM_FAT32_DEFS_H
#define FATSYSTEM_FAT32_DEFS_H

#include <stdint.h>
#include <stddef.h>
#include <libm/cstrTools.h>
#include <libm/stubs.h>

#include "../../../diskStuff/Disk_Interfaces/generic/genericDiskInterface.h"
#include "../../../diskStuff/Disk_Interfaces/sata/sataDiskInterface.h"
#include "../../packdef.h"
#include "../../../osData/osData.h"
#include "../../../devices/rtc/rtc.h"



#define ERROR_CLUSTER 0xFEFEFEFE
#define END_CLUSTER 0x0FFFFFFF

#define ATTR_READ_ONLY 0x01
#define ATTR_HIDDEN    0x02
#define ATTR_SYSTEM    0x04
#define ATTR_VOLUME_ID 0x08
#define ATTR_DIRECTORY 0x10
#define ATTR_ARCHIVE   0x20
#define ATTR_LONG_NAME (ATTR_READ_ONLY | ATTR_HIDDEN | ATTR_SYSTEM | ATTR_VOLUME_ID)
#define DIR_ENTRY_FREE 0xE5
#define MAX_NAME_LENGTH  255


#define FAT32_LONG_NAME_END 0x40
#define FAT32_CMP(ch1,ch2)  (ch1 == ch2 ||( \
((ch1 >= 'a' && ch1 <= 'z') || (ch1 >= 'A' && ch1 <= 'Z')) && \
((ch2 >= 'a' && ch2 <= 'z') || (ch2 >= 'A' && ch2 <= 'Z')) && \
(ch1 + 0x20 == ch2 || ch1 == ch2 + 0x20)) )








#define SECTOR_SIZE  512
#define BUFF_SECTORS 8
#define CACHE_DIR  16
#define FAT_CACHE_FAT  16
#define FAT_CACHE_FAT_DEPTH 4
#define FAT_CACHE_FAT_HASH_MASK (FAT_CACHE_FAT / FAT_CACHE_FAT_DEPTH)

#define MAX_CACHE_SECTOR 16



#define FAT_MAX_LONG_NAME_LEN 256
#define FAT_MAX_PATH_LEN 1024
#define FAT_LONG_NAME_DIR_CAP 13


#define FAT32_END 0x0FFFFFFFU
#define FAT32_FREE 0


#define FAT_SUCCESS  0
#define FAT_IO_ERROR 0x01
#define FAT_INVALID_PARA 0x02
#define FAT_NOT_OPEN     0x03
#define FAT_ACCESS_ERROR 0x04
#define FAT_BOOT_ERROR   0x05
#define FAT_BUFFER_ERROR 0x06
#define FAT_INVALID_SECTOR 0x07
#define FAT_INVALID_MEDIA 0x08
#define FAT_NO_MORE_CONTENT 0x09
#define FAT_NO_MORE_SPACE 0x0A


#define FAT_NOT_FOUND 0x0A
#define FAT_UNKNOWN_ERROR 0xFFFF












#endif //FATSYSTEM_FAT32_DEFS_H
