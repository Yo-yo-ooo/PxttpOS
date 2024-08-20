//
// Created by user on 2021/9/25.
//

#include "../Inc/FAT32_struct.h"
unsigned int FAT32_media_read(FAT32_MEDIA *media_ptr,void*buffer,unsigned int logic_sector)
{
    return FAT32_utility_logic_sector_read(media_ptr,buffer,logic_sector,1);
}
