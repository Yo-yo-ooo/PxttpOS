//
// Created by user on 2021/9/27.
//


#include "../Inc/FAT32_struct.h"




unsigned int FAT32_directory_search(FAT32_MEDIA *media_ptr,char *name_ptr,FAT32_DIR *entry_dir)
{
    if(media_ptr->boot.bytes_per_sector == 0 || media_ptr->boot.sectors_per_cluster == 0)
        return FAT_INVALID_MEDIA;
    unsigned int cluster = 0;
    unsigned int next_cluster = 0;
    unsigned int entry = 0;
    unsigned int status = 0;
    unsigned int shorted_name = 0;
    unsigned int index = 0;
    char* path_buff = media_ptr->last_found.path_string;
    FAT32_DIR last_dir= {0};
    FAT32_DIR dest_dir = {0};

    while(name_ptr) {
        entry = 0;
        last_dir = dest_dir;
        while(*name_ptr == ' ')
            name_ptr++;
        if(*name_ptr == '\\' || *name_ptr == '/')
        {
            *path_buff = '/';
            path_buff++;
        }
        name_ptr = FAT32_directory_name_extract(name_ptr, media_ptr->last_found.path_name_buffer);
        do {
            if(last_dir.DIR_Attr && last_dir.DIR_Attr != ATTR_DIRECTORY && last_dir.DIR_Attr != ATTR_LONG_NAME)
                return FAT_NOT_FOUND;
            unsigned long BufEntry = entry;
            status = FAT32_directory_entry_read(media_ptr, &last_dir, &BufEntry, &dest_dir);
            shorted_name = 1;
            for(unsigned int times = 0;times < 10;times++)
            {
                if(dest_dir.DIR_name[times] == '~')
                    if(dest_dir.DIR_name[times + 1]=='1')
                    {
                        shorted_name = 0;
                        break;
                    }
            }
            if(FAT32_utility_string_compare(media_ptr->name_buff,media_ptr->last_found.path_name_buffer,shorted_name) == FAT_SUCCESS)
                break;
        }while(status != FAT_NO_MORE_CONTENT);
        for(index = 0;index < MAX_NAME_LENGTH && name_ptr;index++)
        {
            path_buff[index] = media_ptr->last_found.path_name_buffer[index];
            if(path_buff[index] == 0)
            {
                path_buff[index] = '/';
                path_buff[index + 1] = 0;
                path_buff += index;
                break;
            }
        }
        media_ptr->last_found.path_entry = entry - 1;
        media_ptr->last_found.path_dir = last_dir;
        if(status != FAT_SUCCESS)
            return  FAT_NOT_FOUND;
    }
    media_ptr->last_found.path_dir = last_dir;
    *entry_dir = dest_dir;

    entry_dir->LastAccessD.year = RTC::Year;
    entry_dir->LastAccessD.month = RTC::Month;
    entry_dir->LastAccessD.day = RTC::Day;

    media_ptr->last_found.path_entry = entry - 1;
    return  FAT_SUCCESS;
}






