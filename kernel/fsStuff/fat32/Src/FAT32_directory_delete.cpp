//
// Created by user on 2021/10/12.
//


#include "../Inc/FAT32_struct.h"

unsigned int FAT32_directory_delete(FAT32_MEDIA *media_ptr, char *name_ptr)
{
    FAT32_DIR temp_dir;
    FAT32_DIR *dir_ptr = NULL;
    unsigned int cluster = 0;
    unsigned int next_cluster;
    if(FAT32_directory_search(media_ptr,name_ptr,&temp_dir) != FAT_SUCCESS)
        return FAT_NOT_FOUND;
    FAT32_utility_logic_sector_read(media_ptr,media_ptr->memory_buff,media_ptr->last_found.path_sector,1);
    unsigned int byte_offset = (media_ptr->last_found.path_entry * sizeof(FAT32_DIR)) % media_ptr->boot.bytes_per_sector;
    dir_ptr = media_ptr->memory_buff + byte_offset;
    cluster = dir_ptr->first_cluster_l + (dir_ptr->first_cluster_h << 16);
    dir_ptr->DIR_name[0] = DIR_ENTRY_FREE;
    dir_ptr--;
    while((dir_ptr->DIR_name[0] & 0x40) == 0 && dir_ptr->DIR_Attr == ATTR_LONG_NAME)
    {
        dir_ptr->DIR_name[0] = DIR_ENTRY_FREE;
        dir_ptr--;
    }
    dir_ptr->DIR_name[0] = DIR_ENTRY_FREE;
    FAT32_utility_logic_sector_read(media_ptr,media_ptr->memory_buff,media_ptr->last_found.path_sector,1);
    while(cluster != FAT32_END) {
        FAT32_utility_FAT_read(media_ptr,cluster,&next_cluster);
        FAT32_utility_FAT_write(media_ptr,cluster,0);
        cluster = next_cluster;
    }
    return FAT_SUCCESS;
}
