#pragma once

#include "../../rendering/BasicRenderer.h"
#include "../pci/pci.h"
#include "../../kernelStuff/stuff/stdArg.h"
#include <stdint.h>
#include <stddef.h>

void __PrintfMsg(const char* msg, ...);

#define info(...)     
#define error(...)    
#define warn(...)     

#define asm_getu(addr)                                                                             \
  ({                                                                                               \
    size_t _var;                                                                                   \
    asm volatile("mov (%1), %0\n\t" : "=r"(_var) : "r"((void *)(addr)));                           \
    _var;                                                                                          \
  })

#define asm_getu8(addr)                                                                            \
  ({                                                                                               \
    uint8_t _var;                                                                                  \
    asm volatile("movb (%1), %0\n\t" : "=r"(_var) : "r"((void *)(addr)));                          \
    _var;                                                                                          \
  })

#define asm_getu16(addr)                                                                           \
  ({                                                                                               \
    uint16_t _var;                                                                                 \
    asm volatile("movw (%1), %0\n\t" : "=r"(_var) : "r"((void *)(addr)));                          \
    _var;                                                                                          \
  })

#define asm_getu32(addr)                                                                           \
  ({                                                                                               \
    uint32_t _var;                                                                                 \
    asm volatile("movl (%1), %0\n\t" : "=r"(_var) : "r"((void *)(addr)));                          \
    _var;                                                                                          \
  })

#define asm_getu64(addr)                                                                           \
  ({                                                                                               \
    uint64_t _var;                                                                                 \
    asm volatile("movq (%1), %0\n\t" : "=r"(_var) : "r"((void *)(addr)));                          \
    _var;                                                                                          \
  })

#define asm_geti(addr)                                                                             \
  ({                                                                                               \
    ssize_t _var;                                                                                  \
    asm volatile("mov (%1), %0\n\t" : "=r"(_var) : "r"((void *)(addr)));                           \
    _var;                                                                                          \
  })

#define asm_geti8(addr)                                                                            \
  ({                                                                                               \
    int8_t _var;                                                                                   \
    asm volatile("movb (%1), %0\n\t" : "=r"(_var) : "r"((void *)(addr)));                          \
    _var;                                                                                          \
  })

#define asm_geti16(addr)                                                                           \
  ({                                                                                               \
    int16_t _var;                                                                                  \
    asm volatile("movw (%1), %0\n\t" : "=r"(_var) : "r"((void *)(addr)));                          \
    _var;                                                                                          \
  })

#define asm_geti32(addr)                                                                           \
  ({                                                                                               \
    int32_t _var;                                                                                  \
    asm volatile("movl (%1), %0\n\t" : "=r"(_var) : "r"((void *)(addr)));                          \
    _var;                                                                                          \
  })

#define asm_geti64(addr)                                                                           \
  ({                                                                                               \
    int64_t _var;                                                                                  \
    asm volatile("movq (%1), %0\n\t" : "=r"(_var) : "r"((void *)(addr)));                          \
    _var;                                                                                          \
  })

#define asm_get(addr)   asm_getu(addr)
#define asm_get8(addr)  asm_getu8(addr)
#define asm_get16(addr) asm_getu16(addr)
#define asm_get32(addr) asm_getu32(addr)

#define asm_set(addr, val)                                                                         \
  ({ asm volatile("mov %0, (%1)\n\t" : : "r"((size_t)(val)), "r"((void *)(addr))); })

#define asm_set8(addr, val)                                                                        \
  ({ asm volatile("movb %0, (%1)\n\t" : : "r"((uint8_t)(val)), "r"((void *)(addr))); })

#define asm_set16(addr, val)                                                                       \
  ({ asm volatile("movw %0, (%1)\n\t" : : "r"((uint16_t)(val)), "r"((void *)(addr))); })

#define asm_set32(addr, val)                                                                       \
  ({ asm volatile("movl %0, (%1)\n\t" : : "r"((uint32_t)(val)), "r"((void *)(addr))); })

#define asm_set64(addr, val)                                                                       \
  ({ asm volatile("movq %0, (%1)\n\t" : : "r"((uint64_t)(val)), "r"((void *)(addr))); })


#define waitif(cond)                                                                               \
  ((void)({                                                                                        \
    while (cond) {}                                                                                \
  }))

#define waituntil(cond)                                                                            \
  ((void)({                                                                                        \
    while (!(cond)) {}                                                                             \
  }))

#define asm_in8(port)                                                                              \
  ({                                                                                               \
    size_t          data;                                                                          \
    size_t          __arg1         = (size_t)(port);                                               \
    register size_t _a1 asm("edx") = __arg1;                                                       \
    asm volatile("inb %%dx, %%al\n\t" : "=a"(data) : "r"(_a1) : "memory");                         \
    data;                                                                                          \
  })

#define asm_in16(port)                                                                             \
  ({                                                                                               \
    size_t          data;                                                                          \
    size_t          __arg1         = (size_t)(port);                                               \
    register size_t _a1 asm("edx") = __arg1;                                                       \
    asm volatile("inw %%dx, %%ax\n\t" : "=a"(data) : "r"(_a1) : "memory");                         \
    data;                                                                                          \
  })

#define asm_in32(port)                                                                             \
  ({                                                                                               \
    size_t          data;                                                                          \
    size_t          __arg1         = (size_t)(port);                                               \
    register size_t _a1 asm("edx") = __arg1;                                                       \
    asm volatile("inl %%dx, %%eax\n\t" : "=a"(data) : "r"(_a1) : "memory");                        \
    data;                                                                                          \
  })

#define asm_out8(port, data)                                                                       \
  ({                                                                                               \
    size_t          __arg1         = (size_t)(data);                                               \
    size_t          __arg2         = (size_t)(port);                                               \
    register size_t _a2 asm("edx") = __arg2;                                                       \
    register size_t _a1 asm("eax") = __arg1;                                                       \
    asm volatile("outb %%al, %%dx\n\t" : : "r"(_a1), "r"(_a2) : "memory");                         \
    (void)0;                                                                                       \
  })

#define asm_out16(port, data)                                                                      \
  ({                                                                                               \
    size_t          __arg1         = (size_t)(data);                                               \
    size_t          __arg2         = (size_t)(port);                                               \
    register size_t _a2 asm("edx") = __arg2;                                                       \
    register size_t _a1 asm("eax") = __arg1;                                                       \
    asm volatile("outw %%ax, %%dx\n\t" : : "r"(_a1), "r"(_a2) : "memory");                         \
    (void)0;                                                                                       \
  })

#define asm_out32(port, data)                                                                      \
  ({                                                                                               \
    size_t          __arg1         = (size_t)(data);                                               \
    size_t          __arg2         = (size_t)(port);                                               \
    register size_t _a2 asm("edx") = __arg2;                                                       \
    register size_t _a1 asm("eax") = __arg1;                                                       \
    asm volatile("outl %%eax, %%dx\n\t" : : "r"(_a1), "r"(_a2) : "memory");                        \
    (void)0;                                                                                       \
  })


typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

#define null 0