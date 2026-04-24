#include "bsp_readWriteProtect.h"
#include "bsp_usart.h"

#define WRITE_PROTECTION_DISABLE 1
#define WRITE_PROTECTION_ENABLE 0

#define FLASH_USER_START_ADDR       ADDR_FLASH_PAGE_254   /* Start @ of user Flash area */
#define FLASH_USER_END_ADDR         ADDR_FLASH_PAGE_255   /* End @ of user Flash area */
#define FLASH_PAGE_TO_BE_PROTECTED (OB_WRP_PAGES62TO255)  
#define DATA_32                     ((uint32_t)0x12345678)

uint8_t Flash_RW_Protect_Test(void)
{
    debug_info("flash read write protect test start\r\n");
    
    uint8_t ret = 0;
    uint32_t page_error = 0, address = 0;
    FLASH_OBProgramInitTypeDef options_bytes_struct;
    FLASH_EraseInitTypeDef erase_init_struct;
    
    HAL_FLASH_Unlock();
    
    HAL_FLASH_OB_Unlock();
    
    HAL_FLASHEx_OBGetConfig(&options_bytes_struct);
    
#if WRITE_PROTECTION_DISABLE == 1
    /* 检查所需页面是否已被写保护 */
    if((options_bytes_struct.WRPPage & FLASH_PAGE_TO_BE_PROTECTED) != FLASH_PAGE_TO_BE_PROTECTED)
    {
        debug_info("page protected, release write protiction\r\n");
        /* 恢复写保护页面 */
        options_bytes_struct.OptionType = OPTIONBYTE_WRP;
        options_bytes_struct.WRPState = OB_WRPSTATE_DISABLE;
        options_bytes_struct.WRPPage = FLASH_PAGE_TO_BE_PROTECTED;
        if(HAL_FLASHEx_OBProgram(&options_bytes_struct) != HAL_OK)
        {
            debug_info("write protiction release faild\r\n");
            return 1;
        }
        /* 生成系统重置以加载新选项字节值 */
        HAL_FLASH_OB_Launch();
    }
#elif WRITE_PROTECTION_ENABLE == 1
    if(((~options_bytes_struct.WRPPage) & FLASH_PAGE_TO_BE_PROTECTED) != FLASH_PAGE_TO_BE_PROTECTED)
    {
        debug_info("page have not write protection, starting write protiection\r\n");
        /* 启用页面写保护 */
        options_bytes_struct.OptionType = OPTIONBYTE_WRP;
        options_bytes_struct.WRPState = OB_WRPSTATE_ENABLE;
        options_bytes_struct.WRPPage = FLASH_PAGE_TO_BE_PROTECTED;
        if(HAL_FLASHEx_OBProgram(&options_bytes_struct) != HAL_OK)
        {
            debug_info("write protiction start faild\r\n");
            return 1;
        }
        /* 生成系统重置以加载新选项字节值 */
        HAL_FLASH_OB_Launch();
    }
#endif
    /* 锁定选项字节 */
    HAL_FLASH_OB_Lock();
    /* 所选页面未被写保护 */
    if((options_bytes_struct.WRPPage & FLASH_PAGE_TO_BE_PROTECTED) != 0x00)
    {
        erase_init_struct.TypeErase = FLASH_TYPEERASE_PAGES;
        erase_init_struct.PageAddress = FLASH_USER_START_ADDR;
        erase_init_struct.NbPages = (FLASH_USER_END_ADDR - FLASH_USER_START_ADDR)/FLASH_PAGE_SIZE;
        if(HAL_FLASHEx_Erase(&erase_init_struct, &page_error) != HAL_OK)
        {
            debug_info("erase page faild\r\n");
            return 1;
        }
        /* 向扇区写入值 */
        address = FLASH_USER_START_ADDR;
        while(address < FLASH_USER_END_ADDR)
        {
            if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address, DATA_32) == HAL_OK)
            {
                address += 4;
            }
            else
            {
                debug_info("flash program faild\r\n");
                return 1;
            }
        }
        /* 检查数据值 */
        address = FLASH_USER_START_ADDR;
        while(address < FLASH_USER_END_ADDR)
        {
            if((*(__IO uint32_t*)address) != DATA_32)
            {
                ret = 1;
            }
            address += 4;
        }
        debug_info("flash write protection test ok\r\n");
    }
    else//所选页面被写保护
    {
        debug_info("page have been write protected, starting write flash test\r\n");
        /* 检查是否允许写入 */
        address = FLASH_USER_START_ADDR;
        if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address, DATA_32) != HAL_OK)
        {
            /* 查看WRPERR标志 */
            if(HAL_FLASH_GetError() == HAL_FLASH_ERROR_WRP)
            {
                ret = 1;
            }
            else
            {
                debug_info("write protection get error failed\r\n");
                return 1;
            }
        }
        else
        {
            debug_info("write protection error\r\n");
            return 1;
        }
        debug_info("flash write protection test ok\r\n");
    }
    
    HAL_FLASH_Lock();
    return 0;
}

void Flash_Write_Proection_Disable(void)
{
    FLASH_OBProgramInitTypeDef options_bytes_struct;
    HAL_FLASH_Unlock();
    
    HAL_FLASH_OB_Unlock();
    
    HAL_FLASHEx_OBGetConfig(&options_bytes_struct);
    if((options_bytes_struct.WRPPage & FLASH_PAGE_TO_BE_PROTECTED) != FLASH_PAGE_TO_BE_PROTECTED)
    {
        debug_info("page protected, release write protiction\r\n");
        /* 恢复写保护页面 */
        options_bytes_struct.OptionType = OPTIONBYTE_WRP;
        options_bytes_struct.WRPState = OB_WRPSTATE_DISABLE;
        options_bytes_struct.WRPPage = FLASH_PAGE_TO_BE_PROTECTED;
        if(HAL_FLASHEx_OBProgram(&options_bytes_struct) != HAL_OK)
        {
            debug_info("write protiction release faild\r\n");
            return;
        }
        /* 生成系统重置以加载新选项字节值 */
        HAL_FLASH_OB_Launch();
    }
}

