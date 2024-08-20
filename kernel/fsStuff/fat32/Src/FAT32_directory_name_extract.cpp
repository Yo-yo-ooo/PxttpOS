//
// Created by user on 2021/10/1.
//

#include "../Inc/FAT32_struct.h"

char* FAT32_directory_name_extract(char *source_ptr,char *dest_ptr)
{
    unsigned int index = 0;
    dest_ptr[0] = 0;
    if((*source_ptr) == '/' || (*source_ptr)=='\\')
        source_ptr++;
    while((*source_ptr) == ' ')
        source_ptr++;

    for(index = 0;index < FAT_MAX_LONG_NAME_LEN - 1;index++,source_ptr++)
    {
        if((*source_ptr) == '\\' || (*source_ptr) == '/' || (*source_ptr) == 0)
            break;
        dest_ptr[index] = *source_ptr;
    }
    dest_ptr[index] = 0;
    if(index)
        index--;
    for(;dest_ptr[index]==' ';index--)
        dest_ptr[index] = 0;
    if(*source_ptr == 0)
        return NULL;
    return source_ptr;
}
