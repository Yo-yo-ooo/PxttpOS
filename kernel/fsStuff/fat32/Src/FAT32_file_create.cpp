//
// Created by user on 2021/10/13.
//

#include "../Inc/FAT32_struct.h"
unsigned int FAT32_file_create(FAT32_MEDIA *media_ptr,char *name_ptr)
{
    struct FAT32_DIR temp_dir;
    unsigned int status = 0;
    char *origin_ptr = name_ptr;
    if(FAT32_directory_search(media_ptr,name_ptr,&temp_dir) == FAT_SUCCESS)
        return FAT_INVALID_PARA;
    char *previous_name_ptr = name_ptr;

    do{
        name_ptr = previous_name_ptr;
        previous_name_ptr = FAT32_directory_name_extract(name_ptr,media_ptr->name_buff);
    }while(previous_name_ptr);
    unsigned int cluster =0;
    FAT32_utility_alloc_cluster(media_ptr,0,&cluster);
    return FAT32_directory_write(media_ptr,&media_ptr->last_found.path_dir,name_ptr,ATTR_ARCHIVE,cluster,0);
}