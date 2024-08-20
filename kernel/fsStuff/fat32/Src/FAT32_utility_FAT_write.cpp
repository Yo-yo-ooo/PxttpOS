//
// Created by user on 2021/10/1.
//

#include "../Inc/FAT32_struct.h"


unsigned int FAT32_utility_FAT_write(FAT32_MEDIA *media_ptr,unsigned int cluster,unsigned int next_cluster)
{
    unsigned int index = (cluster & FAT_CACHE_FAT_HASH_MASK) * FAT_CACHE_FAT_DEPTH;
    struct FAT32_CACHE_FAT *cache_FAT_entry = &(media_ptr->cache_FATs[index]);
    struct FAT32_CACHE_FAT temp_cache_FAT;
    unsigned int status = 0;
    for(unsigned int times = 0;times < FAT_CACHE_FAT_DEPTH;times++)
    {
        if(cache_FAT_entry[times].cached_FAT_cluster == cluster)
        {
            cache_FAT_entry[times].cached_FAT_value = next_cluster;
            cache_FAT_entry[times].cached_FAT_dirty = 1;
            return FAT_SUCCESS;
        }
    }

    if(cache_FAT_entry[FAT_CACHE_FAT_DEPTH - 1].cached_FAT_dirty) {
        status = FAT32_utility_FAT_flush(media_ptr);
        if(status != FAT_SUCCESS)
            return status;
    }
    temp_cache_FAT.cached_FAT_dirty = 0;
    temp_cache_FAT.cached_FAT_cluster = cluster;
    temp_cache_FAT.cached_FAT_value = next_cluster;
    for(unsigned int times = FAT_CACHE_FAT_DEPTH - 1;times;times--)
        cache_FAT_entry[times] = cache_FAT_entry[times - 1];
    cache_FAT_entry[0] = temp_cache_FAT;

    return FAT_SUCCESS;
}
