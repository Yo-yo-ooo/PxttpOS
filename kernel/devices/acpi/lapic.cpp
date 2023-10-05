#include "acpi.h"

static uintptr_t lapic_base;

inline bool lapic_pending();

void lapic_set_base(uint64_t lapic_address)
{
    lapic_base = lapic_address;
}

void pausewhile(bool condition)
{
	while(condition) {
		__asm__ volatile("pause" : : : "memory");
	}
}


inline uint32_t lapic_read_reg(uint16_t offset)
{
    return *(uint32_t volatile*)((uint8_t*)lapic_base + offset); 
}


inline void lapic_write_reg(uint16_t offset, uint32_t data)
{
    *(uint32_t volatile*)((uint8_t*)lapic_base + offset) = data;
    return;
}

void lapic_init()
{
    __asm__ volatile("cli");
    // forgot one thing - In the case that the BIOS disabled the LAPIC,
    // You need to write the lapic_base you found to the IA32_APIC_BASE_MSR (0x1B)  
    // wrmsr(IA32_MSR_ECX_APIC_BASE, lapic_base);


    uint32_t tmp = lapic_read_reg(LAPIC_SPURINTVEC) | 0x1FF;
    lapic_write_reg(LAPIC_DESTFORMAT, 0xFFFFFFFF); // set destination format to flat.
    lapic_write_reg(LAPIC_LOGICDEST ,    1 << 24);
    lapic_write_reg(LAPIC_SPURINTVEC,        tmp); // enable the lapic and set the spurious int vec to idt gate 0xff.
    lapic_write_reg(LAPIC_TASKPRI   ,       0x00); // accept all irq's by changing the task priority reg.

    __asm__ volatile("sti");
}

void lapic_send_int(uint32_t lapic_id, uint8_t vector)
{
    uint32_t tmp1 = lapic_read_reg(LAPIC_INTCOMMND1) | ((lapic_id & 0x3) << 24);
    uint32_t tmp0 = lapic_read_reg(LAPIC_INTCOMMND0) | vector;
    lapic_write_reg(LAPIC_INTCOMMND1, tmp1);
    lapic_write_reg(LAPIC_INTCOMMND0, tmp0);
    return;
}


void lapic_send_init(uint32_t lapic_id)
{    
    uint32_t icr1 = (lapic_read_reg(LAPIC_INTCOMMND1) & 0x00ffffff) | ((lapic_id & 0xFF) << 24);
    uint32_t icr0 = ( (lapic_read_reg(LAPIC_INTCOMMND0) & 0xfff00000) | (ICR_DESTMODE_TYPE_PHYSICAL << 11) ) | ( (ICR_DELIVMODE_INIT << 8) | ICR_INIT_ASSERT );
    //debug_print("icr1 %X\nicr0 %X\n\n", icr1, icr0);

    lapic_write_reg(LAPIC_INTCOMMND1, icr1); // which ap core
    lapic_write_reg(LAPIC_INTCOMMND0, icr0); // INIT IPI Assert
    pausewhile(lapic_pending());


    icr1 = (lapic_read_reg(LAPIC_INTCOMMND1) & 0x00ffffff) | ((lapic_id & 0xFF) << 24);
    icr0 = ( (lapic_read_reg(LAPIC_INTCOMMND0) & 0xfff00000) | (ICR_DESTMODE_TYPE_PHYSICAL << 11) ) | ( (ICR_DELIVMODE_INIT << 8) | ICR_INIT_DEASSERT );
    //debug_print("icr1 %X\nicr0 %X\n\n", icr1, icr0);
    
    lapic_write_reg(LAPIC_INTCOMMND1, icr1); // which ap core
    lapic_write_reg(LAPIC_INTCOMMND0, icr0); // INIT IPI De-assert
    pausewhile(lapic_pending());
    //debug_print("sub-bruh 1\n", "");
    return;
}


void lapic_send_startup(uint32_t lapic_id, uint32_t trampoline)
{
    uint32_t icr1 = (lapic_read_reg(LAPIC_INTCOMMND1) & 0x00ffffff) | ((lapic_id & 0xFF) << 24);
    uint32_t icr0 = ( (lapic_read_reg(LAPIC_INTCOMMND0) & 0xfff00000) | (trampoline >> 12ull) ) | ( (ICR_DELIVMODE_SIPI << 8) | (ICR_DESTMODE_TYPE_PHYSICAL << 11) );
    //debug_print("icr1 %X\nicr0 %X\n\n", icr1, icr0);
    
    lapic_write_reg(LAPIC_INTCOMMND1, icr1); // which ap core
    lapic_write_reg(LAPIC_INTCOMMND0, icr0); // INIT SIPI 
    return;
}


inline void lapic_send_eoi()
{
    lapic_write_reg(LAPIC_EOI, 0);
}


inline uint32_t lapic_get_id()
{
    // under xAPIC   (in my current QEMU Config), this is right. 
    // under x2APIC, the whole register is the ID.
    return lapic_read_reg(LAPIC_ID) >> 24;
}


inline bool lapic_pending()
{
    return lapic_read_reg(LAPIC_INTCOMMND0) & ICR_INT_PENDING;
}