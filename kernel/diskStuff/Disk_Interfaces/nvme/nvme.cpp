#include "nvme.h"
#include "../../../memory/heap.h"
#include "../../../paging/PageTableManager.h"
#include "../../../kernelStuff/stuff/stackmacro.h"
#include "../../../devices/pit/pit.h"
//see https://wiki.osdev.org/NVMe
#define QUEUE_SIZE 4096
#define NVME_SUCCESS 1


static uint64_t submission_queue_tail;
static uint64_t completion_queue_head;

namespace DiskInterface{

uint32_t NvmeDiskInterface::ReadRegister(uint32_t offset){
    volatile uint32_t *nvme_reg = (volatile uint32_t *)(nvme_base_addr + offset);
	//map_page((uint64_t)nvme_reg);
    GlobalPageTableManager.MapMemory((uint64_t)nvme_reg,(uint64_t)nvme_reg);
	return *nvme_reg;
}

void NvmeDiskInterface::WriteRegister(uint32_t offset, uint32_t value){
    volatile uint32_t *nvme_reg = (volatile uint32_t *)(nvme_base_addr + offset);
	//map_page((uint64_t)nvme_reg);
    GlobalPageTableManager.MapMemory((uint64_t)nvme_reg,(uint64_t)nvme_reg);
	*nvme_reg = value;
}

bool NvmeDiskInterface::create_admin_submission_queue(NvmeQueue *sq) {
	sq->address = (uint64_t)_Malloc1(4096);
	if (sq->address == 0)
		return false;
	sq->size = 63;
	// 0x28 is the Admin Submission queue register
	WriteRegister(0x28, sq->address);
	return true;
}

bool NvmeDiskInterface::create_admin_completion_queue(NvmeQueue *cq) {
	cq->address = (uint64_t)_Malloc1(4096);
	if (cq->address == 0)
		return false;
	cq->size = 63;
	// 0x30 is the Admin Completion queue register
	WriteRegister(0x30, cq->address);
	return true;
}

static bool toggle(NvmeRegs*p,bool enable){
    uint32_t value = enable;
    p->ctrlconf.en = value;

    size_t start = PIT::TimeSinceBootMS();
    size_t end = p->caps.to * 500;

    while (start < end)
    {
        if (p->ctrlstat.rdy == value)
            break;

        asm volatile ("" : : : "memory");
        start++;
    }

    return p->ctrlstat.cfs == 0 && p->ctrlstat.rdy == value;
}

void NvmeDiskInterface::HandleIRQ(interrupt_frame* frame){

}

NvmeDiskInterface::NvmeDiskInterface(PCI::PCIDeviceHeader* PCIBaseAddr){
#define bar0 ((PCI::PCIHeader0*)PCIBaseAddr)->BAR0
#define bar1 ((PCI::PCIHeader0*)PCIBaseAddr)->BAR1
    AddToStack();

    this->nvme_base_addr = (uint64_t)(((uint64_t)bar1 << 32) | (bar0 & 0xFFFFFFF0));
    this->nvme_cap_strd = (nvme_base_addr >> 12) & 0xF;

    nvme_regs = (NvmeRegs *)nvme_base_addr;
    PrintfMsg("NVMe Version: %d.%d.%d", 
    nvme_regs->version.mjr, 
    nvme_regs->version.mnr, 
    nvme_regs->version.ter);

    auto css = nvme_regs->caps.css;
    if ((css & 0x01) == 0)
        PrintMsg("NVM command set not supported");

    if (toggle(nvme_regs ,false) == false)
        PrintMsg("Could not disable controller");

    auto queue_size = nvme_regs->caps.mqes;
    {
        uint8_t irqId = PCI::io_read_byte(PCIBaseAddr, PCI_INTERRUPT_LINE);
        PrintfMsg("> Nvme IRQ: %d", irqId);
        {
            IRQHandlerCallbackFuncs[irqId] = (void*)&NvmeDiskInterface::HandleIRQ;
            IRQHandlerCallbackHelpers[irqId] = (void*)this;
        }
    }

    nvme_regs->aqa.asqs = queue_size - 1;
    nvme_regs->aqa.acqs = queue_size - 1;

    if(this->create_admin_completion_queue(cq) == false ||
       this->create_admin_submission_queue(sq) == false){
        PrintMsg("Failed to create admin queues\n");
        
    }

    nvme_regs->asq = sq->address;
    nvme_regs->acq = cq->address;

    nvme_regs->ctrlconf.css = 0b000; // NVM command set
    nvme_regs->ctrlconf.ams = 0b000; // Round robin
    nvme_regs->ctrlconf.iosqes = 6;
    nvme_regs->ctrlconf.iocqes = 4;

    if (toggle(nvme_regs,true) == false)
        PrintMsg("Could not enable controller");

    NvmeQueueEntry *completion = NULL;
    if(nvme_send_command(AdminOpcode::identify,0,0,0,0,completion) == NVME_SUCCESS)
        PrintMsg("Failed to send identify command");
    
    
    completion_queue_head = (uint64_t)cq;
    submission_queue_tail = (uint64_t)sq + 0x1000;

#undef bar0
#undef bar1
    RemoveFromStack();
}

bool NvmeDiskInterface::nvme_send_command(uint8_t opcode, uint32_t nsid, void *data, uint64_t lba, uint16_t num_blocks, NvmeQueueEntry *completion) {
#define submission_queue sq
#define completion_queue cq

    uint64_t sq_entry_addr = submission_queue->address + (submission_queue_tail * sizeof(NvmeCmd));
	uint64_t cq_entry_addr = completion_queue->address + (completion_queue_head * sizeof(NvmeQueueEntry));
	NvmeCmd command_entry;
	command_entry.opcode = opcode;
	command_entry.nsid = nsid;
	command_entry.prp1 = (uintptr_t)data;
	command_entry.prp2 = 0;
	command_entry.command_specific[0] = (uint32_t)lba;
	command_entry.command_specific[1] = (uint32_t)((uint64_t)lba >> 32);
	command_entry.command_specific[2] = (uint16_t)(num_blocks - 1);
	_memcpy((void *)sq_entry_addr, &command_entry, sizeof(NvmeCmd));
	submission_queue_tail++;
	WriteRegister(0x1000 + 2 * (4 << nvme_cap_strd), submission_queue_tail);
	if (submission_queue_tail == QUEUE_SIZE)
		submission_queue_tail = 0;
	// You should wait for completion here
	GlobalPageTableManager.MapMemory(cq_entry_addr,cq_entry_addr);
	completion = (NvmeQueueEntry *)cq_entry_addr;
	completion_queue_head++;
	WriteRegister(0x1000 + 3 * (4 << nvme_cap_strd), completion_queue_head);
	if (completion_queue_head == QUEUE_SIZE)
		completion_queue_head = 0;
	return (bool)completion->status != 0;

#undef submission_queue
#undef completion_queue
}

bool NvmeDiskInterface::Read(uint64_t sector, uint32_t count, void *buffer) {
	NvmeQueueEntry *completion = NULL;
	if (nvme_send_command(0x02, 0, buffer, sector, count, completion) != NVME_SUCCESS)
	    return true;
	if (completion->status != NVME_SUCCESS)
		return true;
	return false;
}

bool NvmeDiskInterface::Write(uint64_t sector, uint32_t count, void *buffer) {
	NvmeQueueEntry *completion = NULL;
	if (nvme_send_command(0x01, 0, buffer, sector, count, completion) != NVME_SUCCESS)
	    return true;
	if (completion->status != NVME_SUCCESS)
		return true;
	return false;
}

#define SectorCount 0x1000000

bool NvmeDiskInterface::ReadBytes(uint64_t address, uint64_t count, void* buffer)
{

    if (count == 0)
        return true;
    if (address + count > SectorCount * 512)
        return false;
    AddToStack();
    uint32_t tempSectorCount = ((((address + count) + 511) / 512) - (address / 512));
    uint8_t* buffer2 = (uint8_t*)_Malloc(tempSectorCount * 512, "Malloc for Read Buffer");
    _memset(buffer2, 0, tempSectorCount * 512);

    if (!Read((address / 512), tempSectorCount, buffer2))
    {
        uint16_t offset = address % 512;
        for (uint64_t i = 0; i < count; i++)
            ((uint8_t*)buffer)[i] = buffer2[i + offset];

        _Free(buffer2);
        RemoveFromStack();
        return false;
    }

    uint16_t offset = address % 512;
    for (uint64_t i = 0; i < count; i++)
        ((uint8_t*)buffer)[i] = buffer2[i + offset];
            
    _Free(buffer2);
    RemoveFromStack();
    return true;
}

bool NvmeDiskInterface::WriteBytes(uint64_t address, uint64_t count, void* buffer)
{
    //Window* window = osData.mainTerminalWindow;
    
    if (count == 0)
        return true;
    if (address + count > SectorCount * 512)
        return false;
    AddToStack();
    uint32_t tempSectorCount = ((((address + count) + 511) / 512) - (address / 512));
    uint8_t* buffer2 = (uint8_t*)_Malloc(512, "Malloc for Write Buffer");
    //window->Log("Writing Bytes...");
    
    if (tempSectorCount == 1)
    {
        //window->Log("Writing Bytes (1/1)");
        //uint8_t* buffer2 = (uint8_t*)malloc(512, "Malloc for Read Buffer (1/1)");
        _memset(buffer2, 0, 512);
        if (!Read((address / 512), 1, buffer2))
        {
            _Free(buffer2);
            RemoveFromStack();
            return false;
        }

        uint16_t offset = address % 512;
        for (uint64_t i = 0; i < count; i++)
            buffer2[i + offset] = ((uint8_t*)buffer)[i];

        if (!Write((address / 512), 1, buffer2))
        {
            _Free(buffer2);
            RemoveFromStack();
            return false;
        }
        
        //free(buffer2);
    }
    else
    {
        //window->Log("Writing Bytes (1/3)");
        uint64_t newAddr = address;
        uint64_t newCount = count;
        uint64_t addrOffset = 0;
        {
            // GlobalRenderer->Clear(Colors.dgray);
            // while (true);


            //uint8_t* buffer2 = (uint8_t*)malloc(512, "Malloc for Read Buffer (1/2)");
            _memset(buffer2, 0, 512);
            //window->Log("Writing to Sector: {}", to_string((address / 512)), Colors.yellow);
            if (!Read((address / 512), 1, buffer2))
            {
                _Free(buffer2);
                RemoveFromStack();
                return false;
            }

            uint16_t offset = address % 512;
            uint16_t specialCount = 512 - offset;
            addrOffset = specialCount;
            newAddr = address + specialCount;
            newCount = count - specialCount;

            // window->Log("Address:        {}", to_string(address), Colors.yellow);
            // window->Log("Count:          {}", to_string(count), Colors.yellow);
            // window->Log("New Address:    {}", to_string(newAddr), Colors.yellow);
            // window->Log("New Count:      {}", to_string(newCount), Colors.yellow);
            // window->Log("Address Offset: {}", to_string(addrOffset), Colors.yellow);

            for (uint64_t i = 0; i < specialCount; i++)
                buffer2[i + offset] = ((uint8_t*)buffer)[i];

            if (!Write((address / 512), 1, buffer2))
            {
                _Free(buffer2);
                RemoveFromStack();
                return false;
            }
            
            //free(buffer2);
        }

        //window->Log("Writing Bytes (2/3)");
        {
            //uint8_t* buffer2 = (uint8_t*)malloc(512, "Malloc for Read Buffer (1/2)");
            _memset(buffer2, 0, 512);
            //window->Log("Writing to Sector: {}", to_string((address + count - 1) / 512), Colors.yellow);
            if (!Read(((address + count) / 512), 1, buffer2))
            {
                _Free(buffer2);
                RemoveFromStack();
                return false;
            }

            uint16_t specialCount = ((address + count) % 512);
            //uint64_t offset = (address + count - specialCount);
            newCount -= specialCount;
            //newAddr = address + specialCount;

            // GlobalRenderer->Clear(Colors.black);

            // GlobalRenderer->Println("Address:        {}", to_string(address), Colors.yellow);
            // GlobalRenderer->Println("Count:          {}", to_string(count), Colors.yellow);
            // GlobalRenderer->Println("New Address:    {}", to_string(newAddr), Colors.yellow);
            // GlobalRenderer->Println("New Count:      {}", to_string(newCount), Colors.yellow);
            // //window->Log("Offset:         {}", to_string(offset), Colors.yellow);
            // GlobalRenderer->Println("Address Offset: {}", to_string(addrOffset), Colors.yellow);
            // GlobalRenderer->Println("Special Count:  {}", to_string(specialCount), Colors.yellow);

            uint64_t blehus = (count - specialCount);
            //GlobalRenderer->Println("blehus:  {}", to_string(blehus), Colors.yellow);

            // GlobalRenderer->Println("BUFF (1):");
            // for (int i = 0; i < 512; i++)
            //     GlobalRenderer->Print("{} ", to_string((int)buffer2[i]), Colors.yellow);
            // GlobalRenderer->Println("");

            for (int64_t i = 0; i < specialCount; i++)
                buffer2[i] = ((uint8_t*)buffer)[i + blehus];

            // GlobalRenderer->Println("BUFF (2):");
            // for (int i = 0; i < 512; i++)
            //     GlobalRenderer->Print("{} ", to_string((int)buffer2[i]), Colors.yellow);
            // GlobalRenderer->Println("");

            //while (true);

            if (!Write(((address + count) / 512), 1, buffer2))
            {
                _Free(buffer2);
                RemoveFromStack();
                return false;
            }
            
            //free(buffer2);
        }
        
        //window->Log("Writing Bytes (3/3)");
        {
            uint64_t newSectorCount = newCount / 512;
            if (newSectorCount != 0)
            {
                uint64_t newSectorStartId = newAddr / 512;

                // window->Log("newSectorCount:    {}", to_string(newSectorCount), Colors.yellow);
                // window->Log("newSectorStartID:  {}", to_string(newSectorStartId), Colors.yellow);
                // window->Log("Address Offset:    {}", to_string(addrOffset), Colors.yellow);

                if (!Write(newSectorStartId, newSectorCount, (void*)((uint64_t)buffer + addrOffset)))
                {
                    _Free(buffer2);
                    RemoveFromStack();
                    return false;
                }
            }
        }


        
    }
    _Free(buffer2);


    // free(buffer2);
    RemoveFromStack();
    return true;
}
#undef SectorCount
}