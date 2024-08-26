//
// Created by user on 2021/10/13.
//

#include "../Inc/FAT32_struct.h"

unsigned int FAT32_file_read(FAT32_MEDIA *media_ptr, char*name_ptr, unsigned int offset, void *buffer, unsigned read_size)
{
    FAT32_DIR dest_dir;
    if(FAT32_directory_search(media_ptr,name_ptr,&dest_dir) != FAT_SUCCESS)
        return FAT_NOT_FOUND;
    if(offset >= dest_dir.file_size)
        return FAT_SUCCESS;
    if(offset + read_size > dest_dir.file_size)
        read_size = dest_dir.file_size - offset;
    unsigned int cluster = dest_dir.first_cluster_l + (dest_dir.first_cluster_h << 16);
    unsigned int sector_offset = offset / media_ptr->boot.bytes_per_sector;
    unsigned int cluster_offset = sector_offset / media_ptr->boot.sectors_per_cluster;
    unsigned int byte_offset = offset % media_ptr->boot.bytes_per_sector;
    sector_offset %= media_ptr->boot.sectors_per_cluster;
    for(;cluster_offset;cluster_offset--)
        FAT32_utility_FAT_read(media_ptr,cluster,&cluster);
    while(read_size)
    {

        for(;cluster_offset < media_ptr->boot.sectors_per_cluster;cluster_offset++)
        {
            FAT32_utility_logic_sector_read(media_ptr,media_ptr->memory_buff, FAT32_utility_cluster_to_sector(media_ptr,cluster) + sector_offset,1);
            if(media_ptr->boot.bytes_per_sector - byte_offset > read_size) {
                FAT32_utility_memory_copy(media_ptr->memory_buff, buffer, read_size);
                read_size = 0;
            }
            else {
                FAT32_utility_memory_copy(media_ptr->memory_buff, buffer,
                                          media_ptr->boot.bytes_per_sector - byte_offset);
                read_size -=media_ptr->boot.bytes_per_sector - byte_offset;
            }
            byte_offset = 0;

        }
        FAT32_utility_FAT_read(media_ptr,cluster,&cluster);
    }

    dest_dir.LastAccessD.day = RTC::Day;
    dest_dir.LastAccessD.month = RTC::Month;
    dest_dir.LastAccessD.year = RTC::Year;

    return FAT_SUCCESS;
}
