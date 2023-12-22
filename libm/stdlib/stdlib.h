#pragma once

#include <stdint.h>
#include <stddef.h>

int atoi(char* pstr);
double atof(const char *str);

//memory
void* malloc(size_t size);
void* calloc(size_t num, size_t size);
void* realloc(void* ptr, size_t size);
void free(void* ptr);