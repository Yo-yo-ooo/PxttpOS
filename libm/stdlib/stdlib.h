#pragma once

#include <stdint.h>
#include <stddef.h>

//convert
int atoi(char* pstr);
double atof(const char *str);
long atol(const char *nptr);
char *itoa(int val, char *buf, unsigned radix);
double strtod(const char *str, char **endptr);
long strtol(const char *str, char **endptr, int base);

//sort
void qsort(void *base, size_t nitems, size_t size, int (*compar)(const void *, const void*));

//memory
void* malloc(size_t size);
void* calloc(size_t num, size_t size);
void* realloc(void* ptr, size_t size);
void free(void* ptr);