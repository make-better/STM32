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

#define EEPROM_PAGE_SIZE 8

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
    for(i = 0; i < 10; i++);
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

/*0:ok    1:error*/
uint8_t i2c_check_device(uint8_t devAddr)
{
    uint8_t ack;
    i2c_start();
    i2c_send_byte(devAddr | I2C_WR);
    ack = i2c_wait_ack();
    i2c_stop();
    return ack;
}

/*0:OK     1:error*/
uint8_t i2c_wait_standby(uint16_t addr)
{
    uint32_t wait_count = 0;
    
    while(i2c_check_device(addr))
    {
        if(wait_count++ > 0xFFFF)
        {
            return 1;
        }
    }
    return 0;
}


/*
****************************************************
* 函 数 名: i2c_write_bytes
* 功能说明: 向串行EEPROM指定地址写入若干数据，采用页写操作提高写入效率
* 形    参：dev_addr: 设备地址
*           address : 起始地址
*           len     : 数据长度，单位为字节
*           buff  : 存放数据的缓冲区指针
* 返 回 值: 0 表示成功，1表示失败
****************************************************
*/
uint8_t i2c_write_bytes(uint8_t *buff, uint16_t dev_addr, uint16_t address, uint16_t len)
{
    uint16_t i,m;
    uint16_t addr = address;
    for(i = 0; i < len; i++)
    {
        /*发送首个字节过页面首地址时，需要重新发启动信号和地址*/
        if((i == 0) || (addr & EEPROM_PAGE_SIZE - 1) == 0)
        {
            /* step0: end and start again */
            i2c_stop();
            for(m = 0; m < 1000; m++)
            {
                /* step1: start */
                i2c_start();
                /* step2: send device address and write cmd */
                i2c_send_byte(dev_addr | I2C_WR);
                /* step3: wait ack */
                if(i2c_wait_ack() == 0)
                {
                    break;
                }
            }
            if(m == 1000)
            {
                goto cmd_fail;//timeout
            }
            /* step4: send data address */
            i2c_send_byte(addr);
            /* step5: wait ack */
            if(i2c_wait_ack() != 0)
            {
                goto cmd_fail;
            }
        }
        /* step6: send data */
        i2c_send_byte(buff[i]);
        /* step7: wait ack */
        if(i2c_wait_ack() != 0)
        {
            goto cmd_fail;
        }
        addr++;
    }
    /*step last: stop*/
    i2c_stop();
    /* 等待最后一次写入完成 */
    if(i2c_wait_standby(dev_addr) != 0)
    {
        goto cmd_fail;
    }
    
    return 0;
    
cmd_fail:
    i2c_stop();
    return 1;
}

/*
****************************************************
* 函 数 名: i2c_read_bytes
* 功能说明: 从串行EEPROM指定地址读取若干数据，采用页写操作提高写入效率
* 形    参：dev_addr: 设备地址
*           address : 起始地址
*           len     : 数据长度，单位为字节
*           buff  : 存放读到的数据的缓冲区指针
* 返 回 值: 0 表示成功，1表示失败
****************************************************
*/
uint8_t i2c_read_bytes(uint8_t *buff, uint16_t dev_addr, uint16_t address, uint16_t len)
{
    uint16_t i;
    
    /* step0: start */
    i2c_start();
    
    /* step1: send device address and write cmd */
    i2c_send_byte(dev_addr | I2C_WR);
    
    /* step2: wait ack */
    if(i2c_wait_ack() != 0)
    {
        goto cmd_fail;
    }
    
    /* step3: send data address */
    i2c_send_byte(address);
    
    /* step4: wait ack */
    if(i2c_wait_ack() != 0)
    {
        goto cmd_fail;
    }
    
    /* step5: start i2c again */
    i2c_start();
    
    /* step6: send device address and read cmd */
    i2c_send_byte(dev_addr | I2C_RD);
    
    /* step7: wait ack */
    if(i2c_wait_ack() != 0)
    {
        goto cmd_fail;
    }
    
    /* step8: read data*/
    for(i = 0; i < len; i++)
    {
        buff[i] = i2c_read_byte();
        if(i != len - 1)
        {
            i2c_ack();
        }
        else
        {
            i2c_nack();
        }
    }
    
    /* step9: stop */
    i2c_stop();
    return 0;
    
cmd_fail:
    i2c_stop();
    return 1;
}



