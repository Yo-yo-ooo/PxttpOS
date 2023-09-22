#pragma once
#include "../ahci/ahci.h"

class sata
{
private:
    AHCI::Port *Port;
    uint32_t SectorCount;
public:
    sata(AHCI::Port *port);
    ~sata();
};


