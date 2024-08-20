//
// Created by user on 2021/10/12.
//

#include "../Inc/FAT32_struct.h"
unsigned int FAT32_utility_string_length(char *string)
{
    unsigned int length = 0;
    while(*string) {
        length++;
        string++;
    }
    return length;
}
