#include <stdint.h>
#include <stddef.h>
#include "../packdef.h"

#define EOF (-1)
#define OPEN_MAX 20

#define O_RDONLY    0b1000000000000000
#define O_WRONLY    0b0100000000000000
#define O_RDWR      0b0010000000000000
#define O_CREAT     0b0001000000000000
#define O_EXCL      0b0000100000000000
#define O_NOCTTY    0b0000010000000000
#define O_TRUNC     0b0000001000000000
#define O_APPEND    0b0000000100000000
#define O_NONBLOCK  0b0000000010000000
#define O_DSYNC     0b0000000001000000
#define O_FASYNC    0b0000000000100000
#define O_NOFOLLOW  0b0000000000010000

PACK(typedef struct vfs_node{
    int (*vfs_open)(const char *pathname, int flags);
    int (*vfs_open_)(const char *pathname, int flags, unsigned int mode);
    int (*vfs_read)(int fd, void *buf, size_t count);
    int (*vfs_write)(int fd, const void *buf, size_t count);
})vfs_node_t;

