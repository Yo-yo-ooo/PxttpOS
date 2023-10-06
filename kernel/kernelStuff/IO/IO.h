#pragma once
#include <stdint.h>

void outb(uint16_t port, uint8_t value);
uint8_t inb(uint16_t port);
void io_wait();   


void outw(uint16_t port, uint16_t value);
uint16_t inw(uint16_t port);

void outl(uint16_t port, uint32_t value);
uint32_t inl(uint16_t port);

void io_wait(uint64_t us);

// memory mapped io stuff

void mOutb(uint64_t address, uint8_t value);
uint8_t mInb(uint64_t address);

void mOutw(uint64_t address, uint16_t value);
uint16_t mInw(uint64_t address);

void mOutl(uint64_t address, uint32_t value);
uint32_t mInl(uint64_t address);

void mOutq(uint64_t address, uint64_t value);
uint64_t mInq(uint64_t address);

inline uint64_t rdtsc()
{
    uint32_t out;
    __asm__ volatile("rdtsc" : "=A"(out));
    return out;
}

inline uint64_t rdmsr(uint64_t msr)
{
    uint32_t low, high;
    __asm__ volatile ("rdmsr" : "=a"(low), "=d"(high) : "c"(msr));
    return (( (uint64_t)high) << 32) | low;
}


inline void wrmsr(uint64_t msr, uint64_t value)
{
    uint32_t low = msr & 0xFFFFFFFF, high = msr >> 32;
    __asm__ volatile("wrmsr" 
        : 
        : "c"(msr), "a"(low), "d"(high)
    );
}

inline void write_reg(uint32_t reg ,uint32_t value){
    *(volatile uint32_t*)reg = value;
}

inline uint32_t ReadRegister(uint32_t registerNumber) {
    uint32_t value;
    asm volatile("mov %0, %1" : "=r" (value) : "r" (registerNumber));
    return value;
}