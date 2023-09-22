#include "ahci.h"
#include <stdint.h>
#include "../../rendering/BasicRenderer.h"
#include "../../paging/PageFrameAllocator.h"
#include "../../paging/PageTableManager.h"
#include "../../interrupts/panic.h"

#define Memset(a,b,c) _memset(a,b,c)
namespace AHCI{

    #define HBA_PORT_DEV_PRESENT 0x3
    #define HBA_PORT_IPM_ACTIVE 0x1
    #define SATA_SIG_ATAPI 0xEB140101
    #define SATA_SIG_ATA 0x00000101
    #define SATA_SIG_SEMB 0xC33C0101
    #define SATA_SIG_PM 0x96690101

    #define HBA_PxCMD_CR 0x8000
    #define HBA_PxCMD_FRE 0x0010
    #define HBA_PxCMD_ST 0x0001
    #define HBA_PxCMD_FR 0x4000

    PortType CheckPortType(HBAPort* Port){
        uint32_t SataStatus = Port->SataStatus;

        uint8_t InterfacePowerManagement = (SataStatus >> 8) & 0b111;
        uint8_t DeviceDetection = SataStatus & 0b111;

        if (DeviceDetection != HBA_PORT_DEV_PRESENT) return PortType::None;
        if (InterfacePowerManagement != HBA_PORT_IPM_ACTIVE) return PortType::None;

        switch (Port->Signature){
            case SATA_SIG_ATAPI:
                return PortType::SATAPI;
            case SATA_SIG_ATA:
                return PortType::SATA;
            case SATA_SIG_PM:
                return PortType::PM;
            case SATA_SIG_SEMB:
                return PortType::SEMB;
            default:
                PortType::None;
        }
    }

    void AHCIDriver::ProbePorts(){
        uint32_t PortsImplemented = ABAR->PortsImplemented;
        for (int i = 0; i < 32; i++){
            if (PortsImplemented & (1 << i)){
                PortType portType = CheckPortType(&ABAR->Ports[i]);

                if (portType == PortType::SATA || portType == PortType::SATAPI){
                    Ports[PortCount] = new Port();
                    Ports[PortCount]->AHCIPortType = portType;
                    Ports[PortCount]->HBAPortPtr = &ABAR->Ports[i];
                    Ports[PortCount]->PortNumber = PortCount;
                    PortCount++;
                }
            }
        }
    }

    void Port::Configure(){
        StopCMD();

        void* NewBase = GlobalAllocator->RequestPage();
        HBAPortPtr->CommandListBase = (uint32_t)(uint64_t)NewBase;
        HBAPortPtr->CommandListBaseUpper = (uint32_t)((uint64_t)NewBase >> 32);
        Memset((void*)(HBAPortPtr->CommandListBase), 0, 1024);

        void* FISBase = GlobalAllocator->RequestPage();
        HBAPortPtr->FISBaseAddress = (uint32_t)(uint64_t)FISBase;
        HBAPortPtr->FISBaseAddressUpper = (uint32_t)((uint64_t)FISBase >> 32);
        Memset(FISBase, 0, 256);

        HBACommandHeader* CommandHeader = (HBACommandHeader*)((uint64_t)HBAPortPtr->CommandListBase + ((uint64_t)HBAPortPtr->CommandListBaseUpper << 32));

        for (int i = 0; i < 32; i++){
            CommandHeader[i].PRDTLength = 8;

            void* CommandTableAddress = GlobalAllocator->RequestPage();
            uint64_t Address = (uint64_t)CommandTableAddress + (i << 8);
            CommandHeader[i].CommandTableBaseAddress = (uint32_t)(uint64_t)Address;
            CommandHeader[i].CommandTableBaseAddressUpper = (uint32_t)((uint64_t)Address >> 32);
            _memset(CommandTableAddress, 0, 256);
        }

        StartCMD();
    }

    void Port::StopCMD(){
        HBAPortPtr->CommandStatus &= ~HBA_PxCMD_ST;
        HBAPortPtr->CommandStatus &= ~HBA_PxCMD_FRE;

        while(true){
            if (HBAPortPtr->CommandStatus & HBA_PxCMD_FR) continue;
            if (HBAPortPtr->CommandStatus & HBA_PxCMD_CR) continue;

            break;
        }

    }

    void Port::StartCMD(){
        while (HBAPortPtr->CommandStatus & HBA_PxCMD_CR);

        HBAPortPtr->CommandStatus |= HBA_PxCMD_FRE;
        HBAPortPtr->CommandStatus |= HBA_PxCMD_ST;
    }

    int Port::FindCommandSlot()
    {
        uint32_t cmdSlots = 32;
        uint32_t slots = (HBAPortPtr->SataControl | HBAPortPtr->CommandIssue);
        for (int i = 0; i < cmdSlots; i++)
        {
            if ((slots & 1) == 0)
                return i;
            slots >>= 1;
        }
        //osData.debugTerminalWindow->Log("Coult not find free Command Slot!");
        return -1;
    }

    bool Port::Write(uint64_t sector, uint32_t sectorCount, void* buffer)
    {
        uint32_t sectorL = (uint32_t)sector;
        uint32_t sectorH = (uint32_t)(sector >> 32);
        uint32_t sectorCountCopy = sectorCount;
        
        HBAPortPtr->InterruptStatus = (uint32_t)-1;
        int slot = FindCommandSlot();
        if (slot == -1)
            return false;

        HBACommandHeader* cmdHeader = (HBACommandHeader*)(uint64_t)HBAPortPtr->CommandListBase;
        cmdHeader += slot; // A
        cmdHeader->CommandFISLength = sizeof(FIS_REG_H2D) / sizeof(uint32_t); // command FIS size
        cmdHeader->Write = 1;
        cmdHeader->PRDTLength = ((sectorCount) / 16) + 1;

        HBACommandTable* commandTable = (HBACommandTable*)((uint64_t)cmdHeader->CommandTableBaseAddress);
        _memset(commandTable, 0, sizeof(HBACommandTable) + (cmdHeader->PRDTLength - 1) * sizeof(HBAPRDTEntry));

        int i = 0;
        for (i = 0; i < cmdHeader->PRDTLength - 1; i++)
        {
            commandTable->PRDTEntry[i].DataBaseAddress = (uint32_t)(uint64_t)buffer;
            commandTable->PRDTEntry[i].DataBaseAddressUpper = (uint32_t)((uint64_t)buffer >> 32);
            commandTable->PRDTEntry[i].ByteCount = 0x2000 - 1;
            commandTable->PRDTEntry[i].InterruptOnCompletion = 1;
            buffer = (uint8_t*)buffer + 0x2000;
            sectorCount -= 16;
        }

        commandTable->PRDTEntry[i].DataBaseAddress = (uint32_t)(uint64_t)buffer;
        commandTable->PRDTEntry[i].DataBaseAddressUpper = (uint32_t)((uint64_t)buffer >> 32);
        commandTable->PRDTEntry[i].ByteCount = (sectorCount << 9) - 1; // 512 bytes per sector
        //osData.debugTerminalWindow->Log("Writing {} Bytes.", to_string((uint64_t)(commandTable->prdtEntry[i].byteCount + 1)), Colors.bgreen);
        commandTable->PRDTEntry[i].InterruptOnCompletion = 1;
        
        FIS_REG_H2D* cmdFIS = (FIS_REG_H2D*)(&commandTable->CommandFIS);
        cmdFIS->FISType = FIS_TYPE_REG_H2D;
        cmdFIS->CommandControl = 1;
        cmdFIS->Command = 0x35;

        // cmdFIS->lba0 = (uint8_t)sectorL;
        // cmdFIS->lba1 = (uint8_t)(sectorL >> 8);
        // cmdFIS->lba2 = (uint8_t)(sectorL >> 16);
        // cmdFIS->lba3 = (uint8_t)sectorH;
        // cmdFIS->lba4 = (uint8_t)(sectorH >> 8);
        // cmdFIS->lba5 = (uint8_t)(sectorH >> 16);

        cmdFIS->LBA0 = (uint8_t)sectorL;
        cmdFIS->LBA1 = (uint8_t)(sectorL >> 8);
        cmdFIS->LBA2 = (uint8_t)(sectorL >> 16);
        cmdFIS->LBA3 = (uint8_t)(sectorL >> 24);
        cmdFIS->LBA4 = (uint8_t)sectorH;
        cmdFIS->LBA5 = (uint8_t)(sectorH >> 8);

        cmdFIS->DeviceRegister = 1<<6; // Set to LBA Mode

        cmdFIS->CountLow = sectorCountCopy & 0xFF;
        cmdFIS->CountHigh = (sectorCountCopy >> 8) & 0xFF;
        
        uint64_t spin = 0;
        while((HBAPortPtr->TaskFileData & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 1000000)
            spin++;
        if (spin == 1000000)
            return false;
        //osData.debugTerminalWindow->Log("Spin: {}", to_string(spin), Colors.bblue);

        HBAPortPtr->CommandIssue = 1<<slot; // A
        
        while (true)
        {
            if ((HBAPortPtr->CommandIssue & (1<<slot)) == 0) // A
                break;
            if (HBAPortPtr->InterruptStatus & HBA_PxIS_TFES) 
                return false;
        }

        if (HBAPortPtr->InterruptStatus & HBA_PxIS_TFES) 
                return false;

        return true;
    }


    bool Port::Read(uint64_t Sector, uint32_t SectorCount, void* Buffer){
        uint32_t SectorL = (uint32_t) Sector;
        uint32_t SectorH = (uint32_t) (Sector >> 32);

        int slot = FindCommandSlot();
        if (slot == -1)
            return false;

        HBAPortPtr->InterruptStatus = (uint32_t)-1; // Clear pending interrupt bits

        HBACommandHeader* CommandHeader = (HBACommandHeader*)HBAPortPtr->CommandListBase;
        CommandHeader->CommandFISLength = sizeof(FIS_REG_H2D)/ sizeof(uint32_t); //command FIS size;
        CommandHeader->Write = 0; //this is a read
        CommandHeader->PRDTLength = 1;

        HBACommandTable* CommandTable = (HBACommandTable*)(CommandHeader->CommandTableBaseAddress);
        Memset(CommandTable, 0, sizeof(HBACommandTable) + (CommandHeader->PRDTLength-1)*sizeof(HBAPRDTEntry));

        CommandTable->PRDTEntry[0].DataBaseAddress = (uint32_t)(uint64_t)Buffer;
        CommandTable->PRDTEntry[0].DataBaseAddressUpper = (uint32_t)((uint64_t)Buffer >> 32);
        CommandTable->PRDTEntry[0].ByteCount = (SectorCount<<9)-1; // 512 bytes per sector
        CommandTable->PRDTEntry[0].InterruptOnCompletion = 1;

        FIS_REG_H2D* CommandFIS = (FIS_REG_H2D*)(&CommandTable->CommandFIS);

        CommandFIS->FISType = FIS_TYPE_REG_H2D;
        CommandFIS->CommandControl = 1; // command
        CommandFIS->Command = ATA_CMD_READ_DMA_EX;

        CommandFIS->LBA0 = (uint8_t)SectorL;
        CommandFIS->LBA1 = (uint8_t)(SectorL >> 8);
        CommandFIS->LBA2 = (uint8_t)(SectorL >> 16);
        CommandFIS->LBA3 = (uint8_t)SectorH;
        CommandFIS->LBA4 = (uint8_t)(SectorH >> 8);
        CommandFIS->LBA4 = (uint8_t)(SectorH >> 16);

        CommandFIS->DeviceRegister = 1<<6; //LBA mode

        CommandFIS->CountLow = SectorCount & 0xFF;
        CommandFIS->CountHigh = (SectorCount >> 8) & 0xFF;

        uint64_t Spin = 0;

        while ((HBAPortPtr->TaskFileData & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && Spin < 1000000){
            Spin ++;
        }
        if (Spin == 1000000) {
            return false;
        }

        HBAPortPtr->CommandIssue = 1;

        while (true){

            if((HBAPortPtr->CommandIssue == 0)) break;
            if(HBAPortPtr->InterruptStatus & HBA_PxIS_TFES)
            {
                return false;
            }
        }

        return true;
    }

    AHCIDriver::AHCIDriver(PCI::PCIDeviceHeader* PCIBaseAddress){
        this->PCIBaseAddress = PCIBaseAddress;
        GlobalRenderer->Print("AHCI Driver instance initialized");
        

        ABAR = (HBAMemory*)((PCI::PCIHeader0*)PCIBaseAddress)->BAR5;

        GlobalPageTableManager.MapMemory(ABAR, ABAR);
        ProbePorts();
        
        for (int i = 0; i < PortCount; i++){
            Port* Port = Ports[i];
            PortType pe = Port->AHCIPortType;

            if(pe == PortType::SATA)
                GlobalRenderer->Println("SATA");
            else if(pe == PortType::SATAPI)
                GlobalRenderer->Println("SATAPI");
            else
                GlobalRenderer->Println("Error:NI");
            
            

            Port->Configure();

            Port->Buffer = (uint8_t*)GlobalAllocator->RequestPage();
            Memset(Port->Buffer, 0, 0x1000);

            Port->Read(0, 4, Port->Buffer);
            
        }
    }

    AHCIDriver::~AHCIDriver(){

    }
}