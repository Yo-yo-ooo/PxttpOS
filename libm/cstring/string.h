#pragma once

#include <stddef.h>

#define NULL    ((void *)0)

void *_memchr(const void *_s, int _c, size_t _n);
int _memcmp(const void *_s1, const void *_s2, size_t _n);
//void *_memcpy(void *_s1, const void *_s2, size_t _n);
//void *_memmove(void *_s1, const void *_s2, size_t _n);
//void *_memset(void *_s, int _c, size_t _n);
char *_strcat(char *_s1, const char *_s2);
char *_strchr(const char *_s, int _c);
int _strncmp(const char *_s1, const char *_s2, size_t _n);
int _strcmp(const char *_s1, const char *_s2);
int _strcoll(const char *_s1, const char *_s2);
char *_strcpy(char *_s1, const char *_s2);
size_t _strcspn(const char *_s1, const char *_s2);
char *_strerror(int _errnum);
size_t _strlen(const char *_s);
char *_strncat(char *_s1, const char *_s2, size_t _n);
char *_strncpy(char *_s1, const char *_s2, size_t _n);
char *_strpbrk(const char *_s1, const char *_s2);
char *_strrchr(const char *_s, int _c);
size_t _strspn(const char *_s1, const char *_s2);
char *_strstr(const char *_s1, const char *_s2);
char *_strtok(char *_s1, const char *_s2);
size_t _strxfrm(char *_s1, const char *_s2, size_t _n);