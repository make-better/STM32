#ifndef __BSP_SPI_FLASH_H__
#define __BSP_SPI_FLASH_H__

#ifdef __cplusplus
extern "C" {
#endif
    
#include "stm32f1xx_hal.h"


uint32_t spi_flash_read_id(void);
uint8_t spi_flash_sector_erease(uint32_t sector_addr);
uint8_t spi_flash_buffer_write(uint8_t *buff, uint32_t write_addr, uint16_t len);
uint8_t spi_flash_buffer_read(uint8_t *buff, uint32_t read_addr, uint16_t len);
uint8_t spi_flash_test(void);
#ifdef __cplusplus
}
#endif

#endif