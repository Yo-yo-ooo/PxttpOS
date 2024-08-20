//
// Created by user on 2021/9/24.
//

#include "../Inc/FAT32_struct.h"


unsigned int FAT32_utility_boot_info_extract(FAT32_MEDIA *media_ptr, void *source) {
    if (media_ptr && source) {
        struct FAT32_BOOT *boot_ptr = source;
        if (boot_ptr->bytes_per_sector == 0)
            return FAT_BOOT_ERROR;
        if (boot_ptr->sectors_per_FAT == 0)
            return FAT_BOOT_ERROR;
        if (boot_ptr->sectors_per_cluster == 0)
            return FAT_BOOT_ERROR;
        if (boot_ptr->root_cluster == 0)
            return FAT_BOOT_ERROR;
        if (boot_ptr->total_sectors == 0)
            return FAT_BOOT_ERROR;
        if (boot_ptr->reserved_sectors == 0)
            return FAT_BOOT_ERROR;


        media_ptr->boot = *boot_ptr;
        return FAT_SUCCESS;
    }
    return FAT_INVALID_PARA;
}