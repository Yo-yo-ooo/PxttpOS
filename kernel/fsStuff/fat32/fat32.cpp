#include "fat32.h"

unsigned char buffer[10240];

Fat32::Fat32(int DiskNum){
    if (max_size != NULL) {
        media_ptr.status = 0;
        media_ptr.read_times = 0;
        media_ptr.write_times = 0;
        media_ptr.ID = DiskNum;
        media_ptr.max_size = max_size;
        media_ptr.media_name = "Fat32";
        media_ptr.open = nullptr;
        media_ptr.close = nullptr;
        media_ptr.read = nullptr;
        media_ptr.write = nullptr;
        FAT32_media_open(&media,&media_ptr,FAT32_driver,buffer,10240);
    }
    return;
}

bool Fat32::CreateFile(char *name_ptr){
    return FAT32_file_create(&media,name_ptr);
}

bool Fat32::DeleteFile(char *name_ptr){
    return FAT32_file_delete(&media,name_ptr);
}

bool Fat32::ReadFile(char *name_ptr, unsigned int offset, void *buffer, unsigned read_size){
    return FAT32_file_read(&media,name_ptr,offset,buffer,read_size);
}

bool Fat32::WriteFile(char *name_ptr, unsigned int offset, void *buffer, unsigned write_size){
    return FAT32_file_write(&media,name_ptr,offset,buffer,write_size);
}

bool Fat32::CreateDir(char *name_ptr){
    return FAT32_directory_create(&media,name_ptr);
}

bool Fat32::DeleteDir(char *name_ptr){
    return FAT32_directory_delete(&media,name_ptr);
}