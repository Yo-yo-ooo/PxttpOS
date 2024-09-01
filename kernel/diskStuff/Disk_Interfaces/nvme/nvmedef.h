#pragma once
#include "../../../fsStuff/packdef.h"
#include <stdint.h>
#include <stddef.h>

PACK(typedef struct NvmeCommand
{
    uint8_t opcode;
    uint8_t fused : 2;
    uint8_t rsvd : 4;
    uint8_t PRP_or_SGL : 2;
    uint16_t CmdId;

    uint32_t nsid;
    uint64_t rsvd_;
    uint64_t MPTR;
    uint64_t prp1;
    uint64_t prp2;
    uint64_t command_specific[3];
})NvmeCmd;

PACK(typedef struct nvme_completion_queue_entry
{
    uint32_t CmdSpecific;
    uint32_t rsvd;
    uint16_t SQHeadPointer;
    uint16_t SQID;
    uint16_t CmdID;
    uint8_t  phase : 1;
    uint16_t status : 15;
})NvmeQueueEntry;

PACK(typedef struct nvme_queue_{
    uint64_t address;
    uint64_t size;
})NvmeQueue;

PACK(typedef struct NvmeVerisonReg{
    uint32_t ter : 8;
    uint32_t mnr : 8;
    uint32_t mjr : 16;
})NvmeVReg;

enum nvme_opcode{
    nvme_cmd_flush          = 0x00,
    nvme_cmd_write          = 0x01,
    nvme_cmd_read           = 0x02,
    nvme_cmd_write_uncor    = 0x04,
    nvme_cmd_compare        = 0x05,
    nvme_cmd_write_zeroes   = 0x08,
    nvme_cmd_dsm            = 0x09,
    nvme_cmd_resv_register  = 0x0d,
    nvme_cmd_resv_report    = 0x0e,
    nvme_cmd_resv_acquire   = 0x11,
    nvme_cmd_resv_release   = 0x15,
};