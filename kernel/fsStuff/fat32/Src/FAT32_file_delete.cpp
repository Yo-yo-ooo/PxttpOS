//
// Created by user on 2021/10/13.
//

#include "../Inc/FAT32_struct.h"

unsigned int FAT32_file_delete(FAT32_MEDIA *media_ptr, char*name_ptr)
{

    return FAT32_directory_delete(media_ptr,name_ptr);
}
