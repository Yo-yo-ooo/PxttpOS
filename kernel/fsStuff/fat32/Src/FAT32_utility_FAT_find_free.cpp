//
// Created by user on 2021/10/12.
//

#include "../Inc/FAT32_struct.h"

unsigned int FAT32_utility_FAT_find_free(FAT32_MEDIA *media_ptr, unsigned int *cluster_ptr)
{
    if(media_ptr->boot.bytes_per_sector == 0)
        return FAT_INVALID_MEDIA;
    int FAT_sector = 0;
    unsigned int* FAT_ptr = NULL;
    for(FAT_sector = 0;FAT_sector < media_ptr->boot.sectors_per_FAT;FAT_sector++)
    {
        FAT32_utility_logic_sector_read(media_ptr,media_ptr->memory_buff,FAT_sector + media_ptr->boot.reserved_sectors,1);
        FAT_ptr = media_ptr->memory_buff;
        for(unsigned int times = 0;times < media_ptr->boot.bytes_per_sector >> 2;times++)
        {
            if(FAT_ptr[times] == FAT32_FREE)
            {
                *cluster_ptr = FAT_sector * (media_ptr->boot.bytes_per_sector >> 2) + times;
                return  FAT_SUCCESS;
            }
        }
    }
    return FAT_NOT_FOUND;
}
