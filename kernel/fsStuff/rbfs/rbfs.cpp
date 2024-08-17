#include "rbfs.h"
#include "../../diskStuff/Disk_Interfaces/sata/sataDiskInterface.h"
#include "../../osData/osData.h"
#include <libm/memStuff.h>
#include "../../devices/rtc/rtc.h"
#include <libm/stubs.h>

static int CurDisk;
static uint32_t CurBlockSize;
static int SBlockDataSize;

#define ThisSata (DiskInterface::SataDiskInterface*)osData.diskInterfaces[CurDisk]

int InitTFS(uint32_t BlockSize,uint64_t DiskNum){
    if(!(BlockSize % 512)){return 0;}
    CurDisk = DiskNum;
    CurBlockSize = 4096;
    SBlockDataSize = BlockSize - 5;
    if(ThisSata->WriteBytes(0x201,ThisSata->GetMaxSectorCount(),NULL) == true){
        return 1;
    }else{return 0;}
    return 0;
}

PACK(typedef struct TFSB{                 
    bool HaveNextBlock;                   
    uint64_t NextBlockAddr;               
    char data[4091];               
})RBFSBlock;                              

typedef struct RDR{
    int i;bool IsLast;
    int IsSuccess;
}RDRets;

static uint64_t GetAllBlockSize(uint64_t BStartAddr){
    RBFSBlock *TBlock;
    uint64_t i;
GetAgain:
    if(ThisSata->ReadBytes(BStartAddr,CurBlockSize,(void*)TBlock) == true){
        if(TBlock->HaveNextBlock){
            BStartAddr = TBlock->NextBlockAddr;
            i++;
            goto GetAgain;
        }
        else if(!TBlock->HaveNextBlock){
            return i * (CurBlockSize - 5);
        }else{return 0;}
    }else{return 0;}
    return 0;
}

static char* MemCat(char *_s1, const char *_s2)
{
    char *pszOrigDst = _s1;

    while(*_s1)
        _s1++;
    while((*_s1++ = *_s2++) != '\0')
        ;

    return pszOrigDst;
}

static RDRets _RDF(char* buffer,uint64_t StartAddr){
    uint64_t i = 0;
    _memset(buffer,0,sizeof(*buffer));
    RDRets Ret;
    RBFSBlock *TBlock;
    if(ThisSata->ReadBytes(StartAddr,CurBlockSize,TBlock) == true){
        i++;
        buffer = MemCat((char*)buffer,(char*)TBlock->data);
        if(TBlock->HaveNextBlock == false){Ret.i = i;Ret.IsLast = true;Ret.IsSuccess = true;return Ret;}
        else{
            Ret = _RDF(buffer,TBlock->NextBlockAddr);
            if(Ret.IsLast == true){Ret.i = i;Ret.IsSuccess = true;return Ret;}
        }
    }else{Ret.IsSuccess = false;return Ret;}
}

int ReadFile(uint64_t StartAddr,void* buffer){
    uint64_t i = 0;
    char* p;
    RBFSFile *Tfile;
    if(ThisSata->ReadBytes(StartAddr,sizeof(RBFSFile),(void*)Tfile) == true){
        if(Tfile->StartBlockAddr == 0 && Tfile->FileSize == 0){return ReadFault::EmptyFile;} //空文件
        else{
            p = (char*)_Malloc1(GetAllBlockSize(Tfile->StartBlockAddr));
            RDRets ret = _RDF(p,Tfile->StartBlockAddr);
            if(ret.IsSuccess == true){buffer = (void*)p;return ReadFault::Success;}
            else{return ReadFault::DiskReadFault;}
        }
        Tfile->FileSize = i * (CurBlockSize - 5);
        Tfile->NearlyAccessTime.Hour = RTC::Hour;
        Tfile->NearlyAccessTime.Minute = RTC::Minute;
        Tfile->NearlyAccessTime.Second = RTC::Second;
        Tfile->NearlyAccessDate.Year = RTC::Year - 1980;
        Tfile->NearlyAccessDate.Month = RTC::Month;
        Tfile->NearlyAccessDate.Day = RTC::Day;
        ThisSata->WriteBytes(StartAddr,sizeof(RBFSFile),Tfile);
        return ReadFault::Success;
    }else{return ReadFault::DiskReadFault;}
    return ReadFault::NoAnyCanDo;
}