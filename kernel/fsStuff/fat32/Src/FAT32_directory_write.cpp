//
// Created by user on 2021/10/12.
//

#include "../Inc/FAT32_struct.h"

unsigned int FAT32_directory_write(FAT32_MEDIA *media_ptr, FAT32_DIR *dest_dir, char *name_ptr, unsigned char ATTR,unsigned int local_cluster,unsigned int size)
{
    unsigned int name_length = FAT32_utility_string_length(name_ptr);
    unsigned int need_free_dir = 1;
    unsigned int dir_offset;
    unsigned int sector_offset = 0;
    unsigned int cluster_offset = 0;
    unsigned int byte_offset = 0;
    unsigned int shorted_name = 0;
    struct FAT32_DIR *dir_ptr = NULL;
    unsigned int cluster = dest_dir->first_cluster_l + (dest_dir->first_cluster_h << 16);
    unsigned int dot_local = 0;
    for(;dot_local < name_length;dot_local++)
        if(name_ptr[dot_local] == '.')
            break;
    if(name_length > 11)
    {
        if(name_length < 13)
            need_free_dir = 2;
        else
            need_free_dir += name_length / FAT_LONG_NAME_DIR_CAP;
        shorted_name = 1;
    }
    unsigned int order = need_free_dir - 1;
    FAT32_directory_free_find(media_ptr,dest_dir,need_free_dir,&dir_offset);
    cluster_offset = (dir_offset * sizeof(struct FAT32_DIR) / media_ptr->boot.bytes_per_sector) / media_ptr->boot.sectors_per_cluster;
    sector_offset =(dir_offset * sizeof(struct FAT32_DIR) / media_ptr->boot.bytes_per_sector) % media_ptr->boot.sectors_per_cluster;
    byte_offset = (dir_offset * sizeof(struct FAT32_DIR)) % media_ptr->boot.bytes_per_sector;
    for(;cluster_offset;cluster_offset--)
        FAT32_utility_FAT_read(media_ptr,cluster,&cluster);
    for(;sector_offset < media_ptr->boot.sectors_per_cluster;sector_offset++)
    {
        FAT32_utility_logic_sector_read(media_ptr, media_ptr->memory_buff,FAT32_utility_cluster_to_sector(media_ptr,cluster) + sector_offset,1);
        for(;byte_offset < media_ptr->boot.bytes_per_sector;byte_offset += sizeof(FAT32_DIR))
        {

            dir_ptr = media_ptr->memory_buff + byte_offset;
            if(order)
            {
                unsigned int offset = (order - 1) * FAT_LONG_NAME_DIR_CAP;
                struct FAT32_DIR_long_name* long_dir = (void*)dir_ptr;
                long_dir->order = order;
                long_dir->DIR_attr = ATTR_LONG_NAME;
                for(unsigned int times = 0;times < FAT_LONG_NAME_DIR_CAP;times++)
                {
                    unsigned char ch = name_ptr[offset + times];
                    if(offset + times >= name_length)
                        ch = 0;
                    if(times < 5)
                        long_dir->name1[times * 2] = ch;
                    else if(times < 11)
                        long_dir->name2[(times - 5) * 2] = ch;
                    else
                        long_dir->name3[(times - 11) * 2] = ch;
                }


                if(order == need_free_dir - 1)
                    long_dir->order += 0x40;
                order--;
            }
            else
            {
                if(shorted_name)
                {
                    dir_ptr->DIR_name[0] = name_ptr[0];
                    dir_ptr->DIR_name[1] = name_ptr[1];
                    dir_ptr->DIR_name[2] = name_ptr[2];
                    dir_ptr->DIR_name[3] = '~';
                    dir_ptr->DIR_name[4] = '1';
                    unsigned int local = 10;
                    for(name_length--;name_length > dot_local && local > 5;name_length--,local--)
                    {
                        dir_ptr->DIR_name[local] = name_ptr[name_length];
                    }
                }
                else
                {
                    for(unsigned int local = 0;local <dot_local;local++)
                    {
                        dir_ptr->DIR_name[local] = name_ptr[local];
                    }
                    unsigned int local = 10;
                    for(name_length--;name_length > dot_local && local > 5;name_length--,local--)
                    {
                        dir_ptr->DIR_name[local] = name_ptr[name_length];
                    }
                }
                for(unsigned int index = 0;index < 11;index++)
                {
                    if(dir_ptr->DIR_name[index] >= 'a' && dir_ptr->DIR_name[index] <= 'z')
                        dir_ptr->DIR_name[index] -= 0x20;
                }

                dir_ptr->DIR_Attr = ATTR;
                dir_ptr->file_size = size;
                dir_ptr->first_cluster_h = cluster >> 16;
                dir_ptr->first_cluster_l = cluster;

                dir_ptr->LastAccessD.year = RTC::Year;
                dir_ptr->LastAccessD.month = RTC::Month;
                dir_ptr->LastAccessD.day = RTC::Day;

                return FAT_SUCCESS;
            }
        }
        FAT32_utility_logic_sector_write(media_ptr, media_ptr->memory_buff,FAT32_utility_cluster_to_sector(media_ptr,cluster) + sector_offset,1);
        byte_offset = 0;
    }
    return FAT_UNKNOWN_ERROR;
}
