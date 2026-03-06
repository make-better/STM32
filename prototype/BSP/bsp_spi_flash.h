#ifndef __BSP_SPI_FLASH_H__
#define __BSP_SPI_FLASH_H__

#ifdef __cplusplus
extern "C" {
#endif
    
#include "stm32f1xx_hal.h"
#define sFLASH_ID 0XEF4017

uint32_t spi_flash_read_id(void);
uint32_t spi_flash_read_device_id(void);
uint8_t spi_flash_powerdown(void);
uint8_t spi_flash_wakeup(void);
uint8_t spi_flash_sector_erease(uint32_t sector_addr);
uint8_t spi_flash_buffer_write(uint8_t *buff, uint32_t write_addr, uint16_t len);
uint8_t spi_flash_buffer_read(uint8_t *buff, uint32_t read_addr, uint16_t len);
uint8_t spi_flash_bulk_erase(void);

uint8_t spi_flash_test(void);
#ifdef __cplusplus
}
#endif

#endif