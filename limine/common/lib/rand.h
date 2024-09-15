#ifndef LIB__RAND_H__
#define LIB__RAND_H__

#include <stdint.h>

void srand(uint32_t s);

uint32_t rand32(void);
uint64_t rand64(void);

#endif
