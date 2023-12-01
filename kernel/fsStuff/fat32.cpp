#include "fat32.h"

using namespace DiskInterface;

bpb_t ReadBPB(AHCI::Port* port){
    bpb_t bpb;
    SataDiskInterface diskInterface(port);
    diskInterface.ReadBytes(0,sizeof(bpb_t),&bpb);
    return bpb;
}