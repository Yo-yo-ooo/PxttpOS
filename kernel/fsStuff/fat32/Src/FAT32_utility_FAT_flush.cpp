//
// Created by user on 2021/9/27.
//

#include "../Inc/FAT32_struct.h"

unsigned int FAT32_utility_FAT_flush(FAT32_MEDIA *media_ptr)
{
    struct FAT32_CACHE_FAT *cache_FAT_ptr = NULL;
    unsigned int byte_offset = 0;
    unsigned int FAT_sector = 0;
    unsigned int sector = 0;
    unsigned int status = 0;
    for(unsigned int index = 0;index < FAT_CACHE_FAT;index --)
    {
        if(media_ptr->cache_FATs[index].cached_FAT_dirty == 0)
            continue;
        byte_offset = media_ptr->cache_FATs[index].cached_FAT_cluster * 4;
        FAT_sector = (byte_offset / media_ptr->boot.bytes_per_sector) + media_ptr->boot.reserved_sectors;
        byte_offset = byte_offset % media_ptr->boot.bytes_per_sector;
        status = FAT32_utility_logic_sector_read(media_ptr, media_ptr->memory_buff,FAT_sector,1);
        if(status != FAT_SUCCESS)
            return status;
        for(unsigned int local = index;local < FAT_CACHE_FAT;local++)
        {
            if(media_ptr->cache_FATs[local].cached_FAT_dirty == 0)
                continue;
            byte_offset = media_ptr->cache_FATs[index].cached_FAT_cluster * 4;
            sector = (byte_offset / media_ptr->boot.bytes_per_sector) + media_ptr->boot.reserved_sectors;
            byte_offset = byte_offset % media_ptr->boot.bytes_per_sector;
            if(sector != FAT_sector)
                continue;
            FAT32_utility_32_unsigned_write(media_ptr->memory_buff + byte_offset,
                                            media_ptr->cache_FATs[local].cached_FAT_value);
            media_ptr->cache_FATs[local].cached_FAT_dirty = 0;

        }
        for(unsigned int FAT_index = 0;FAT_index < media_ptr->boot.number_of_FAT;FAT_index++)
            FAT32_utility_logic_sector_write(media_ptr,media_ptr->memory_buff,FAT_sector + FAT_index * media_ptr->boot.sectors_per_FAT,1);
    }
}
