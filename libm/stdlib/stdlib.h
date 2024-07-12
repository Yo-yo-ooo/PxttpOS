#pragma once

#include <stdint.h>
#include <stddef.h>
#include "cpuid.h"

#ifdef __cplusplus
#include <libm/rnd/rnd.h>
#define rand() RND::RandomInt()
inline void srand(uint32_t a){
    RND::g_lehmer64_state = (__uint128_t)a;
}
#endif

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
void* _malloc(size_t size);
void* _calloc(size_t num, size_t size);
void _free(void* ptr);