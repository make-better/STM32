#include "bsp_internal_flash.h"
#include "main.h"

#define WRITE_START_ADDR ((uint32_t)0x08010000)

#define WRITE_END_ADDR ((uint32_t)0x08011000)

uint8_t InternalFlash_Test(void)
{
    uint32_t address = 0x00;    //记录写入地址
    uint32_t data_32 = 0x3210ABCD;  //记录写入的数据
    uint32_t nbr_of_page = 0x00; //记录写入多少页
    uint8_t ret = 0;
    
    uint32_t sectro_err = 0;
    static FLASH_EraseInitTypeDef EraseInitStruct;
    /* 解锁 */
    HAL_FLASH_Unlock();
    
    /* 计算要擦除多少页 */
    nbr_of_page = (WRITE_END_ADDR - WRITE_START_ADDR) / FLASH_PAGE_SIZE;
    EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.NbPages = nbr_of_page;
    EraseInitStruct.PageAddress = WRITE_START_ADDR;
    
    if(HAL_FLASHEx_Erase(&EraseInitStruct, &sectro_err) != HAL_OK)
    {
        /* 擦除出错 */
        return -1;
    }
    //写入数据
    address = WRITE_START_ADDR;
    while(address < WRITE_END_ADDR)
    {
        if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address, data_32) == HAL_OK)
        {
            address += 4;
        }
        else
        {
            return -1;
        }
    }
    HAL_FLASH_Lock();
    
    /* 检查写入数据 */
    address = WRITE_START_ADDR;
    while((address < WRITE_END_ADDR) && ret == 0)
    {
        if((*(__IO uint32_t *)address) != data_32)
        {
            ret = 1;
        }
        address += 4;
    }
    return ret;
}

