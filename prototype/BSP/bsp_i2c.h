#ifndef __BSP_I2C_H__
#define __BSP_I2C_H__

#ifdef __cplusplus
extern "C" {
#endif
    
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_cortex.h"

#define EEPROM_ADDRESS 0X0A

uint32_t I2C_EE_ByteWrite(uint8_t *pBuffer, uint8_t WriteAddr);
uint32_t I2C_EE_BufferRead(uint8_t* pBuffer, uint8_t ReadAddr, uint16_t NumByteToRead);

void I2C_By_GPIO_Init(void);
uint8_t i2c_check_device(uint8_t devAddr);

uint8_t i2c_write_bytes(uint8_t *buff, uint16_t dev_addr, uint16_t address, uint16_t len);
uint8_t i2c_read_bytes(uint8_t *buff, uint16_t dev_addr, uint16_t address, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif