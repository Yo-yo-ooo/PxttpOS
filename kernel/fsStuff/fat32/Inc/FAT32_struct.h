//
// Created by user on 2021/5/25.
//

#ifndef FAT32_FS_FAT32_STRUCT_H
#define FAT32_FS_FAT32_STRUCT_H

#include "FAT32_defs.h"

typedef enum {
    INIT,
    UNINIT,
    READ_BOOT,
    WRITE_BOOT,
    READ,
    WRITE,
    FLUSH
} FAT32_REQUEST;


PACK(struct FAT32_BOOT {
    unsigned char jump_boot[3];
    unsigned char OEM_name[8];
    unsigned short bytes_per_sector;
    unsigned char sectors_per_cluster;
    unsigned short reserved_sectors;
    unsigned char number_of_FAT;
    unsigned short unused;
    unsigned short unused1;
    unsigned char media_id;
    unsigned short unused2;
    unsigned short sectors_per_track;
    unsigned short number_of_heads;
    unsigned int hidden_sectors;
    unsigned int total_sectors;
    unsigned int sectors_per_FAT;
    unsigned short ext_flag;
    unsigned short fs_version;
    unsigned int root_cluster;
    unsigned short sectors_per_FSINFO;
    unsigned short BkBookSec;
    unsigned char reserved2[12];
    unsigned char reserved_for_os;
    unsigned char reserved3;
    unsigned char boot_sign;
    unsigned int volume_id;
    unsigned char volume_lab[11];
    unsigned char file_system_type[8];
});

PACK(struct FAT32_FSInfo {
    unsigned int lead_sign;
    unsigned int reserved[120];
    unsigned int struct_sign;
    unsigned int free_count;
    unsigned int next_free;
    unsigned int reserved2[3];
    unsigned int trail_sign;
});

PACK(typedef struct FAT32_T{
    uint32_t sec : 5;
    uint32_t min : 6;
    uint32_t hour : 5;
})FAT32_Time;

PACK(typedef struct FAT32_D{
    uint32_t day : 5;
    uint32_t month : 4;
    uint32_t year : 7;
})FAT32_Date;

PACK(struct FAT32_DIR {
    unsigned char DIR_name[11];
    unsigned char DIR_Attr;
    unsigned char reserved[8];
    unsigned short first_cluster_h;
    unsigned int reserved2;
    unsigned short first_cluster_l;
    unsigned int file_size;

    FAT32_Time CreationT;
    FAT32_Date CreationD;
    FAT32_Date LastAccessD;
});

PACK(struct FAT32_DIR_long_name {
    unsigned char order;
    unsigned char name1[10];
    unsigned char DIR_attr;
    unsigned char type;
    unsigned char check_sum;
    unsigned char name2[12];
    unsigned short reserved;
    unsigned char name3[4];
});

struct FAT32_CACHE_SECTOR {
    unsigned short valid;
    unsigned short dirty;
    unsigned int sector_number;
    void *cache_memory_ptr;
    struct FAT32_CACHE_SECTOR *next_used_cache_sector;
};


struct FAT32_CACHE_FAT {
    unsigned int cached_FAT_cluster;
    unsigned int cached_FAT_value;
    unsigned int cached_FAT_dirty;
};

struct FAT32_PATH {
    struct FAT32_DIR path_dir;
    char path_string[FAT_MAX_PATH_LEN];   //路径名
    char path_name_buffer[FAT_MAX_LONG_NAME_LEN]; //文件名
    unsigned long path_sector;
    unsigned int path_entry;
};

//添加底层设备相关类
//通过此抽象同一FAT_driver
//既可应用于用户态的文件
//同样可直接操作底层设备
struct MEDIA_DISK {
    char *media_name;
    unsigned int max_size;
    int ID;
    unsigned int read_times;
    unsigned int write_times;
    unsigned int status;
    int reserved;

    int (*open)(struct MEDIA_DISK *media_ptr);

    int (*read)(struct MEDIA_DISK *media_ptr, unsigned int offset, void *dest, int length);

    int (*write)(struct MEDIA_DISK *media_ptr, unsigned int offset, void *source, int length);

    void (*close)(struct MEDIA_DISK *media_ptr);
};


struct FAT32_MEDIA {

    struct FAT32_BOOT boot;
    struct FAT32_PATH path;

    struct FAT32_PATH
            last_found;

    struct FAT32_CACHE_SECTOR
            cache_sectors[MAX_CACHE_SECTOR];
    unsigned int cache_sector_size;
    struct FAT32_CACHE_SECTOR
            *cache_sector_list;


    struct FAT32_CACHE_FAT
            cache_FATs[FAT_CACHE_FAT];
    unsigned int dirty_sectors;

    void *memory_buff;
    void *memory_cache_sector_end;
    unsigned int memory_size;

    char name_buff[FAT_MAX_LONG_NAME_LEN * 4];


    void *operate_buff;
    unsigned int boot_sectors;
    unsigned int operate_sectors_start;
    unsigned int operate_sectors;

    FAT32_REQUEST request;
    unsigned int status;

    unsigned long cache_hit;
    struct MEDIA_DISK *disk;

    void (*driver)(struct FAT32_MEDIA *media_ptr);
};


typedef struct FAT32_MEDIA FAT32_MEDIA;
typedef struct MEDIA_DISK MEDIA_DISK;
typedef struct FAT32_DIR FAT32_DIR;


unsigned int FAT32_media_open(FAT32_MEDIA *media_ptr, MEDIA_DISK *disk_ptr,
                              void (*driver)(FAT32_MEDIA *), void *memory_buffer,
                              unsigned int memory_size);

unsigned int FAT32_media_close(FAT32_MEDIA *media_ptr);

unsigned int FAT32_media_flush(FAT32_MEDIA *media_ptr);

unsigned int FAT32_directory_search(FAT32_MEDIA *media_ptr, char *name_ptr, FAT32_DIR *entry_dir);

char *FAT32_directory_name_extract(char *source_ptr, char *dest_ptr);

unsigned int FAT32_directory_entry_read(FAT32_MEDIA *media_ptr, FAT32_DIR *source_dir, unsigned long *entry_ptr,
                                        FAT32_DIR *dest_dir);
unsigned int FAT32_directory_write(FAT32_MEDIA *media_ptr,FAT32_DIR *dest_dir,char *name_ptr,unsigned char ATTR,unsigned int local_cluster,unsigned int size);


unsigned int FAT32_directory_create(FAT32_MEDIA *media_ptr, char *name_ptr);

unsigned int FAT32_directory_delete(FAT32_MEDIA *media_ptr, char *name_ptr);
//dir_offset:在sour_dir中，空闲空间相对起始点的偏移的struct FAT32_DIR数
unsigned int FAT32_directory_free_find(FAT32_MEDIA *media_ptr,FAT32_DIR *source_dir,unsigned int free_dir_size,unsigned int *dir_offset_ptr);


unsigned int FAT32_file_create(FAT32_MEDIA *media_ptr,char *name_ptr);

unsigned int FAT32_file_delete(FAT32_MEDIA *media_ptr,char *name_ptr);

unsigned int FAT32_file_read(FAT32_MEDIA *media_ptr,char*name_ptr,unsigned int offset,void *buffer,unsigned read_size);


unsigned int FAT32_file_write(FAT32_MEDIA *media_ptr,char*name_ptr,unsigned int offset,void *buffer,unsigned write_size);



unsigned short FAT32_utility_16_unsigned_read(void *source_ptr);

unsigned int FAT32_utility_32_unsigned_read(void *source_ptr);

void FAT32_utility_16_unsigned_write(void *dest_ptr, unsigned short value);

void FAT32_utility_32_unsigned_write(void *dest_ptr, unsigned int value);

unsigned int FAT32_utility_alloc_cluster(FAT32_MEDIA *media_ptr, unsigned int last_cluster, unsigned int *cluster_ptr);

unsigned int FAT32_utility_boot_info_extract(FAT32_MEDIA *media_ptr, void *source);

void FAT32_utility_memory_copy(void *src, void *dest, unsigned int length);

void FAT32_utility_memory_set(void *src, unsigned int c, unsigned int length);

unsigned int FAT32_utility_logic_sector_read(FAT32_MEDIA *media_ptr, void *buffer, unsigned int logic_sector,
                                             unsigned int read_sectors);

unsigned int FAT32_utility_logic_sector_write(FAT32_MEDIA *media_ptr, void *buffer, unsigned int logic_sector,
                                              unsigned int write_sectors);

struct FAT32_CACHE_SECTOR *
FAT32_utility_logic_sector_cache_entry_read(FAT32_MEDIA *media_ptr, unsigned int logic_sector,
                                            struct FAT32_CACHE_SECTOR **previous_cache_sector);

unsigned int FAT32_utility_logic_sector_flush(FAT32_MEDIA *media_ptr, unsigned int logic_sector, unsigned int sectors,
                                              unsigned int invalid);

unsigned int FAT32_utility_FAT_flush(FAT32_MEDIA *media_ptr);

unsigned int FAT32_utility_FAT_read(FAT32_MEDIA *media_ptr, unsigned int cluster, unsigned int *FAT_entry_ptr);

unsigned int FAT32_utility_FAT_write(FAT32_MEDIA *media_ptr, unsigned int cluster, unsigned int next_cluster);

unsigned int FAT32_utility_FAT_find_free(FAT32_MEDIA *media_ptr, unsigned int *cluster_ptr);

unsigned int FAT32_utility_string_compare(char *str1, char *str2, unsigned int shorted_name_flag);

unsigned int FAT32_utility_string_length(char *string);
unsigned int FAT32_utility_cluster_to_sector(FAT32_MEDIA *media_ptr, unsigned int cluster_number);

void FAT32_driver(struct FAT32_MEDIA *media_ptr);

#endif //FAT32_FS_FAT32_STRUCT_H
