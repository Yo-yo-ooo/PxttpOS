#pragma once

#ifndef _OS_STDLIB_H_ 
#define _OS_STDLIB_H_

#include <stdint.h>
#include <stddef.h>
#include "cpuid.h"

//convert
int _atoi(char* pstr);
double _atof(const char *str);
long _atol(const char *nptr);
char *_itoa(int val, char *buf, unsigned radix);
double _strtod(const char *str, char **endptr);
long _strtol(const char *str, char **endptr, int base);
unsigned long _strtoul(const char *str, char **endptr, int base);
unsigned long long _strtoull(const char *nptr, char **endptr, int base);

//sort
void _qsort(void *base, size_t nitems, size_t size, int (*compar)(const void *, const void*));

//search
void *_bsearch(const void *key, const void *base, size_t nitems, size_t size, int (*compar)(const void *, const void *));

//memory
void* OS_malloc(size_t size);
void* OS_calloc(size_t num, size_t size);
void  OS_free(void* ptr);
void* OS_realloc(void* ptr, size_t size);


#endif // 