#include "bsp_i2c.h"
#include "i2c.h"
#include "stdio.h"

uint32_t I2C_EE_ByteWrite(uint8_t *pBuffer, uint8_t WriteAddr)
{
    HAL_StatusTypeDef status = HAL_OK;
    status = HAL_I2C_Mem_Write(&hi2c1, EEPROM_ADDRESS, WriteAddr, I2C_MEMADD_SIZE_8BIT, pBuffer, 1, 100);
    
    if(status != HAL_OK)
    {
        printf("%s %d HAL I2C write fail\r\n",__FUNCTION__, __LINE__);
        return status;
    }
    
    while(HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY);
    
    while(HAL_I2C_IsDeviceReady(&hi2c1, EEPROM_ADDRESS, 3, 10)== HAL_TIMEOUT);
    
    while(HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY);
    return status;
}

uint32_t I2C_EE_BufferRead(uint8_t* pBuffer, uint8_t ReadAddr, uint16_t NumByteToRead)
{
    HAL_StatusTypeDef status = HAL_OK;
    status = HAL_I2C_Mem_Read(&hi2c1, EEPROM_ADDRESS, ReadAddr, I2C_MEMADD_SIZE_8BIT, pBuffer, NumByteToRead, 100);
    return status;
}



