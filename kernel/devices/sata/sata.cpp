#include "sata.h"
#include "../../kernelStuff/stuff/stackmacro.h"
#include "../../paging/PageFrameAllocator.h"
#include "../../paging/PageTableManager.h"

sata::sata(AHCI::Port *port){
    AddToStack();
    this->Port = port;
    this->Port->Buffer = (uint8_t*)GlobalAllocator->RequestPage();

    GlobalPageTableManager.MapMemory(this->Port->buffer, this->Port->buffer, false);
    RemoveFromStack();

    AddToStack();
    //SectorCount = ();
    RemoveFromStack();
}