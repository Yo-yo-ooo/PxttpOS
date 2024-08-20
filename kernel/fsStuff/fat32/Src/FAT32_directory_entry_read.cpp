//
// Created by user on 2021/10/7.
//

#include "../Inc/FAT32_struct.h"

void FAT32_read_long_name_dir(struct FAT32_DIR_long_name *source_dir, unsigned char *dest_ptr) {
    unsigned int local = (source_dir->order & 0x1F) - 1;
    unsigned int times = 0;
    dest_ptr += local * 13;
    for (times = 0; times < 5; times++)
        dest_ptr[times] = source_dir->name1[times * 2];
    for (times = 0; times < 6; times++)
        dest_ptr[times + 5] = source_dir->name2[times * 2];
    dest_ptr[11] = source_dir->name3[0];
    dest_ptr[12] = source_dir->name3[2];
    if (source_dir->order & FAT32_LONG_NAME_END) {
        dest_ptr[13] = 0;

    }
}


//entry:指明读取源文件夹中的第几个文件信息
unsigned int FAT32_directory_entry_read(FAT32_MEDIA *media_ptr, FAT32_DIR *source_dir, unsigned long *entry_ptr,
                                        FAT32_DIR *dest_dir) {
    struct FAT32_DIR *read_ptr = NULL;
    struct FAT32_DIR_long_name *long_name_read_ptr = NULL;
    unsigned int long_name_dir_num = 0;
    unsigned int byte_offset = *entry_ptr * sizeof(struct FAT32_DIR);
    unsigned int bytes_per_cluster = (media_ptr->boot.bytes_per_sector * media_ptr->boot.sectors_per_cluster);
    if (bytes_per_cluster == 0)
        return FAT_INVALID_MEDIA;
    unsigned int relative_cluster = byte_offset / bytes_per_cluster;
    unsigned int relative_sector = 0;
    unsigned int cluster = 0;
    unsigned int next_cluster = 0;
    unsigned int logic_sector = 0;
    byte_offset %= bytes_per_cluster;
    relative_sector = byte_offset / media_ptr->boot.bytes_per_sector;
    byte_offset %= media_ptr->boot.bytes_per_sector;

    if (source_dir && source_dir->DIR_Attr && source_dir->DIR_name[0])
        cluster = (unsigned int) (source_dir->first_cluster_h << 16) + (unsigned int) source_dir->first_cluster_l;
    else
        cluster = media_ptr->boot.root_cluster;

    for (unsigned int times = 0; times < relative_cluster; times++) {
        FAT32_utility_FAT_read(media_ptr, cluster, &next_cluster);
        cluster = next_cluster;
    }

    logic_sector = FAT32_utility_cluster_to_sector(media_ptr, cluster) + relative_sector;
    FAT32_utility_logic_sector_read(media_ptr, media_ptr->memory_buff, logic_sector, 1);

    media_ptr->last_found.path_sector = logic_sector;
    read_ptr = media_ptr->memory_buff + byte_offset;
    while (read_ptr->DIR_name[0] == DIR_ENTRY_FREE) {
        byte_offset += sizeof(struct FAT32_DIR);
        if (byte_offset >= media_ptr->boot.bytes_per_sector) {
            relative_sector++;
            if (relative_sector >= media_ptr->boot.sectors_per_cluster) {
                FAT32_utility_FAT_read(media_ptr, cluster, &next_cluster);
                cluster = next_cluster;
                relative_sector = 0;
            }
            logic_sector = FAT32_utility_cluster_to_sector(media_ptr, cluster) + relative_sector;
            FAT32_utility_logic_sector_read(media_ptr, media_ptr->memory_buff, logic_sector, 1);
            byte_offset = 0;
        }
        read_ptr = media_ptr->memory_buff + byte_offset;
        (*entry_ptr)++;
    }
    if (read_ptr->DIR_Attr == 0 && read_ptr->DIR_name[0] == 0)
        return FAT_NO_MORE_CONTENT;
    if (read_ptr->DIR_Attr == ATTR_LONG_NAME) {
        if ((read_ptr->DIR_name[0] & FAT32_LONG_NAME_END) == 0)
            return FAT_INVALID_PARA;
        long_name_dir_num = read_ptr->DIR_name[0] & 0x1F;
        for (unsigned int times = 0;
             times < long_name_dir_num; times++, byte_offset += sizeof(struct FAT32_DIR), (*entry_ptr)++) {
            if (byte_offset >= media_ptr->boot.bytes_per_sector) {
                relative_sector++;
                if (relative_sector >= media_ptr->boot.sectors_per_cluster) {
                    FAT32_utility_FAT_read(media_ptr, cluster, &next_cluster);
                    cluster = next_cluster;
                    relative_sector = 0;
                }
                logic_sector = FAT32_utility_cluster_to_sector(media_ptr, cluster) + relative_sector;
                FAT32_utility_logic_sector_read(media_ptr, media_ptr->memory_buff, logic_sector, 1);
                byte_offset = 0;
            }
            long_name_read_ptr = media_ptr->memory_buff + byte_offset;
            FAT32_read_long_name_dir(long_name_read_ptr, media_ptr->name_buff);
        }
        if (byte_offset >= media_ptr->boot.bytes_per_sector) {
            relative_sector++;
            if (relative_sector >= media_ptr->boot.sectors_per_cluster) {
                FAT32_utility_FAT_read(media_ptr, cluster, &next_cluster);
                cluster = next_cluster;
                relative_sector = 0;
            }
            logic_sector = FAT32_utility_cluster_to_sector(media_ptr, cluster) + relative_sector;
            FAT32_utility_logic_sector_read(media_ptr, media_ptr->memory_buff, logic_sector, 1);
            byte_offset = 0;
        }
        read_ptr = media_ptr->memory_buff + byte_offset;
    } else {
        unsigned int index = 0;
        unsigned int move = 0;
        for (unsigned int times = 0; times < 11; times++)
            media_ptr->name_buff[times] = read_ptr->DIR_name[times];
        media_ptr->name_buff[11] = 0;
        if (media_ptr->name_buff[10] != ' ') {
            for (unsigned int times = 10; times; times--) {
                if (media_ptr->name_buff[times] == ' ') {
                    move = times + 1;
                    for (; times; times--) {
                        if (media_ptr->name_buff[times - 1] != ' ') {
                            index = times;
                            media_ptr->name_buff[index] = '.';
                            index++;
                            for (; move < 11; move++, index++) {
                                media_ptr->name_buff[index] = media_ptr->name_buff[move];
                            }
                            media_ptr->name_buff[index] = 0;
                            times = 1;
                            break;
                        }
                    }
                }
            }
        } else {
            for (unsigned int times = 10; times; times--) {
                if (media_ptr->name_buff[times] != ' ')
                    break;
                else
                    media_ptr->name_buff[times] = 0;
            }
        }
    }
    *dest_dir = *read_ptr;

    dest_dir->LastAccessD.year = RTC::Year;
    dest_dir->LastAccessD.month = RTC::Month;
    dest_dir->LastAccessD.day = RTC::Day;

    (*entry_ptr)++;
    return FAT_SUCCESS;
}