#pragma once

#include <stddef.h>
#include <stdint.h>



#define IOAPICID          0x00
#define IOAPICVER         0x01
#define IOAPICARB         0x02
#define IOAPICREDTBL(n)   (0x10 + 2 * n) // lower-32bits (add +1 for upper 32-bits)

union RedirectionEntry
{
    struct
    {
        uint64_t vector       : 8;
        uint64_t delvMode     : 3;
        uint64_t destMode     : 1;
        uint64_t delvStatus   : 1;
        uint64_t pinPolarity  : 1;
        uint64_t remoteIRR    : 1;
        uint64_t triggerMode  : 1;
        uint64_t mask         : 1;
        uint64_t reserved     : 39;
        uint64_t destination  : 8;
    };

    struct
    {
        uint32_t lowerDword;
        uint32_t upperDword;
    };
};

void IOAPIC_INIT(unsigned long physRegs, unsigned long apicId, unsigned long gsib);

RedirectionEntry IOAPIC_GetRedirEntry(unsigned char entNo);
void IOAPIC_WriteRedirEntry(unsigned char entNo, RedirectionEntry *entry);
unsigned char IOAPIC_id();
unsigned char IOAPIC_ver();
unsigned char IOAPIC_redirectionEntries();
unsigned long IOAPIC_globalInterruptBase();

#define IA32_APIC_BASE_MSR 0x1B
#define IA32_APIC_BASE_MSR_BSP 0x100 // Processor is a BSP
#define IA32_APIC_BASE_MSR_ENABLE 0x800

uint32_t CpuReadIoApic(void *ioapicaddr, uint32_t reg);
void CpuWriteIoApic(void *ioapicaddr, uint32_t reg, uint32_t value);
 