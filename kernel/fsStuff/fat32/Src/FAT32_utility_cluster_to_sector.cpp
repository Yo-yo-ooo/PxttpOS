//
// Created by user on 2021/10/7.
//

#include "../Inc/FAT32_struct.h"
unsigned int FAT32_utility_cluster_to_sector(FAT32_MEDIA* media_ptr,unsigned int cluster_number)
{
return media_ptr->boot.sectors_per_FAT * media_ptr->boot.number_of_FAT + media_ptr->boot.reserved_sectors +
(cluster_number - 2) * media_ptr->boot.sectors_per_cluster;
}