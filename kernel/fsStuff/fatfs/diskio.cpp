/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */
#include "../../diskStuff/Disk_Interfaces/sata/sataDiskInterface.h"
#include "../../osData/osData.h"

/* Definitions of physical drive number for each drive */
#define DEV_RAM		0	/* Example: Map Ramdisk to physical drive 0 */
#define DEV_MMC		1	/* Example: Map MMC/SD card to physical drive 1 */
#define DEV_USB		3	/* Example: Map USB MSD to physical drive 2 */
#define DEV_SATA    2	/* Example: Map SATA to physical drive 3 */

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	if(osData.diskInterfaces[pdrv]->InterfaceType != DiskInterface::Sata){
        return STA_NODISK;
    }else{
        return STA_NOINIT;
    }

	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	if(osData.diskInterfaces[pdrv]->InterfaceType != DiskInterface::Sata){
        return STA_NODISK;
    }else{
        return STA_PROTECT;
    }
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	LBA_t sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{

	if(osData.diskInterfaces[pdrv]->InterfaceType != DiskInterface::Sata){
        return RES_PARERR;
    }else{
        if((int)osData.diskInterfaces[pdrv]->Read(sector, count, (void*)buff) == true){
            return RES_OK;
        }
    }

	return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	LBA_t sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{

	if(osData.diskInterfaces[pdrv]->InterfaceType != DiskInterface::Sata){
        return RES_PARERR;
    }else{
        if((int)osData.diskInterfaces[pdrv]->Write(sector, count, (void*)buff) == true){
            return RES_OK;
        }
    }

	return RES_PARERR;
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
    for(int i = 0; i < osData.diskInterfaces.GetCount();i++){
        if(osData.diskInterfaces[i]->InterfaceType == DiskInterface::Sata){
            switch ( cmd ) //fatfs内核使用cmd调用
            {
                case GET_SECTOR_COUNT:	//扇区数量
                    *(DWORD*)buff = (DWORD)osData.diskInterfaces[pdrv]->GetMaxSectorCount();
                    return RES_OK;
                case GET_SECTOR_SIZE:	//扇区大小 单位bytes
                    *(DWORD*)buff = 512;
                    return RES_OK;
                case GET_BLOCK_SIZE:	//block size, 
                    *(DWORD*)buff = 4096;	//单位为 sector(FatFs)
                    return RES_OK;
                case CTRL_SYNC:			//同步命令，貌似FatFs内核用来判断写操作是否完成
                    return RES_OK;
            }
            //return res;
        }
    }

	return RES_PARERR;
}

