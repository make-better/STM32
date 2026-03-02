#include "bsp_i2c.h"
#include "i2c.h"
#include "stdio.h"

/*使用HAL库写1Byte数据*/
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

/*使用HAL库读1Byte数据*/
uint32_t I2C_EE_BufferRead(uint8_t* pBuffer, uint8_t ReadAddr, uint16_t NumByteToRead)
{
    HAL_StatusTypeDef status = HAL_OK;
    status = HAL_I2C_Mem_Read(&hi2c1, EEPROM_ADDRESS, ReadAddr, I2C_MEMADD_SIZE_8BIT, pBuffer, NumByteToRead, 100);
    return status;
}

#define EEPROM_I2C_WR 0
#define EEPROM_I2C_RD 1

#define I2Cx_SDA_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOB_CLK_ENABLE()
#define I2Cx_SCL_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOB_CLK_ENABLE()

#define I2Cx_SCL_PIN                GPIO_PIN_6
#define I2Cx_SCL_GPIO_PORT          GPIOB
#define I2Cx_SDA_PIN                GPIO_PIN_7
#define I2Cx_SDA_GPIO_PORT          GPIOB

#define EEPROM_I2C_SCL_1()  I2Cx_SCL_GPIO_PORT->BSRR = I2Cx_SCL_PIN /*SCL = 1*/
#define EEPROM_I2C_SCL_0()  I2Cx_SCL_GPIO_PORT->BRR = I2Cx_SCL_PIN  /*SCL = 1*/

#define EEPROM_I2C_SDA_1()  I2Cx_SDA_GPIO_PORT->BSRR = I2Cx_SDA_PIN /*SDA = 1*/
#define EEPROM_I2C_SDA_0()  I2Cx_SDA_GPIO_PORT->BRR = I2Cx_SDA_PIN  /*SDA = 1*/

#define EEPROM_I2C_SDA_READ() ((I2Cx_SDA_GPIO_PORT->IDR & I2Cx_SDA_PIN) != 0)

void I2C_By_GPIO_Init(void)
{
    I2Cx_SDA_GPIO_CLK_ENABLE();
    I2Cx_SCL_GPIO_CLK_ENABLE();
}