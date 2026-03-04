#ifndef __BSP_SPI_FLASH_H__
#define __BSP_SPI_FLASH_H__

#ifdef __cplusplus
extern "C" {
#endif
    
#include "stm32f1xx_hal.h"
uint32_t spi_flash_read_id(void);

#ifdef __cplusplus
}
#endif

#endif