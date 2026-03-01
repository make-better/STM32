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

#ifdef __cplusplus
}
#endif

#endif