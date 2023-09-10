#pragma once

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

    char * strcpy (char * dst, const char * src);
    char * strncpy (char *dest, const char *source, unsigned count);
    char * strcat (char * dst, const char * src);
    char *strncat (char *front, const char *back, unsigned count);
    int strcmp ( const char *s1, const char *s2);
    int strncmp(const char *s1, const char *s2, size_t n);
    char *strchr (const char *string, int chr);
    char *strrchr (const char *string, int ch);
    char *strchrnul (const char *string, int chr);
    char * strstr (const char * str1,const char * str2);
    void* memcpy(void* memTo, void* memFrom, size_t size);

#ifdef __cplusplus
}
#endif