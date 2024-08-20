//
// Created by user on 2021/10/12.
//

#include "../Inc/FAT32_struct.h"

unsigned int FAT32_utility_alloc_cluster(FAT32_MEDIA *media_ptr, unsigned int last_cluster, unsigned int *cluster_ptr)
{
    unsigned int status = 0;
    status = FAT32_utility_FAT_find_free(media_ptr,cluster_ptr);
    if(status != FAT_SUCCESS)
        return status;
    if(last_cluster >= 2)
        status = FAT32_utility_FAT_write(media_ptr,last_cluster,*cluster_ptr);






    return status;
}
