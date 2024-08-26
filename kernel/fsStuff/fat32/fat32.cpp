#include "fat32.h"
#include "../../devices/rtc/rtc.h"
#include "../vfs/vfs.h"
#include <libm/memStuff.h>

using namespace DiskInterface;

Fat32::Fat32(int DiskNum){
    max_size = ((GenericDiskInterface*)osData.diskInterfaces[DiskNum])->GetMaxSectorCount() * SECTOR_SIZE;
    Tfd = 4;
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

#define GET_BIT(value,bit) ((value)&(1<<(bit)))    //读取指定位
#define CPL_BIT(value,bit) ((value)^=(1<<(bit)))   //取反指定位

#define SET0_BIT(value,bit) ((value)&=~(1<<(bit))) //把某个位置0
#define SET1_BIT(value,bit) ((value)|= (1<<(bit))) //把某个位置1

int Fat32::open(const char *pathname, int flag){
    AddToStack();
    FAT32_DIR dir;

    unsigned int EID = FAT32_directory_search(&media,pathname,&dir);

    if (EID == FAT_NOT_FOUND)
        PrintfMsg("File Open FUNC:can`t open a direcotry with open(), use opendir() to instead");
    else if (EID == FAT_NOT_FOUND)
        PrintfMsg("File Open FUNC:can`t open a file %s",pathname);

    if (Tfd <= 3){
        PrintfMsg("File Open FUNC:too many files opened");
        return -1;
    }
    else if (Tfd > 255){
        PrintfMsg("File Open FUNC:too many files opened");
        return -1;
    }
    
    switch (flag & O_CREAT) {
        case O_CREAT:{
            PrintfMsg("File Open FUNC:creating file");
            if(FAT32_file_create(&media, pathname) != FAT_SUCCESS)
                return -1;
            else
                Tfd++;
        }
        break;
        default:{
            Tfd++;
        }
    }
    RemoveFromStack();

    return Tfd;

}

static unsigned int _FAT32_directory_write(FAT32_MEDIA *media_ptr, FAT32_DIR *dest_dir, char *name_ptr, unsigned char ATTR,unsigned int local_cluster,unsigned int size,int perms)
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

                dir_ptr->reserved2 = perms;

                return FAT_SUCCESS;
            }
        }
        FAT32_utility_logic_sector_write(media_ptr, media_ptr->memory_buff,FAT32_utility_cluster_to_sector(media_ptr,cluster) + sector_offset,1);
        byte_offset = 0;
    }
    return FAT_UNKNOWN_ERROR;
}
unsigned int _FAT32_file_create(FAT32_MEDIA *media_ptr,char *name_ptr,int perms)
{
    struct FAT32_DIR temp_dir;
    unsigned int status = 0;
    char *origin_ptr = name_ptr;
    if(FAT32_directory_search(media_ptr,name_ptr,&temp_dir) == FAT_SUCCESS)
        return FAT_INVALID_PARA;
    char *previous_name_ptr = name_ptr;

    do{
        name_ptr = previous_name_ptr;
        previous_name_ptr = FAT32_directory_name_extract(name_ptr,media_ptr->name_buff);
    }while(previous_name_ptr);
    unsigned int cluster =0;
    FAT32_utility_alloc_cluster(media_ptr,0,&cluster);

    return _FAT32_directory_write(media_ptr,&media_ptr->last_found.path_dir,name_ptr,ATTR_ARCHIVE,cluster,0,perms);
}

int Fat32::creat(char* name, int perms){
    AddToStack();
    FAT32_DIR dir;

    unsigned int EID = FAT32_directory_search(&media,name,&dir);

    if (Tfd <= 3){
        PrintfMsg("File Open FUNC:too many files opened");
        return -1;
    }
    else if (Tfd > 255){
        PrintfMsg("File Open FUNC:too many files opened");
        return -1;
    }

    if(_FAT32_file_create(&media,name,perms) != FAT_SUCCESS)
        return -1;
    else
        Tfd++;
    

    RemoveFromStack();

    return Tfd;
}

FILE* Fat32::fopen( const char *filename, const char *mode){
    int fd;
    FILE* fp;

    if(*mode != 'r' && *mode != 'w' && *mode != 'a')
        return NULL;
    for (fp = _iob; fp < _iob + 20; fp++)
        if (fp->_flag & (_READ | _WRITE) == 0)
            break;
    if (fp >= _iob + 20)
        return NULL;
    if (*mode == 'w')
        fd = this->creat(filename,0666);
    else if (*mode == 'a'){
        if((fd = this->open(filename, O_WRONLY)) == -1)
            fd = this->creat(filename,0666);
    }else
        fd = this->open((char*)filename,0);

    if(fd == -1)
        return NULL;
    fp->_file = fd;
    fp->_flag = (*mode == 'r') ? _READ : _WRITE;
    fp->_cnt = 0;
    fp->_base = NULL;
    return fp;
}