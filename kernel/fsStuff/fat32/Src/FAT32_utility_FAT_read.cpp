//
// Created by user on 2021/9/28.
//

#include "../Inc/FAT32_struct.h"

unsigned int FAT32_utility_FAT_read(FAT32_MEDIA *media_ptr,unsigned int cluster,unsigned int *FAT_entry_ptr)
{
    struct FAT32_CACHE_FAT temp_cache_FAT;
    struct FAT32_CACHE_FAT *cache_FAT_entry;
    unsigned int index = (cluster & FAT_CACHE_FAT_HASH_MASK) * FAT_CACHE_FAT_DEPTH;
    unsigned int status = 0;

    cache_FAT_entry = &(media_ptr->cache_FATs[index]);


    for(unsigned int times = 0;times < FAT_CACHE_FAT_DEPTH;times++)
    {
        if(cache_FAT_entry[times].cached_FAT_cluster == cluster)
        {
            *FAT_entry_ptr = cache_FAT_entry[times].cached_FAT_value;
            //将使用的cache_FAT移到最前端，当前cache_FAT后的向前移动
            temp_cache_FAT = cache_FAT_entry[times];
            for(unsigned int local = times;local;local--)
                cache_FAT_entry[local] = cache_FAT_entry[local - 1];
            cache_FAT_entry[0] = temp_cache_FAT;
            return  FAT_SUCCESS;
        }
    }

    if(cache_FAT_entry[FAT_CACHE_FAT_DEPTH - 1].cached_FAT_dirty)
    {
        status = FAT32_utility_FAT_flush(media_ptr);
        if(status != FAT_SUCCESS)
            return status;
    }
    //从第一个FAT中获取相应的值
    unsigned int byte_offset = cluster * 4;
    unsigned int sector = media_ptr->boot.reserved_sectors + (byte_offset / media_ptr->boot.bytes_per_sector);
    void *FAT_ptr;

    byte_offset = byte_offset & media_ptr->boot.bytes_per_sector;
    status = FAT32_utility_logic_sector_read(media_ptr,media_ptr->memory_buff,sector,1);
    if(status != FAT_SUCCESS)
        return status;
    FAT_ptr = media_ptr->memory_buff + byte_offset;

    temp_cache_FAT.cached_FAT_dirty = 0;
    temp_cache_FAT.cached_FAT_value = FAT32_utility_32_unsigned_read(FAT_ptr);
    temp_cache_FAT.cached_FAT_cluster = cluster;
    *FAT_entry_ptr = temp_cache_FAT.cached_FAT_value;
    for(unsigned int index = FAT_CACHE_FAT_DEPTH - 1;index;index--)
        cache_FAT_entry[index] = cache_FAT_entry[index - 1];
    cache_FAT_entry[0] = temp_cache_FAT;
    return FAT_SUCCESS;
}
