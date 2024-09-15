#if defined (BIOS)

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <sys/a20.h>
#include <sys/cpu.h>
#include <lib/real.h>

bool a20_check(void) {
    bool ret = false;
    uint16_t orig = mminw(0x7dfe);

    mmoutw(0x7dfe, 0x1234);

    if (mminw(0x7dfe) != mminw(0x7dfe + 0x100000)) {
        ret = true;
        goto out;
    }

    mmoutw(0x7dfe, ~mminw(0x7dfe));

    if (mminw(0x7dfe) != mminw(0x7dfe + 0x100000)) {
        ret = true;
        goto out;
    }

out:
    mmoutw(0x7dfe, orig);
    return ret;
}

// Keyboard controller method code below taken from:
// https://wiki.osdev.org/A20_Line

bool a20_enable(void) {
    if (a20_check())
        return true;

    // BIOS method
    struct rm_regs r = {0};
    r.eax = 0x2401;
    rm_int(0x15, &r, &r);

    if (a20_check())
        return true;

    // Keyboard controller method
    while (inb(0x64) & 2);
    outb(0x64, 0xad);
    while (inb(0x64) & 2);
    outb(0x64, 0xd0);
    while (!(inb(0x64) & 1));
    uint8_t b = inb(0x60);
    while (inb(0x64) & 2);
    outb(0x64, 0xd1);
    while (inb(0x64) & 2);
    outb(0x60, b | 2);
    while (inb(0x64) & 2);
    outb(0x64, 0xae);
    while (inb(0x64) & 2);

    if (a20_check())
        return true;

    // Fast A20 method
    b = inb(0x92);
    if ((b & 0x02) == 0) {
        b &= ~0x01;
        b |= 0x02;
        outb(0x92, b);
    }

    if (a20_check())
        return true;

    return false;
}

#endif
