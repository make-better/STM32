#include "bsp_i2c.h"
#include "i2c.h"
#include "stdio.h"
#include "main.h"

/*HAL库写入1Byte*/
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

/*HAL库读取1Byte*/
uint32_t I2C_EE_BufferRead(uint8_t* pBuffer, uint8_t ReadAddr, uint16_t NumByteToRead)
{
    HAL_StatusTypeDef status = HAL_OK;
    status = HAL_I2C_Mem_Read(&hi2c1, EEPROM_ADDRESS, ReadAddr, I2C_MEMADD_SIZE_8BIT, pBuffer, NumByteToRead, 100);
    return status;
}

#define I2C_WR 0
#define I2C_RD 1

#define I2Cx_SDA_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOB_CLK_ENABLE()
#define I2Cx_SCL_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOB_CLK_ENABLE()

#define I2Cx_SCL_PIN                GPIO_PIN_6
#define I2Cx_SCL_GPIO_PORT          GPIOB
#define I2Cx_SDA_PIN                GPIO_PIN_7
#define I2Cx_SDA_GPIO_PORT          GPIOB

#define I2C_SCL_1()  I2Cx_SCL_GPIO_PORT->BSRR = I2Cx_SCL_PIN /*SCL = 1*/
#define I2C_SCL_0()  I2Cx_SCL_GPIO_PORT->BRR = I2Cx_SCL_PIN  /*SCL = 1*/

#define I2C_SDA_1()  I2Cx_SDA_GPIO_PORT->BSRR = I2Cx_SDA_PIN /*SDA = 1*/
#define I2C_SDA_0()  I2Cx_SDA_GPIO_PORT->BRR = I2Cx_SDA_PIN  /*SDA = 1*/

#define I2C_SDA_READ() ((I2Cx_SDA_GPIO_PORT->IDR & I2Cx_SDA_PIN) != 0) /* 读取SDA状态 */

static void i2c_start(void);
static void i2c_stop(void);

void I2C_By_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    I2Cx_SDA_GPIO_CLK_ENABLE();
    
    GPIO_InitStruct.Pin = I2Cx_SCL_PIN|I2Cx_SDA_PIN;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    HAL_GPIO_Init(I2Cx_SDA_GPIO_PORT, &GPIO_InitStruct);
    
    i2c_stop();
}

static void i2c_delay(void)
{
    uint8_t i;
    /*72M SCL频率约为205KHz*/
    for(i = 0; i < 20; i++);
}

static void i2c_start(void)
{
    I2C_SDA_1();
    I2C_SCL_1();
    i2c_delay();
    I2C_SDA_0();
    i2c_delay();
    I2C_SCL_0();
    i2c_delay();
}

static void i2c_stop(void)
{
    I2C_SDA_0();
    I2C_SCL_1();
    i2c_delay();
    I2C_SDA_1();
}

static void i2c_send_byte(uint8_t byte)
{
    uint8_t i;
    for(i = 0; i < 8; i++)
    {
        if(byte & 0x80)
        {
            I2C_SDA_1();
        }
        else
        {
            I2C_SDA_0();
        }
        i2c_delay();
        I2C_SCL_1();
        i2c_delay();
        I2C_SCL_0();
        if(i == 7)
        {
            I2C_SDA_1(); // 释放总线
        }
        byte <<= 1;
        i2c_delay();
    }
}

static uint8_t i2c_read_byte(void)
{
    uint8_t i;
    uint8_t value = 0;
    for(i = 0; i < 8; i++)
    {
        value <<= 1;
        I2C_SCL_1();
        i2c_delay();
        if(I2C_SDA_READ())
        {
            value++;
        }
        I2C_SCL_0();
        i2c_delay();
    }
    return value;
}

static uint8_t i2c_wait_ack(void)
{
    uint8_t re;
    I2C_SDA_1();
    i2c_delay();
    I2C_SCL_1();
    i2c_delay();
    if(I2C_SDA_READ())
    {
        re = 1;
    }
    else
    {
        re = 0;
    }
    I2C_SCL_0();
    i2c_delay();
    return re;
}

static void i2c_ack(void)
{
    I2C_SDA_0();
    i2c_delay();
    I2C_SCL_1();
    i2c_delay();
    I2C_SCL_0();
    i2c_delay();
    I2C_SDA_1();
}

static void i2c_nack(void)
{
    I2C_SDA_1();
    i2c_delay();
    I2C_SCL_1();
    i2c_delay();
    I2C_SCL_0();
    i2c_delay();
}

uint8_t i2c_check_device(uint8_t devAddr)
{
    uint8_t ack;
    i2c_start();
    i2c_send_byte(devAddr | I2C_WR);
    ack = i2c_wait_ack();
    i2c_stop();
    return ack;
}
