//
// Created by user on 2021/9/24.
//

#include "../Inc/FAT32_struct.h"

unsigned int FAT32_media_flush(FAT32_MEDIA *media_ptr)
{
    if(media_ptr)
    {
        FAT32_utility_logic_sector_flush(media_ptr,1,FAT32_END,1);
        FAT32_utility_FAT_flush(media_ptr);

        return FAT_SUCCESS;
    }
    return  FAT_INVALID_MEDIA;
}
unsigned int FAT32_media_close(FAT32_MEDIA*media_ptr)
{
    return FAT32_media_flush(media_ptr);
    return FAT_INVALID_PARA;
}
