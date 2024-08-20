//
// Created by user on 2021/10/12.
//

#include "../Inc/FAT32_struct.h"

unsigned int FAT32_directory_free_find(FAT32_MEDIA *media_ptr, FAT32_DIR *source_dir,unsigned int free_dir_size, unsigned int *dir_offset_ptr)
{
    if(source_dir->DIR_Attr != ATTR_DIRECTORY)
        return FAT_INVALID_PARA;
    if(free_dir_size == 0)
        return  FAT_SUCCESS;
    unsigned int dir_offset = 0;
    unsigned int current_offset = 0;
    unsigned int free_dir = 0;
    unsigned int cluster = source_dir->first_cluster_l + (source_dir->first_cluster_h << 16);
    unsigned int sector = 0;
    unsigned int next_cluster = 0;

    do{
        for(sector = 0;sector < media_ptr->boot.sectors_per_cluster;sector++)
        {
            FAT32_utility_logic_sector_read(media_ptr,media_ptr->memory_buff,sector + FAT32_utility_cluster_to_sector(media_ptr,cluster),1);
            source_dir = media_ptr->memory_buff;

            for(unsigned int index = 0;index < media_ptr->boot.bytes_per_sector / sizeof(struct FAT32_DIR);index++)
            {
                if(source_dir[index].DIR_name[0] == 0 && source_dir[index].DIR_Attr == 0)
                {
                    *dir_offset_ptr = dir_offset;
                    return  FAT_SUCCESS;
                }
                if(source_dir[index].DIR_name[0] == DIR_ENTRY_FREE)
                {

                    free_dir++;
                    if(free_dir_size == free_dir)
                    {
                        *dir_offset_ptr = dir_offset;
                        return FAT_SUCCESS;
                    }
                    continue;
                }
                current_offset++;
                dir_offset = current_offset;
            }
        }
        FAT32_utility_FAT_read(media_ptr,cluster,&next_cluster);
    }while(next_cluster != FAT32_END);
    *dir_offset_ptr = current_offset;


    return FAT_SUCCESS;
}
