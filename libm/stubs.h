#pragma once
#include <stdint.h>
#include <stddef.h>

#ifdef _KERNEL_SRC

#include "../kernel/osData/MStack/MStackM.h"
#include "../kernel/memory/heap.h"
#include "../kernel/interrupts/panic.h"
#include "../kernel/osData/MStack/MStackM.h"
#include "../kernel/devices/serial/serial.h"

#endif


inline void memset(void* start, uint8_t value, uint64_t num)
{
    for (uint64_t i = 0; i < num; i++)
        *(uint8_t*)((uint64_t)start + i) = value;
}

inline void memcpy(const void* src, void* dest, uint64_t size)
{
    const char* _src  = (const char*)src;
    char* _dest = (char*)dest;
    while (size--)
    {
        *_dest = *_src;
        _dest++;
        _src++;
    }
}

inline int memcmp(const void* src, const void* dest, int amt)
{
    const char* fi = (const char*)src;
    const char* la = (const char*)dest;
    for (int i = 0; i < amt; i++)
    {
        if (fi[i] > la[i])
            return 1;
        if (fi[i] < la[i])
            return -1;
    }
    return 0;
}

#ifdef _KERNEL_MODULE

inline void Panic(const char* message, bool stop = false)
{
    asm("cli");
    asm("hlt");
}
inline void* _Malloc(uint64_t size, const char* name)
{
    return NULL;
}
inline void* _Malloc(uint64_t size)
{
    return NULL;
}

inline void _Free(void* ptr)
{

}

namespace Serial
{
    inline void Writelnf(const char* format, ...)
    {

    }
}

#define AddToStack()
#define RemoveFromStack()

#endif

#ifdef _USER_MODULE

inline void Panic(const char* message, bool stop = false)
{
    asm("cli");
    asm("hlt");
}
inline void* _Malloc(uint64_t size, const char* name)
{
    return NULL;
}
inline void* _Malloc(uint64_t size)
{
    return NULL;
}

inline void _Free(void* ptr)
{

}

namespace Serial
{
    inline void Writelnf(const char* format, ...)
    {

    }
}

#define AddToStack()
#define RemoveFromStack()
#endif