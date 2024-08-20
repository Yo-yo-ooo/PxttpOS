//
// Created by user on 2021/10/7.
//


#include "../Inc/FAT32_struct.h"

unsigned int FAT32_directory_create(FAT32_MEDIA *media_ptr,char *name_ptr)
{
    struct FAT32_DIR temp_dir;
    FAT32_DIR *dir_ptr = NULL;
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
    FAT32_directory_write(media_ptr,&media_ptr->last_found.path_dir,name_ptr,ATTR_DIRECTORY,cluster,0);
    FAT32_utility_logic_sector_read(media_ptr,media_ptr->memory_buff, FAT32_utility_cluster_to_sector(media_ptr,cluster),1);
    dir_ptr = media_ptr->memory_buff;
    dir_ptr->DIR_name[0]=".";
    for(unsigned int index=1;index < 11;index++)
        dir_ptr->DIR_name[index] = ' ';
    dir_ptr->file_size = 0;
    dir_ptr->DIR_Attr = ATTR_DIRECTORY;
    dir_ptr->first_cluster_l = cluster;
    dir_ptr->first_cluster_h = cluster >> 16;

    dir_ptr++;
    dir_ptr->DIR_name[0]=".";
    dir_ptr->DIR_name[1]='.';
    for(unsigned int index=2;index < 11;index++)
        dir_ptr->DIR_name[index] = ' ';
    dir_ptr->file_size = 0;
    dir_ptr->DIR_Attr = ATTR_DIRECTORY;
    //需实现上级目录的查找
    dir_ptr->first_cluster_l = cluster;
    dir_ptr->first_cluster_h = cluster >> 16;

    
    dir_ptr->CreationD.year = RTC::Year;
    dir_ptr->CreationD.month = RTC::Month;
    dir_ptr->CreationD.day = RTC::Day;

    dir_ptr->CreationT.hour = RTC::Hour;
    dir_ptr->CreationT.min = RTC::Minute;
    dir_ptr->CreationT.sec = RTC::Second;

    dir_ptr->LastAccessD.year = RTC::Year;
    dir_ptr->LastAccessD.month = RTC::Month;
    dir_ptr->LastAccessD.day = RTC::Day;

    return FAT32_utility_logic_sector_write(media_ptr,media_ptr->memory_buff, FAT32_utility_cluster_to_sector(media_ptr,cluster),1);

}
