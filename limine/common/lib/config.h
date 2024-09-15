#ifndef LIB__CONFIG_H__
#define LIB__CONFIG_H__

#include <stddef.h>
#include <stdbool.h>
#include <lib/part.h>

extern bool config_ready;
extern bool bad_config;

struct menu_entry {
    char name[64];
    char *comment;
    struct menu_entry *parent;
    struct menu_entry *sub;
    bool expanded;
    char *body;
    struct menu_entry *next;
};

struct conf_tuple {
    char *value1;
    char *value2;
};

extern bool config_format_old;
extern struct menu_entry *menu_tree;

int init_config_disk(struct volume *part);
int init_config(size_t config_size);

char *config_get_value(const char *config, size_t index, const char *key);
struct conf_tuple config_get_tuple(const char *config, size_t index,
                                   const char *key1, const char *key2);

#endif
