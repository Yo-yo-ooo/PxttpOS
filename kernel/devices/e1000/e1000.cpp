#include "e1000.h"
#include "../../interrupts/panic.h"
#include "../../paging/PageTableManager.h"
#include "../serial/serial.h"
#include "../pci/pci.h"
#include "../../kernelStuff/IO/IO.h"
#include "../../osData/MStack/MStackM.h"
#include <libm/stubs.h>

namespace MMIOUtils{

uint8_t read8 (uint64_t p_address)
{
    return *((volatile uint8_t*)(p_address));
}
uint16_t read16 (uint64_t p_address)
{
    return *((volatile uint16_t*)(p_address));
    
}
uint32_t read32 (uint64_t p_address)
{
    return *((volatile uint32_t*)(p_address));
    
}
uint64_t read64 (uint64_t p_address)
{
    return *((volatile uint64_t*)(p_address));    
}
void write8 (uint64_t p_address,uint8_t p_value)
{
    (*((volatile uint8_t*)(p_address)))=(p_value);
}
void write16 (uint64_t p_address,uint16_t p_value)
{
    (*((volatile uint16_t*)(p_address)))=(p_value);    
}
void write32 (uint64_t p_address,uint32_t p_value)
{
    (*((volatile uint32_t*)(p_address)))=(p_value);
    
}
void write64 (uint64_t p_address,uint64_t p_value)
{
    (*((volatile uint64_t*)(p_address)))=(p_value);    
}
}

namespace Ports{
void outportb (uint16_t p_port,uint8_t p_data)
{
    asm volatile ("outb %1, %0" : : "dN" (p_port), "a" (p_data));
}

/* void Ports::outportw (uint16_t p_port,uint16_t p_data)
 * 
 * This method outputs a word to a hardware port.
 * 
 *  p_port: the port number to output the byte p_data to.
 *  p_data: the byte to to output to the port p_port.
 * 
 */


void outportw (uint16_t p_port,uint16_t p_data)
{
    asm volatile ("outw %1, %0" : : "dN" (p_port), "a" (p_data));
}

/* void Ports::outportl (uint16_t p_port,uint32_t p_data)
 * 
 * This method outputs a double word to a hardware port.
 * 
 *  p_port: the port number to output the byte p_data to.
 *  p_data: the byte to to output to the port p_port.
 * 
 */


void outportl (uint16_t p_port,uint32_t p_data)
{
    asm volatile ("outl %1, %0" : : "dN" (p_port), "a" (p_data));
}

/* uint8_t Ports::inportb( uint16_t p_port)
 * 
 * This method reads a byte from a hardware port.
 * 
 *  p_port: the port number to read the byte from.
 *  return value : a byte read from the port p_port.
 * 
 * Notice the output constraint "=a", this tells the compiler 
 * to expect the save the value of register AX into the variable l_ret
 * The register AX should contain the result of the inb instruction.
 * 
 * 
 */

uint8_t inportb( uint16_t p_port)
{
    uint8_t l_ret;
    asm volatile("inb %1, %0" : "=a" (l_ret) : "dN" (p_port));
    return l_ret;
}

/* uint16_t Ports::inportw( uint16_t p_port)
 * 
 * This method reads a word from a hardware port.
 * 
 *  p_port: the port number to read the word from.
 *  return value : a word read from the port p_port.
 * 
 */


uint16_t inportw( uint16_t p_port)
{
    uint16_t l_ret;
    asm volatile ("inw %1, %0" : "=a" (l_ret) : "dN" (p_port));
    return l_ret;
}


/* uint16_t Ports::inportl( uint16_t p_port)
 * 
 * This method reads a double word from a hardware port.
 * 
 *  p_port: the port number to read the double word from.
 *  return value : a double word read from the port p_port.
 * 
 */

uint32_t inportl( uint16_t p_port)
{
    uint32_t l_ret;
    asm volatile ("inl %1, %0" : "=a" (l_ret) : "dN" (p_port));
    return l_ret;
}
}

void E1000::writeCommand( uint16_t p_address, uint32_t p_value)
{
    if ( bar_type == PCI::PCI_BAR_TYPE_ENUM::MMIO32 || bar_type == PCI::PCI_BAR_TYPE_ENUM::MMIO64 )
        MMIOUtils::write32(mem_base+p_address,p_value);
    else if(bar_type == PCI::PCI_BAR_TYPE_ENUM::IO)
    {
        Ports::outportl(io_base, p_address);
        Ports::outportl(io_base + 4, p_value);
    }else{return;}
}
uint32_t E1000::readCommand( uint16_t p_address)
{
    if ( bar_type == PCI::PCI_BAR_TYPE_ENUM::MMIO32 || bar_type == PCI::PCI_BAR_TYPE_ENUM::MMIO64 )
        return MMIOUtils::read32(mem_base+p_address);
    else if (bar_type == PCI::PCI_BAR_TYPE_ENUM::IO)
    {
        Ports::outportl(io_base, p_address);
        return Ports::inportl(io_base + 4);
    }else{return 0;}
}

bool E1000::detectEEProm()
{
    uint32_t val = 0;
    writeCommand(REG_EEPROM, 0x1); 

    for(int i = 0; i < 1000 && ! eerprom_exists; i++)
    {
            val = readCommand( REG_EEPROM);
            if(val & 0x10)
                    eerprom_exists = true;
            else
                    eerprom_exists = false;
    }
    return eerprom_exists;
}

uint32_t E1000::eepromRead( uint8_t addr)
{
	uint16_t data = 0;
	uint32_t tmp = 0;
    if ( eerprom_exists)
    {
            writeCommand( REG_EEPROM, (1) | ((uint32_t)(addr) << 8) );
        while( !((tmp = readCommand(REG_EEPROM)) & (1 << 4)) );
    }
    else
    {
        writeCommand( REG_EEPROM, (1) | ((uint32_t)(addr) << 2) );
        while( !((tmp = readCommand(REG_EEPROM)) & (1 << 1)) );
    }
	data = (uint16_t)((tmp >> 16) & 0xFFFF);
	return data;
}

bool E1000::readMACAddress()
{
    if ( eerprom_exists)
    {
        uint32_t temp;
        temp = eepromRead( 0);
        mac[0] = temp &0xff;
        mac[1] = temp >> 8;
        temp = eepromRead( 1);
        mac[2] = temp &0xff;
        mac[3] = temp >> 8;
        temp = eepromRead( 2);
        mac[4] = temp &0xff;
        mac[5] = temp >> 8;
    }
    else
    {
        uint8_t * mem_base_mac_8 = (uint8_t *) (mem_base+0x5400);
        uint32_t * mem_base_mac_32 = (uint32_t *) (mem_base+0x5400);
        if ( mem_base_mac_32[0] != 0 )
        {
            for(int i = 0; i < 6; i++)
            {
                mac[i] = mem_base_mac_8[i];
            }
        }
        else return false;
    }
    return true;
}

void E1000::rxinit()
{
    uint8_t * ptr;
    struct e1000_rx_desc *descs;

    // Allocate buffer for receive descriptors. For simplicity, in my case khmalloc returns a virtual address that is identical to it physical mapped address.
    // In your case you should handle virtual and physical addresses as the addresses passed to the NIC should be physical ones
 
    ptr = (uint8_t *)_Malloc1(sizeof(struct e1000_rx_desc)*E1000_NUM_RX_DESC + 16);

    descs = (struct e1000_rx_desc *)ptr;
    for(int i = 0; i < E1000_NUM_RX_DESC; i++)
    {
        rx_descs[i] = (struct e1000_rx_desc *)((uint8_t *)descs + i*16);
        rx_descs[i]->addr = (uint64_t)(uint8_t *)_Malloc1(8192 + 16);
        rx_descs[i]->status = 0;
    }

    writeCommand(REG_TXDESCLO, (uint32_t)((uint64_t)ptr >> 32) );
    writeCommand(REG_TXDESCHI, (uint32_t)((uint64_t)ptr & 0xFFFFFFFF));

    writeCommand(REG_RXDESCLO, (uint64_t)ptr);
    writeCommand(REG_RXDESCHI, 0);

    writeCommand(REG_RXDESCLEN, E1000_NUM_RX_DESC * 16);

    writeCommand(REG_RXDESCHEAD, 0);
    writeCommand(REG_RXDESCTAIL, E1000_NUM_RX_DESC-1);
    rx_cur = 0;
    writeCommand(REG_RCTRL, RCTL_EN| RCTL_SBP| RCTL_UPE | RCTL_MPE | RCTL_LBM_NONE | RTCL_RDMTS_HALF | RCTL_BAM | RCTL_SECRC  | RCTL_BSIZE_8192);
    
}


void E1000::txinit()
{    
    uint8_t *  ptr;
    struct e1000_tx_desc *descs;
    // Allocate buffer for receive descriptors. For simplicity, in my case khmalloc returns a virtual address that is identical to it physical mapped address.
    // In your case you should handle virtual and physical addresses as the addresses passed to the NIC should be physical ones
    ptr = (uint8_t *)_Malloc1(sizeof(struct e1000_tx_desc)*E1000_NUM_TX_DESC + 16);

    descs = (struct e1000_tx_desc *)ptr;
    for(int i = 0; i < E1000_NUM_TX_DESC; i++)
    {
        tx_descs[i] = (struct e1000_tx_desc *)((uint8_t*)descs + i*16);
        tx_descs[i]->addr = 0;
        tx_descs[i]->cmd = 0;
        tx_descs[i]->status = TSTA_DD;
    }

    writeCommand(REG_TXDESCHI, (uint32_t)((uint64_t)ptr >> 32) );
    writeCommand(REG_TXDESCLO, (uint32_t)((uint64_t)ptr & 0xFFFFFFFF));


    //now setup total length of descriptors
    writeCommand(REG_TXDESCLEN, E1000_NUM_TX_DESC * 16);


    //setup numbers
    writeCommand( REG_TXDESCHEAD, 0);
    writeCommand( REG_TXDESCTAIL, 0);
    tx_cur = 0;
    writeCommand(REG_TCTRL,  TCTL_EN
        | TCTL_PSP
        | (15 << TCTL_CT_SHIFT)
        | (64 << TCTL_COLD_SHIFT)
        | TCTL_RTLC);

    // This line of code overrides the one before it but I left both to highlight that the previous one works with e1000 cards, but for the e1000e cards 
    // you should set the TCTRL register as follows. For detailed description of each bit, please refer to the Intel Manual.
    // In the case of I217 and 82577LM packets will not be sent if the TCTRL is not configured using the following bits.
    writeCommand(REG_TCTRL,  0b0110000000000111111000011111010);
    writeCommand(REG_TIPG,  0x0060200A);

}

void E1000::enableInterrupt()
{
    //PCI::enable_interrupt((uint64_t)pciConfigHeader);
}

E1000::E1000(PCI::PCIDeviceHeader * p_pciConfigHeader)
{
    AddToStack();
    DriverBaseAddress = (uint64_t)p_pciConfigHeader;
    uint64_t address = DriverBaseAddress;
    PrintMsgStartLayer("e1000 driver");
    PrintMsg("> Enabled PCI IO SPACE");
    PCI::enable_io_space(address);
    io_wait(500);
    PrintMsg("> Enabled PCI MEM SPACE");
    PCI::enable_mem_space(address);
    io_wait(500);

    //Enable bus mastering and interrupts
    io_wait(500);
    PCI::enable_interrupt(address);
    PrintMsg("> Enabled PCI Interrupts");
    io_wait(500);
    PCI::enable_bus_mastering(address);
    PrintMsg("> Enabled PCI Bus Mastering");
    Println();
    if (((PCI::PCIHeader0*)address)->BAR1 == 0)
    {
        PrintMsg("> e1000 Driver only uses BAR0 and is not supported");
        PrintMsgEndLayer("e1000 driver");
        return;
    }        
    // Get BAR0 type, io_base address and MMIO base address
    PCI::PCI_BAR_TYPE BufBarType = PCI::pci_get_bar((PCI::PCIHeader0*)address, 0);
    //m_mixer_type = PCI::pci_get_bar((PCI::PCIHeader0*)address, 0);

    //PrintMsgCol("> Mixer Address: {}", ConvertHexToString(m_mixer_address), Colors.yellow);
    //PrintMsgCol("> Mixer Address (2): {}", ConvertHexToString(((PCI::PCIHeader0*)address)->BAR0), Colors.yellow);
    if (BufBarType.type == PCI::PCI_BAR_TYPE_ENUM::MMIO64)
    {
        PrintMsgCol("> BUS TYPE USING BAR2", Colors.orange);
        BufferBarType = PCI::pci_get_bar((PCI::PCIHeader0*)address, 2);
    }
    else
    {
        PrintMsgCol("> BUS TYPE USING BAR1", Colors.orange);
        BufferBarType = PCI::pci_get_bar((PCI::PCIHeader0*)address, 1);
    }
    bar_type = BufferBarType.type;
    io_base = BufferBarType.io_address;
    mem_base = BufferBarType.mem_address;
    PrintMsg("> Successfully read BAR0 type, io_base address and MMIO base address!");
    PrintfMsg("> BAR0 type: %d", bar_type);
    // Off course you will need here to map the memory address into you page tables and use corresponding virtual addresses

    // Enable bus mastering
    //PCI::enable_bus_mastering((uint64_t)address);
    BufIrqId = PCI::io_read_byte(address, PCI_INTERRUPT_LINE);
    PrintfMsg("> e1000 IRQ: %d", BufIrqId);
    {
        IRQHandlerCallbackFuncs[BufIrqId] = (void*)&E1000::fire;
        IRQHandlerCallbackHelpers[BufIrqId] = (void*)this;
    }

    PrintMsg("> IRQ Handler was seted");
    E1000::start();

    PrintMsgEndLayer("e1000 driver");
    RemoveFromStack();
}

void E1000::start(){
    detectEEProm ();
    PrintMsg("> Detected EEProm");
    if (! readMACAddress()) PrintMsg("> Read MAC address error");
     for(int i = 0; i < 0x80; i++)
        writeCommand(0x5200 + i*4, 0);
    PrintMsg("> OK-1");
    //PCI::enable_interrupt((uint64_t)address);
    rxinit();
    txinit();        
    PrintfMsgCol("E1000 card started",Colors.green);
}
void E1000::startLink (){}

void E1000::fire (interrupt_frame* frame)
{
    if(frame->interrupt_number == BufIrqId){
        writeCommand(REG_IMASK, 0x1);
        
        uint32_t status = readCommand(0xc0);
        if(status & 0x04)
        {
            startLink();
        }
        else if(status & 0x10)
        {
            // good threshold
        }
        else if(status & 0x80)
        {
            handleReceive();
        }
    }
}

void E1000::handleReceive()
{
    uint16_t old_cur;
    bool got_packet = false;
 
    while((rx_descs[rx_cur]->status & 0x1))
    {
            got_packet = true;
            uint8_t *buf = (uint8_t *)rx_descs[rx_cur]->addr;
            uint16_t len = rx_descs[rx_cur]->length;

            // Here you should inject the received packet into your network stack


            rx_descs[rx_cur]->status = 0;
            old_cur = rx_cur;
            rx_cur = (rx_cur + 1) % E1000_NUM_RX_DESC;
            writeCommand(REG_RXDESCTAIL, old_cur );
    }    
}

int E1000::sendPacket(const void * p_data, uint16_t p_len)
{    
    tx_descs[tx_cur]->addr = (uint64_t)p_data;
    tx_descs[tx_cur]->length = p_len;
    tx_descs[tx_cur]->cmd = CMD_EOP | CMD_IFCS | CMD_RS;
    tx_descs[tx_cur]->status = 0;
    uint8_t old_cur = tx_cur;
    tx_cur = (tx_cur + 1) % E1000_NUM_TX_DESC;
    writeCommand(REG_TXDESCTAIL, tx_cur);   
    while(!(tx_descs[old_cur]->status & 0xff));    
    return 0;
}