#include <stdint.h>
#include <stddef.h>
#include "../packdef.h"

PACK(typedef struct vfs_node{
    int (*vfs_open)(void *media, char *name_ptr);
    int (*vfs_close)(void *media);
    int (*vfs_read)(void *media, unsigned int offset, void *buffer, unsigned int read_size);
    int (*vfs_write)(void *media, unsigned int offset, void *buffer, unsigned int write_size);
})vfs_node_t;

/* 打开文件的选项 */
enum oflags {
   O_RDONLY,	  // 只读
   O_WRONLY,	  // 只写
   O_RDWR,	  // 读写
   O_CREAT = 4	  // 创建
};
