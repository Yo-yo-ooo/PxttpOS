//
// Created by user on 2021/9/25.
//
#include "../Inc/FAT32_struct.h"

/*       如果在cache中找到相应逻辑扇区
 *   将相应FAT32_CACHE_SECTOR更新到链表最前端
 *   将media_ptr->memory_buff设置指向FAT32_CACHE_SECTOR::cache_memory_ptr
 *   返回NULL
 *
 *      如果cache中没找到相应逻辑扇区
 *   返回将被更新的FAT32_CACHE_SECTOR的指针
 *   并将其前一个节点的next_used_cache_sector更新为NULL
 * */
struct FAT32_CACHE_SECTOR *
FAT32_utility_logic_sector_cache_entry_read(struct FAT32_MEDIA *media_ptr, unsigned int logic_sector,struct FAT32_CACHE_SECTOR **previous_cache_sector) {
    if (media_ptr->cache_sector_list == NULL)
        return media_ptr->cache_sectors;

    *previous_cache_sector = NULL;
    struct FAT32_CACHE_SECTOR *current_cache_sector = media_ptr->cache_sector_list;

    for (unsigned int cache_size = media_ptr->cache_sector_size; cache_size; cache_size--) {
        if (current_cache_sector->valid && current_cache_sector->sector_number == logic_sector) {
            media_ptr->cache_hit++;
            media_ptr->memory_buff = current_cache_sector->cache_memory_ptr;
            if (*previous_cache_sector != NULL) {
                (*previous_cache_sector)->next_used_cache_sector = current_cache_sector->next_used_cache_sector;
                current_cache_sector->next_used_cache_sector = media_ptr->cache_sector_list;
                media_ptr->cache_sector_list = current_cache_sector;
            }
            return NULL;
        }
        if (current_cache_sector->next_used_cache_sector) {
            (*previous_cache_sector) = current_cache_sector;
            current_cache_sector = current_cache_sector->next_used_cache_sector;
        }
    }
    return  current_cache_sector;
}


