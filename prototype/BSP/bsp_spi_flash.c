#include "bsp_spi_flash.h"
#include "spi.h"
#include "main.h"

#define digitalHi(p,i)          {p->BSRR=i;}                    //设置为高电平
#define digitalLo(p,i)          {p->BSRR=(uint32_t)i << 16;}    //输出低电平

#define SPI_FLASH_CS_LOW()      digitalLo(SPIFLASH_CS_GPIO_Port, SPIFLASH_CS_Pin )
#define SPI_FLASH_CS_HIGH()     digitalHi(SPIFLASH_CS_GPIO_Port, SPIFLASH_CS_Pin )
 
extern SPI_HandleTypeDef hspi1;

/*FLASH常用命令*/
#define W25X_WriteEnable 0x06
#define W25X_WriteDisable 0x04
#define W25X_ReadStatusReg 0x05
#define W25X_WriteStatusReg 0x01
#define W25X_ReadData 0x03
#define W25X_FastReadData 0x0B
#define W25X_FastReadDual 0x3B
#define W25X_PageProgram 0x02
#define W25X_BlockErase 0xD8
#define W25X_SectorErase 0x20
#define W25X_ChipErase 0xC7
#define W25X_PowerDown 0xB9
#define W25X_ReleasePowerDown 0xAB
#define W25X_DeviceID 0xAB
#define W25X_ManufactDeviceID 0x90
#define W25X_JedecDeviceID 0x9F
/*其它*/
#define sFLASH_ID 0XEF4017
#define Dummy_Byte 0xFF


uint32_t spi_flash_read_id(void)
{
    uint8_t temp[4] = {0};
    uint8_t send[4] = {W25X_JedecDeviceID, Dummy_Byte, Dummy_Byte, Dummy_Byte};
    HAL_StatusTypeDef status = HAL_OK;
    
    HAL_GPIO_WritePin(SPIFLASH_CS_GPIO_Port, SPIFLASH_CS_Pin, 0);
    status = HAL_SPI_TransmitReceive(&hspi1, send, temp, 4, 1000);
    HAL_GPIO_WritePin(SPIFLASH_CS_GPIO_Port, SPIFLASH_CS_Pin, 0);
    if(status != HAL_OK)
    {
        return 0x0;
    }
    return temp[1] << 16 | temp[2] << 8 | temp[3];
}






