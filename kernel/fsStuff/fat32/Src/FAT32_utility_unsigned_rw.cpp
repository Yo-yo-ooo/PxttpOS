//
// Created by user on 2021/10/1.
//


#include "../Inc/FAT32_struct.h"



unsigned short FAT32_utility_16_unsigned_read(void *source_ptr)
{
    return *((unsigned short*)source_ptr);
}
unsigned int FAT32_utility_32_unsigned_read(void *source_ptr)
{
    return *((unsigned int*)source_ptr);
}
void FAT32_utility_16_unsigned_write(void *dest_ptr,unsigned short value)
{
    *((unsigned short*)dest_ptr) = value;
}
void FAT32_utility_32_unsigned_write(void *dest_ptr,unsigned int value)
{
    *((unsigned int*)dest_ptr) = value;
}
