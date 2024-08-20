//
// Created by user on 2021/9/25.
//

#include "../Inc/FAT32_struct.h"

/*
 * 若buffer属于media_ptr->memory_buff缓存，则默认仅读取一个扇区
 * 同时将响应结果设置到media_ptr->memory_buff
 *
 * 若buff不位于media_ptr缓存，
 * 则从前后端各获取一次缓存扇区
 * 剩余未获取扇区则通过直接读取的方式获取
 * 若未获取扇区大小小于缓存的1/4
 * 则将其送入缓存
 */

unsigned int FAT32_utility_logic_sector_read(FAT32_MEDIA *media_ptr, void *buffer, unsigned int logic_sector,
                                             unsigned int read_sectors) {
    struct FAT32_CACHE_SECTOR *previous_cache_sector = NULL;
    struct FAT32_CACHE_SECTOR *cache_sector = NULL;
    if (buffer >= media_ptr->memory_buff &&
        buffer <= media_ptr->memory_cache_sector_end)//位于缓存内
    {
        cache_sector = FAT32_utility_logic_sector_cache_entry_read(media_ptr, logic_sector, &previous_cache_sector);
        if (cache_sector == NULL)
            return FAT_SUCCESS;
        if (cache_sector->valid && cache_sector->dirty) {
            media_ptr->operate_sectors_start = cache_sector->sector_number;
            media_ptr->operate_buff = cache_sector->cache_memory_ptr;
            media_ptr->operate_sectors = 1;
            media_ptr->request = WRITE;
            media_ptr->status = FAT_IO_ERROR;
            (media_ptr->driver)(media_ptr);
            if (media_ptr->status != FAT_SUCCESS)
                return FAT_IO_ERROR;
            cache_sector->dirty = 0;
            media_ptr->dirty_sectors--;
        }


        cache_sector->sector_number = logic_sector;
        cache_sector->valid = 1;
        cache_sector->dirty = 0;
        media_ptr->operate_sectors_start = logic_sector;
        media_ptr->operate_buff = cache_sector->cache_memory_ptr;
        media_ptr->operate_sectors = 1;
        media_ptr->request = READ;
        media_ptr->status = FAT_IO_ERROR;
        (media_ptr->driver)(media_ptr);

        if (previous_cache_sector && media_ptr->status == FAT_SUCCESS) {
            previous_cache_sector->next_used_cache_sector = cache_sector->next_used_cache_sector;
            cache_sector->next_used_cache_sector = media_ptr->cache_sector_list;
            media_ptr->cache_sector_list = cache_sector;
        } else {
            cache_sector->valid = 0;
        }
        media_ptr->memory_buff = cache_sector->cache_memory_ptr;
        return media_ptr->status;
    }
    else {
        if (logic_sector + read_sectors > media_ptr->boot.total_sectors)
            return FAT_INVALID_SECTOR;
        unsigned int end_sector = logic_sector + read_sectors - 1;
        while (read_sectors) {
            if (FAT32_utility_logic_sector_cache_entry_read(media_ptr, logic_sector, &previous_cache_sector) != NULL)
                break;
            FAT32_utility_memory_copy(media_ptr->memory_buff, buffer, media_ptr->boot.bytes_per_sector);
            logic_sector++;
            read_sectors--;
            buffer += media_ptr->boot.bytes_per_sector;
        }

        while (read_sectors) {
            if (FAT32_utility_logic_sector_cache_entry_read(media_ptr, end_sector, &previous_cache_sector) != NULL)
                break;
            FAT32_utility_memory_copy(media_ptr->memory_buff, buffer +
                                                              media_ptr->boot.bytes_per_sector * (read_sectors - 1),
                                      media_ptr->boot.bytes_per_sector);
            end_sector--;
            read_sectors--;
        }

        if (read_sectors == 0)
            return FAT_SUCCESS;
        FAT32_utility_logic_sector_flush(media_ptr, logic_sector, read_sectors, 1);


        media_ptr->operate_buff = buffer;
        media_ptr->operate_sectors_start = logic_sector;
        media_ptr->operate_sectors = read_sectors;
        media_ptr->request = READ;
        (media_ptr->driver)(media_ptr);

        if(media_ptr->status == FAT_SUCCESS && read_sectors < (media_ptr->cache_sector_size) / 4)
        {
            while(read_sectors) {
                cache_sector = FAT32_utility_logic_sector_cache_entry_read(media_ptr,logic_sector,&previous_cache_sector);
                if(cache_sector->valid && cache_sector->dirty)
                {
                    media_ptr->operate_sectors_start = cache_sector->sector_number;
                    media_ptr->operate_buff = cache_sector->cache_memory_ptr;
                    media_ptr->operate_sectors = 1;
                    media_ptr->request = WRITE;
                    (media_ptr->driver)(media_ptr);
                    if(media_ptr->status == FAT_SUCCESS)
                    {
                        cache_sector->dirty = 0;
                        media_ptr->dirty_sectors--;
                    }
                    else
                        return  media_ptr->status;
                }
                cache_sector->dirty = 0;
                cache_sector->valid = 1;
                cache_sector->sector_number = logic_sector;
                if(previous_cache_sector) {
                    previous_cache_sector->next_used_cache_sector = cache_sector->next_used_cache_sector;
                    cache_sector->next_used_cache_sector = media_ptr->cache_sector_list;
                    media_ptr->cache_sector_list = cache_sector;
                }
                FAT32_utility_memory_copy(buffer,cache_sector->cache_memory_ptr,media_ptr->boot.bytes_per_sector);
                buffer += media_ptr->boot.bytes_per_sector;
                read_sectors--;
                logic_sector++;
            }
        }
    }
    return media_ptr->status;
}