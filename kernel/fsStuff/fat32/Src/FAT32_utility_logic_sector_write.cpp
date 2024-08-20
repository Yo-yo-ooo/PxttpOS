//
// Created by user on 2021/9/27.
//
#include "../Inc/FAT32_struct.h"


#undef ThisSata
#define ThisSata    ((SataDiskInterface*)osData.diskInterfaces[media_ptr->disk->ID])


unsigned int FAT32_utility_logic_sector_write(FAT32_MEDIA *media_ptr,void *buffer,unsigned int logic_sector,unsigned int write_sectors)
{

    struct FAT32_CACHE_SECTOR *current_sector = media_ptr->cache_sector_list;
    if(buffer >= media_ptr->memory_buff && buffer <= media_ptr->memory_cache_sector_end)
    {
        for(unsigned int cache_size = media_ptr->cache_sector_size;cache_size;cache_size--)
        {
            if(current_sector->sector_number == logic_sector && current_sector->valid)
            {
                current_sector->dirty = 1;
                return FAT_SUCCESS;
            }
            if(current_sector->next_used_cache_sector)
             current_sector = current_sector->next_used_cache_sector;
        }

    }
    FAT32_utility_logic_sector_flush(media_ptr,logic_sector,write_sectors,1);
    media_ptr->operate_buff = buffer;
    media_ptr->operate_sectors_start = logic_sector;
    media_ptr->operate_sectors = write_sectors;
    media_ptr->request = WRITE;
    (media_ptr->driver)(media_ptr);
    return media_ptr->status;
}
