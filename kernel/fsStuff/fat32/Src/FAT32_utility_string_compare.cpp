//
// Created by user on 2021/10/7.
//

#include "../Inc/FAT32_struct.h"
unsigned int FAT32_utility_string_compare(char *str1,char* str2,unsigned int shorted_name_flag)
{
    for(;*str1 && *str2;str1++,str2++)
    {
        if(shorted_name_flag) {
            if (FAT32_CMP(*str1, *str2) == 0) {
                return 1;
            }
        }
        else
            if(*str1 != *str2)
                return 1;
    }
    if(shorted_name_flag) {
        if (*str1 != *str2)
            return 1;
    }
    else
        if(FAT32_CMP(*str1,*str2) == 0)
            return 1;
    return FAT_SUCCESS;
}
