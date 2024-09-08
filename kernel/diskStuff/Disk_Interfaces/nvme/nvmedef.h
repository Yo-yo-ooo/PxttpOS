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

PACK(typedef struct Registers
{
    volatile struct [[gnu::packed]]
    {
        uint64_t mqes : 16;
        uint64_t cqr : 1;
        uint64_t ams : 2;
        uint64_t rsv0 : 5;
        uint64_t to : 8;
        uint64_t dstrd : 4;
        uint64_t nssrs : 1;
        uint64_t css : 8;
        uint64_t bps : 1;
        uint64_t cps : 2;
        uint64_t mpsmin : 4;
        uint64_t mpsmax : 4;
        uint64_t pmrs : 1;
        uint64_t cmbs : 1;
        uint64_t nsss : 1;
        uint64_t crms : 2;
        uint64_t rsv1 : 3;
    } caps;
    volatile struct [[gnu::packed]]
    {
        uint32_t ter : 8;
        uint32_t mnr : 8;
        uint32_t mjr : 16;
    } version;
    uint32_t intms;
    uint32_t intmc;
    volatile struct [[gnu::packed]]
    {
        uint32_t en : 1;
        uint32_t rsv0 : 3;
        uint32_t css : 3;
        uint32_t mps : 4;
        uint32_t ams : 3;
        uint32_t shn : 2;
        uint32_t iosqes : 4;
        uint32_t iocqes : 4;
        uint32_t crime : 1;
        uint32_t rsv1 : 7;
    } ctrlconf;
    uint32_t rsv0;
    volatile struct [[gnu::packed]]
    {
        uint32_t rdy : 1;
        uint32_t cfs : 1;
        uint32_t shst : 2;
        uint32_t nssro : 1;
        uint32_t pp : 1;
        uint32_t st : 1;
        uint32_t rsv0 : 25;
    } ctrlstat;
    uint32_t nssr;
    volatile struct [[gnu::packed]]
    {
        uint32_t asqs : 12;
        uint32_t rsv0 : 4;
        uint32_t acqs : 12;
        uint32_t rsv1 : 4;
    } aqa;
    uint64_t asq;
    uint64_t acq;
    
})NvmeRegs;

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

enum AdminOpcode
{
    deleteSQ = 0x0,
    createSQ = 0x1,
    deleteCQ = 0x4,
    createCQ = 0x5,
    identify = 0x6
};