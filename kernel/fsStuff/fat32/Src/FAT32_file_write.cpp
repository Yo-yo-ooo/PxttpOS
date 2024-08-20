//
// Created by user on 2021/10/13.
//

#include "../Inc/FAT32_struct.h"
//更新原目录项filesize
unsigned int FAT32_file_write(FAT32_MEDIA *media_ptr, char*name_ptr, unsigned int offset, void *buffer, unsigned write_size)
{
    FAT32_DIR dest_dir;
    if(FAT32_directory_search(media_ptr,name_ptr,&dest_dir) != FAT_SUCCESS)
        return FAT_NOT_FOUND;
    if(offset >= dest_dir.file_size)
        offset = dest_dir.file_size;
    unsigned int cluster = dest_dir.first_cluster_l + (dest_dir.first_cluster_h << 16);
    unsigned int sector_offset = offset / media_ptr->boot.bytes_per_sector;
    unsigned int cluster_offset = sector_offset / media_ptr->boot.sectors_per_cluster;
    unsigned int byte_offset = offset % media_ptr->boot.bytes_per_sector;
    sector_offset %= media_ptr->boot.sectors_per_cluster;
    for(;cluster_offset;cluster_offset--)
        FAT32_utility_FAT_read(media_ptr,cluster,&cluster);
    while(write_size)
    {

        for(;cluster_offset < media_ptr->boot.sectors_per_cluster;cluster_offset++)
        {
            FAT32_utility_logic_sector_read(media_ptr,media_ptr->memory_buff, FAT32_utility_cluster_to_sector(media_ptr,cluster) + sector_offset,1);
            if(media_ptr->boot.bytes_per_sector - byte_offset > write_size) {
                FAT32_utility_memory_copy(media_ptr->memory_buff, buffer,  write_size);
                write_size = 0;
            }
            else {
                FAT32_utility_memory_copy(media_ptr->memory_buff, buffer,
                                          media_ptr->boot.bytes_per_sector - byte_offset);
                write_size -=media_ptr->boot.bytes_per_sector - byte_offset;
            }
            byte_offset = 0;

        }
        FAT32_utility_FAT_read(media_ptr,cluster,&cluster);
        if(cluster == FAT32_END)
        {
            FAT32_utility_alloc_cluster(media_ptr,cluster,&cluster);
        }
    }




    return FAT_SUCCESS;
}
