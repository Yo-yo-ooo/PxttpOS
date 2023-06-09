#ifndef _X86_IO_H
#define _X86_IO_H

unsigned char ioport_in8(unsigned int port);
unsigned short ioport_in16(unsigned int port);
unsigned int ioport_in32(unsigned int port);
void ioport_out8(unsigned int port, unsigned int data);
void ioport_out16(unsigned int port, unsigned int data);
void ioport_out32(unsigned int port, unsigned int data);

void ioport_read_bytes(unsigned short port, void* buf, unsigned int n);
void ioport_write_bytes(unsigned short port, void* buf, unsigned int n);

#define in8         ioport_in8
#define in16        ioport_in16
#define in32        ioport_in32

#define out8        ioport_out8
#define out16       ioport_out16
#define out32       ioport_out32

#define io_read     ioport_read_bytes
#define io_write    ioport_write_bytes

static inline void outportw(unsigned short _port, unsigned short _data)
{
    __asm__ volatile ("cli");
    __asm__ volatile ("outw %1, %0"::"dN"(_port), "a"(_data));
    __asm__ volatile ("sti");
}

static inline unsigned short inportw(unsigned short _port)
{
	unsigned short rv;
    __asm__ volatile ("cli");
    __asm__ volatile ("inw %1, %0":"=a"(rv):"dN"(_port));
    __asm__ volatile ("sti");

    return rv;
}

#endif  /* _X86_IO_H */
