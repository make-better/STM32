#include "bsp_readWriteProtect.h"
#include "bsp_usart.h"

#define WRITE_PROTECTION_DISABLE 1
#define WRITE_PROTECTION_ENABLE 0

#define FLASH_USER_START_ADDR       ADDR_FLASH_PAGE_64   /* Start @ of user Flash area */
#define FLASH_USER_END_ADDR         ADDR_FLASH_PAGE_68   /* End @ of user Flash area */
#define FLASH_PAGE_TO_BE_PROTECTED (OB_WRP_PAGES0TO1 | OB_WRP_PAGES2TO3)  
#define DATA_32                     ((uint32_t)0x12345678)

void Flash_RW_Protect_Test(void)
{
    debug_info("flash read write protect test start\r\n");
    
    uint8_t ret = 0;
    FLASH_OBProgramInitTypeDef options_bytes_struct;
    
    HAL_FLASH_Unlock();
    
    HAL_FLASH_OB_Unlock();
    
    HAL_FLASHEx_OBGetConfig(&options_bytes_struct);
    
#ifdef WRITE_PROTECTION_DISABLE
    /* 检查所需页面是否已被写保护 */
    if((options_bytes_struct & FLASH_PAGE_TO_BE_PROTECTED) != )
#elif defined WRITE_PROTECTION_ENABLE
    
    
#endif
    
}



