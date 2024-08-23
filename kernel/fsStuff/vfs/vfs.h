#include <stdint.h>
#include <stddef.h>
#include "../packdef.h"

PACK(typedef struct vfs_node{
    int (*vfs_open)(void *media, char *name_ptr);
    int (*vfs_close)(void *media);
    int (*vfs_read)(void *media, unsigned int offset, void *buffer, unsigned int read_size);
    int (*vfs_write)(void *media, unsigned int offset, void *buffer, unsigned int write_size);
})vfs_node_t;

