//
// Created by user on 2021/9/25.
//
#include "../Inc/FAT32_struct.h"

/*
 * 刷新media_ptr缓存中sector_number为[logic_sector,logic_sector + sectors ) 的FAT32_CACHE_SECTOR
 * 根据invalid确定是否将其无效化
 */

unsigned int FAT32_utility_logic_sector_flush(FAT32_MEDIA *media_ptr, unsigned int logic_sector, unsigned int sectors,
                                              unsigned int invalid) {
    unsigned int end_sector = logic_sector + sectors - 1;
    struct FAT32_CACHE_SECTOR *current_sector = media_ptr->cache_sector_list;
    for (unsigned int cache_size = media_ptr->cache_sector_size; cache_size && current_sector; cache_size--) {
        if (media_ptr->dirty_sectors == 0 && invalid == 0)
            break;
        if (current_sector->sector_number >= logic_sector && current_sector->sector_number <= end_sector) {

            if (current_sector->valid && current_sector->dirty) {
                media_ptr->operate_sectors_start = current_sector->sector_number;
                media_ptr->operate_buff = current_sector->cache_memory_ptr;
                media_ptr->operate_sectors = 1;
                media_ptr->request = WRITE;
                media_ptr->status = FAT_IO_ERROR;
                (media_ptr->driver)(media_ptr);
                if (media_ptr->status != FAT_SUCCESS) {
                    media_ptr->request = UNINIT;
                    media_ptr->status = FAT_IO_ERROR;
                    (media_ptr->driver)(media_ptr);
                    return FAT_IO_ERROR;
                }
                current_sector->dirty = 0;
                media_ptr->dirty_sectors--;
            }

            if(invalid)
                current_sector->valid = 0;
        }
        current_sector = current_sector->next_used_cache_sector;
    }
    return FAT_SUCCESS;
}
