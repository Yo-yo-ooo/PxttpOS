//
// Created by user on 2021/9/24.
//

#include "../Inc/FAT32_struct.h"

unsigned int FAT32_media_open(FAT32_MEDIA*media_ptr,MEDIA_DISK* disk_ptr,
                              void (*driver)(FAT32_MEDIA*),void *memory_buffer,
                              unsigned int memory_size)
{
    if(media_ptr && disk_ptr && driver && memory_buffer)
    {
        if(memory_size < SECTOR_SIZE)
            return FAT_INVALID_PARA;

        unsigned int i = 0;


        media_ptr->disk = disk_ptr;
        media_ptr->memory_buff = memory_buffer;
        media_ptr->driver = driver;
        media_ptr->status = 0;
        media_ptr->request = 0;
        media_ptr->cache_hit = 0;
        media_ptr->dirty_sectors = 0;

        media_ptr->operate_buff = memory_buffer;
        media_ptr->request = INIT;
        media_ptr->status = FAT_IO_ERROR;
        (media_ptr->driver)(media_ptr);
        if(media_ptr->status != FAT_SUCCESS)
        {
            media_ptr->request = UNINIT;
            media_ptr->status = FAT_IO_ERROR;
            (media_ptr->driver)(media_ptr);
            return  FAT_BOOT_ERROR;
        }


        media_ptr->operate_buff = memory_buffer;
        media_ptr->request = READ_BOOT;
        media_ptr->status = FAT_IO_ERROR;
        (media_ptr->driver)(media_ptr);
        if(media_ptr->status != FAT_SUCCESS)
        {
            media_ptr->request = UNINIT;
            media_ptr->status = FAT_IO_ERROR;
            (media_ptr->driver)(media_ptr);
            return  FAT_BOOT_ERROR;
        }

        if(FAT32_utility_boot_info_extract(media_ptr,memory_buffer) != FAT_SUCCESS)
        {
            media_ptr->request = UNINIT;
            media_ptr->status = FAT_IO_ERROR;
            (media_ptr->driver)(media_ptr);
            return  FAT_BOOT_ERROR;
        }

        media_ptr->cache_sector_size = memory_size / media_ptr->boot.bytes_per_sector;
        if(media_ptr->cache_sector_size > MAX_CACHE_SECTOR)
            media_ptr->cache_sector_size = MAX_CACHE_SECTOR;
        media_ptr->memory_cache_sector_end = media_ptr->memory_buff + media_ptr->cache_sector_size * media_ptr->boot.bytes_per_sector - 1;
        if(media_ptr->cache_sector_size == 0)
        {
            media_ptr->request = UNINIT;
            media_ptr->status = FAT_IO_ERROR;
            (media_ptr->driver)(media_ptr);
            return  FAT_BUFFER_ERROR;
        }

        media_ptr->cache_sector_list = media_ptr->cache_sectors;
        for(i = 0;i < media_ptr->cache_sector_size;i++)
        {
            media_ptr->cache_sectors[i].dirty = 0;
            media_ptr->cache_sectors[i].sector_number = 0;
            media_ptr->cache_sectors[i].valid = 0;
            media_ptr->cache_sectors[i].cache_memory_ptr = memory_buffer + i * media_ptr->boot.bytes_per_sector;
            media_ptr->cache_sectors[i].next_used_cache_sector = &(media_ptr->cache_sectors[i+1]);
        }
        media_ptr->cache_sectors[i - 1].next_used_cache_sector = NULL;


        for(i = 0;i < FAT_CACHE_FAT;i ++)
        {
            media_ptr->cache_FATs[i].cached_FAT_cluster = 0;
            media_ptr->cache_FATs[i].cached_FAT_dirty = 0;
            media_ptr->cache_FATs[i].cached_FAT_value = 0;
        }

        FAT32_utility_memory_set(&(media_ptr->last_found),0, sizeof(struct FAT32_PATH));
        FAT32_utility_memory_set(&(media_ptr->path),0, sizeof(struct FAT32_PATH));
        return FAT_SUCCESS;
    }
    return  FAT_INVALID_PARA;
}