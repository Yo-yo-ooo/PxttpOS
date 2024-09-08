#include "nvmedef.h"
#include "../generic/genericDiskInterface.h"
#include "../../../devices/pci/pci.h"
#include "nvmedef.h"
#include "../../../interrupts/interrupts.h"

namespace DiskInterface
{
    class NvmeDiskInterface : public GenericDiskInterface
    {
    private:
        uint64_t nvme_base_addr;
        uint32_t nvme_cap_strd;
        NvmeQueue *sq, *cq;
        NvmeRegs *nvme_regs;

        uint32_t ReadRegister(uint32_t offset);
        void WriteRegister(uint32_t offset, uint32_t value);
        bool nvme_send_command(uint8_t opcode, uint32_t nsid, 
        void *data, uint64_t lba, uint16_t num_blocks, NvmeQueueEntry *completion);
        bool create_admin_submission_queue(NvmeQueue *sq);
        bool create_admin_completion_queue(NvmeQueue *cq);

        void HandleIRQ(interrupt_frame* frame);
    public:
        NvmeDiskInterface(PCI::PCIDeviceHeader* PCIBaseAddr);
        bool Read(uint64_t sector, uint32_t count, void *buffer);
        bool Write(uint64_t sector, uint32_t count, void *buffer);
        bool ReadBytes(uint64_t address, uint64_t count, void* buffer);
        bool WriteBytes(uint64_t address, uint64_t count, void* buffer);
    };
}