#include "apic.h"
#include "../../paging/PageFrameAllocator.h"
#include "../../paging/PageTableManager.h"
#include "../pit/pit.h"

unsigned long physRegs;
 
/*
    * Holds the base address of the registers in virtual memory. This
    * address is non-cacheable (see paging).
    */
unsigned long virtAddr;

/*
    * Software has complete control over the apic-id. Also, hardware
    * won't automatically change its apic-id so we could cache it here.
    */
unsigned char apicId;

/*
    * Hardware-version of the apic, mainly for display purpose. ToDo: specify
    * more purposes.
    */
unsigned char apicVer;

/*
    * Although entries for current IOAPIC is 24, it may change. To retain
    * compatibility make sure you use this.
    */
unsigned char redirEntryCnt;

/*
* The first IRQ which this IOAPIC handles. This is only found in the
* IOAPIC entry of the ACPI 2.0 MADT. It isn't found in the IOAPIC
* registers.
*/
unsigned long globalIntrBase;

uint32_t read(unsigned char regOff)
{
        *(uint32_t volatile*) virtAddr = regOff;
        return *(uint32_t volatile*)(virtAddr + 0x10);
}

/*
* Writes the data into the register associated. 
*
@param regOff - the register's offset which is being written
@param data - dword to write to the register
*/
void write(unsigned char regOff, uint32_t data)
{
        *(uint32_t volatile*) virtAddr = regOff;
        *(uint32_t volatile*)(virtAddr + 0x10) = data;
}

void IOAPIC_INIT(unsigned long physRegs, unsigned long apicId, unsigned long gsib){
    virtAddr = (unsigned long)GlobalAllocator->RequestPage();
    GlobalPageTableManager.MapMemory((void*)virtAddr, physRegs,PT_Flag::ReadWrite | PT_Flag::CacheDisabled
        | PT_Flag::Present);
    virtAddr += physRegs % 4096;
    apicId = (read(IOAPICID) >> 24) & 0xF0;
    apicVer = read(IOAPICVER);// cast to uint8_t (unsigned char) hides upper bits

    //< max. redir entry is given IOAPICVER[16:24]
    redirEntryCnt = (read(IOAPICVER) >> 16) + 1;// cast to uint8_t occuring ok!
    globalIntrBase = gsib;
}

unsigned char IOAPIC_id(){ return (apicId); }
unsigned char IOAPIC_ver(){ return (apicVer); }
unsigned char IOAPIC_redirectionEntries(){ return (redirEntryCnt); }
unsigned long IOAPIC_globalInterruptBase(){ return (globalIntrBase); }

/*
* Bit of assignment here - implement this on your own. Use the lowerDword & upperDword
* fields of RedirectionEntry using
*                               write(entNo, ent->lowerDword);
*                               write(entNo, ent->upperDword);
*
* Be sure to check that entNo < redirectionEntries()
*
* @param entNo - entry no. for which redir-entry is required
* @param entry - ptr to entry to write
*/
void IOAPIC_WriteRedirEntry(unsigned char entNo, RedirectionEntry *entry){
    if(entNo >= redirEntryCnt){
        return;
    }else{
        write(IOAPICREDTBL(entNo), entry->lowerDword);
        write(IOAPICREDTBL(entNo) + 1, entry->upperDword);
    }
}

/*
* Bit of assignment here - implement this on your own. Use the lowerDword & upperDword
* fields of RedirectionEntry using
*                                 ent.lowerDword = read(entNo);
*                                 ent.upperDword = read(entNo);
*                                 return (ent);
*
* Be sure to check that entNo < redirectionEntries()
*
* @param entNo - entry no. for which redir-entry is required
* @return entry associated with entry no.
*/
RedirectionEntry IOAPIC_GetRedirEntry(unsigned char entNo){
    RedirectionEntry ent;
    if(entNo >= redirEntryCnt){
        ent.lowerDword = 0;
        ent.upperDword = 0;
        return ent;
    }else{
        ent.lowerDword = read(IOAPICREDTBL(entNo));
        ent.upperDword = read(IOAPICREDTBL(entNo) + 1);
        return (ent);
    }
}

uint32_t CpuReadIoApic(void *ioapicaddr, uint32_t reg)
{
    uint32_t volatile *ioapic = (uint32_t volatile *)ioapicaddr;
    ioapic[0] = (reg & 0xff);
    return ioapic[4];
}
 
void CpuWriteIoApic(void *ioapicaddr, uint32_t reg, uint32_t value)
{
    uint32_t volatile *ioapic = (uint32_t volatile *)ioapicaddr;
    ioapic[0] = (reg & 0xff);
    ioapic[4] = value;
}

void start_apic_timer(){
    write(APIC_REGISTER_TIMER_DIV,0x3);
    PIT::Sleep(10);
    write(APIC_REGISTER_TIMER_INITCNT, 0xFFFFFFFF);

    write(APIC_REGISTER_LVT_TIMER, APIC_LVT_INT_MASKED);
 
    // Now we know how often the APIC timer has ticked in 10ms
    uint32_t ticksIn10ms = 0xFFFFFFFF - read(APIC_REGISTER_TIMER_CURRCNT);

    // Start timer as periodic on IRQ 0, divider 16, with the number of ticks we counted
    write(APIC_REGISTER_LVT_TIMER, 32 | APIC_LVT_TIMER_MODE_PERIODIC);
    write(APIC_REGISTER_TIMER_DIV, 0x3);
    write(APIC_REGISTER_TIMER_INITCNT, ticksIn10ms);
}