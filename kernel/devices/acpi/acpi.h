#pragma once
#include <stdint.h>

namespace ACPI
{
    struct RSDP1
    {
        unsigned char Signature[8];
        uint8_t Checksum;
        uint8_t OEM_ID[6];
        uint8_t Revision;
        uint32_t RSDTAddress;
    } __attribute__((packed));

    struct RSDP2
    {
        RSDP1 firstPart;

        uint32_t Length;
        uint64_t XSDTAddress;
        uint8_t ExtendedChecksum;
        uint8_t Reserved[3];

    } __attribute__((packed));



    struct SDTHeader
    {
        unsigned char Signature[4];
        uint32_t Length;
        uint8_t Revision;
        uint8_t Checksum;
        uint8_t OEM_ID[6];
        uint8_t OEM_TABLE_ID[8];
        uint32_t OEM_Revision;
        uint32_t Creator_ID; 
        uint32_t CreatorRevision;
    } __attribute__((packed));

    struct MCFGHeader
    {
        SDTHeader Header;
        uint64_t Reserved;
    } __attribute__((packed));

    struct DeviceConfig
    {
        uint64_t BaseAddress;
        uint16_t PCISegGroup;
        uint8_t StartBus;
        uint8_t EndBus;
        uint32_t Reserved;
    } __attribute__((packed));


    void* FindTable(SDTHeader* sdtHeader, const char* signature, int div);
    bool CheckSumHeader(void* start, uint8_t extra, uint32_t len);

}  

// register offsets
#define LAPIC_ID      	   0x20
#define LAPIC_VERSION      0x30
#define LAPIC_TASKPRI      0x80
#define LAPIC_ARBPRI       0x90
#define LAPIC_PROCPRI      0xA0
#define LAPIC_EOI     	   0xB0
#define LAPIC_RMTREAD      0xC0
#define LAPIC_LOGICDEST    0xD0
#define LAPIC_DESTFORMAT   0xE0
#define LAPIC_SPURINTVEC   0xF0
#define LAPIC_INSERVICE0   0x100
#define LAPIC_INSERVICE1   0x110
#define LAPIC_INSERVICE2   0x120
#define LAPIC_INSERVICE3   0x130
#define LAPIC_INSERVICE4   0x140
#define LAPIC_INSERVICE5   0x150
#define LAPIC_INSERVICE6   0x160
#define LAPIC_INSERVICE7   0x170
#define LAPIC_TRIGGERMODE0 0x180
#define LAPIC_TRIGGERMODE1 0x190
#define LAPIC_TRIGGERMODE2 0x1A0
#define LAPIC_TRIGGERMODE3 0x1B0
#define LAPIC_TRIGGERMODE4 0x1C0
#define LAPIC_TRIGGERMODE5 0x1D0
#define LAPIC_TRIGGERMODE6 0x1E0
#define LAPIC_TRIGGERMODE7 0x1F0
#define LAPIC_INTREQ0      0x200
#define LAPIC_INTREQ1      0x210
#define LAPIC_INTREQ2      0x220
#define LAPIC_INTREQ3      0x230
#define LAPIC_INTREQ4      0x240
#define LAPIC_INTREQ5      0x250
#define LAPIC_INTREQ6      0x260
#define LAPIC_INTREQ7      0x270
#define LAPIC_ERRSTTS      0x280
#define LAPIC_LVTCMCI      0x2F0
#define LAPIC_INTCOMMND0   0x300
#define LAPIC_INTCOMMND1   0x310
#define LAPIC_LVTTIMER     0x320
#define LAPIC_LVTSENSTHERM 0x330
#define LAPIC_LVTPERFCNT   0x340
#define LAPIC_LVTINT0      0x350
#define LAPIC_LVTINT1      0x360
#define LAPIC_LVTERR       0x370
#define LAPIC_TIMERINITCNT 0x380
#define LAPIC_TIMERCURCNT  0x390
#define LAPIC_DIVCONFIG    0x3E0

// useful register information
#define RO // read only  access
#define RW // read write access


#define ICR_DESTMODE_TYPE_PHYSICAL 0
#define ICR_DESTMODE_TYPE_LOGICAL  1
#define ICR_DELIVMODE_NORMAL 0
#define ICR_DELIVMODE_LOWEST 1
#define ICR_DELIVMODE_SMI    2
#define ICR_DELIVMODE_NMI    4
#define ICR_DELIVMODE_INIT   5
#define ICR_DELIVMODE_SIPI   6
#define ICR_INIT_ASSERT    (1 << 14)
#define ICR_INIT_DEASSERT  (1 << 15)

#define ICR_INT_PENDING (1 << 12)