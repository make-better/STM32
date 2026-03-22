/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2025        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"			/* Basic definitions of FatFs */
#include "diskio.h"		/* Declarations FatFs MAI */

/* Example: Declarations of the platform and disk functions in the project */
#include "spi.h"
#include "bsp_spi_flash.h"
#include "bsp_sdio_sdcard.h"
#include "cmsis_os.h"
#include "string.h"

/* Example: Mapping of physical drive number for each drive */
#define DEV_MMC		0	/* Map MMC/SD card to physical drive 0 */
#define SPI_FLASH	1	/* Map FTL to physical drive 1 */
#define DEV_USB		2	/* Map USB MSD to physical drive 2 */



/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat = STA_NOINIT;
	int result;
    int i = 0;

	switch (pdrv) {
	case SPI_FLASH :
        do
        {
            if(sFLASH_ID == spi_flash_read_id())
            {
                stat &= ~STA_NOINIT;
                i = 5;
            }
            else
            {
                stat = STA_NOINIT;
                i++;
            }
        }while(i < 5);
		
		// translate the reslut code here
		return stat;

	case DEV_MMC :
		do
        {
            if(0 == BSP_SD_GetCardState())
            {
                stat &= ~STA_NOINIT;
                i = 5;
            }
            else
            {
                stat = STA_NOINIT;
                i++;
            }
        }while(i < 5);

		return stat;

//	case DEV_USB :
//		result = USB_disk_status();

//		// translate the reslut code here

//		return stat;
    default:break;
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
	DSTATUS stat = STA_NOINIT;
	uint8_t result;
    
	switch (pdrv) {
	case SPI_FLASH :
        result = spi_flash_wakeup();
        if(result)
        {
            stat = STA_NOINIT;
        }
        else
        {
            stat = disk_status(SPI_FLASH);
        }
		// translate the reslut code here

		return stat;

	case DEV_MMC :
		result = BSP_SD_Init();
        if(result)
        {
            stat = STA_NOINIT;
        }
		else
        {
            stat &= ~STA_NOINIT;
        }

		return stat;

//	case DEV_USB :
//		result = USB_disk_initialize();

//		// translate the reslut code here

//		return stat;
        default:break;
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
	DRESULT res = RES_OK;
	uint8_t result;
    uint8_t timeout = 10;

	switch (pdrv) {
	case SPI_FLASH :
		// 扇区偏移2MB,外部flash文件系统空间放在SPI Flash后面6MB空间
        sector += 512;
		//每个扇区4096个字节
        result = spi_flash_buffer_read(buff, sector << 12, count << 12);
        if(result)
        {
            res = RES_ERROR;
        }
		// translate the reslut code here

		return res;

	case DEV_MMC :
        if(BSP_SD_ReadBlocks((uint32_t *)buff, sector, count, 1000) == MSD_OK)
        {
            while(BSP_SD_GetCardState() != SD_TRANSFER_OK)
            {
                HAL_Delay(1);
                timeout--;
                if(timeout == 0)
                {
                    res = RES_ERROR;
                    break;
                }
            }
        }
        else
        {
            res = RES_ERROR;
        }
		return res;

//	case DEV_USB :
//		// translate the arguments here

//		result = USB_disk_read(buff, sector, count);

//		// translate the reslut code here

//		return res;
    default:break;
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
	DRESULT res = RES_OK;
	uint8_t result;
    uint32_t w_addr;
    uint8_t timeout = 10;
	switch (pdrv) {
	case SPI_FLASH :
		// translate the arguments here
        // 扇区偏移2MB,外部flash文件系统空间放在SPI Flash后面6MB空间
        sector += 512;
        w_addr = sector << 12;
        result = spi_flash_sector_erease(w_addr);
        if(result)
        {
            res = RES_WRPRT;
        }
        //每个扇区4096个字节
		result = spi_flash_buffer_write((uint8_t*)buff, w_addr, count << 12);
        if(result)
        {
            res = RES_WRPRT;
        }
        res = RES_OK;
		// translate the reslut code here

		return res;

	case DEV_MMC :
        result = BSP_SD_WriteBlocks((uint32_t *)buff, sector, count, 1000);
        if(result == MSD_OK)
        {
            while(BSP_SD_GetCardState() != SD_TRANSFER_OK)
            {
                HAL_Delay(1);
                timeout--;
                if(timeout == 0)
                {
                    res = RES_ERROR;
                    break;
                }
            }
        }
        else
        {
            res = RES_ERROR;
        }
		return res;

//	case DEV_USB :
//		// translate the arguments here

//		result = USB_disk_write(buff, sector, count);

//		// translate the reslut code here

//		return res;
    default:break;
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
	DRESULT res;
	int result;
    int i = 0;
    BSP_SD_CardInfo card_info;
    
	switch (pdrv) {
	case SPI_FLASH :
        switch(cmd)
        {
            /* 扇区数量:1536*4096/1024/1024 = 6MB */
            case GET_SECTOR_COUNT:
                *(DWORD *)buff = 1536;
                break;
            /* 扇区大小 */
            case GET_SECTOR_SIZE:
                *(WORD *)buff = 4096;
                break;
            /* 同时擦除扇区个数 */
            case GET_BLOCK_SIZE:
                *(DWORD *)buff = 1;
                break;
            case CTRL_SYNC:
                do
                {
                    if(spi_flash_wait_for_write_end() != 0)
                    {
                        i++;
                        res = RES_PARERR;
                    }
                    else
                    {
                        i = 5;
                        res = RES_OK;
                        return res;
                    }
                }while(i < 5);
            default:
                res = RES_PARERR;
                return res;
        }
		// Process of the command for the RAM drive
        res = RES_OK;
		return res;

	case DEV_MMC :
        
        switch(cmd)
        {
            case CTRL_SYNC:
                res = RES_OK;
                break;
            case GET_SECTOR_COUNT:
                BSP_SD_GetCardInfo(&card_info);
                *(DWORD*)buff = card_info.LogBlockNbr;
                res = RES_OK;
                break;
            case GET_SECTOR_SIZE:
                BSP_SD_GetCardInfo(&card_info);
                *(DWORD*)buff = card_info.LogBlockSize;
                res = RES_OK;
                break;
            case GET_BLOCK_SIZE:
                BSP_SD_GetCardInfo(&card_info);
                *(DWORD*)buff = card_info.LogBlockSize;
                res = RES_OK;
                break;
            default:
                res = RES_PARERR;
                break;
        }

		return res;

//	case DEV_USB :

//		// Process of the command the USB drive

//		return res;
    default:break;
	}

	return RES_PARERR;
}

__weak DWORD get_fattime(void)
{
    /* 返回当前时间戳 */
     return    ((DWORD)(2026 - 1980) << 25)  /* Year 2026 */
             | ((DWORD)1 << 21)        /* Month 1 */
             | ((DWORD)1 << 16)        /* Mday 1 */
             | ((DWORD)0 << 11)        /* Hour 0 */
             | ((DWORD)0 << 5)         /* Min 0 */
             | ((DWORD)0 >> 1);        /* Sec 0 */
}
