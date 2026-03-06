#include "bsp_spi_flash.h"
#include "spi.h"
#include "main.h"
#include "bsp_usart.h"

#define digitalHi(p,i)          {p->BSRR=i;}                    //设置为高电平
#define digitalLo(p,i)          {p->BSRR=(uint32_t)i << 16;}    //输出低电平

#define SPI_FLASH_CS_LOW()      digitalLo(SPIFLASH_CS_GPIO_Port, SPIFLASH_CS_Pin )
#define SPI_FLASH_CS_HIGH()     digitalHi(SPIFLASH_CS_GPIO_Port, SPIFLASH_CS_Pin )
 
extern SPI_HandleTypeDef hspi1;

/*FLASH常用命令*/
#define W25X_WriteEnable 0x06
#define W25X_WriteDisable 0x04
#define W25X_ReadStatusReg 0x05
#define W25X_WriteStatusReg 0x01
#define W25X_ReadData 0x03
#define W25X_FastReadData 0x0B
#define W25X_FastReadDual 0x3B
#define W25X_PageProgram 0x02
#define W25X_BlockErase 0xD8
#define W25X_SectorErase 0x20
#define W25X_ChipErase 0xC7
#define W25X_PowerDown 0xB9
#define W25X_ReleasePowerDown 0xAB
#define W25X_DeviceID 0xAB
#define W25X_ManufactDeviceID 0x90
#define W25X_JedecDeviceID 0x9F
/*其它*/

#define Dummy_Byte 0xFF

/* WIP(busy)标志，FLASH内部正在写入 */
#define WIP_FLAG  0x01

#define SPI_FLASH_PER_WRITE_PAGE_SIZE 256
#define SPI_FLASH_PAGE_SIZE   0x1000

static void spi_flash_cs_low(void)
{
    HAL_GPIO_WritePin(SPIFLASH_CS_GPIO_Port, SPIFLASH_CS_Pin, 0);
}

static void spi_flash_cs_high(void)
{
    HAL_GPIO_WritePin(SPIFLASH_CS_GPIO_Port, SPIFLASH_CS_Pin, 1);
}

//读取flashID
uint32_t spi_flash_read_id(void)
{
    uint8_t temp[4] = {0};
    uint8_t send[4] = {W25X_JedecDeviceID, Dummy_Byte, Dummy_Byte, Dummy_Byte};
    HAL_StatusTypeDef status = HAL_OK;
    
    spi_flash_cs_low();
    status = HAL_SPI_TransmitReceive(&hspi1, send, temp, 4, 1000);
    spi_flash_cs_high();
    
    if(status != HAL_OK)
    {
        debug_info("%s %d error\r\n",__FUNCTION__,__LINE__);
        return 0xffff;
    }
    return temp[1] << 16 | temp[2] << 8 | temp[3];
}

//读取flash设备ID
uint32_t spi_flash_read_device_id(void)
{
    uint8_t temp[4] = {0};
    uint8_t send[4] = {W25X_DeviceID, Dummy_Byte, Dummy_Byte, Dummy_Byte};
    HAL_StatusTypeDef status = HAL_OK;
    
    spi_flash_cs_low();
    status = HAL_SPI_TransmitReceive(&hspi1, send, temp, 4, 1000);
    spi_flash_cs_high();
    
    if(status != HAL_OK)
    {
        debug_info("%s %d error\r\n",__FUNCTION__,__LINE__);
        return 0xffff;
    }
    return temp[3];
}

static uint8_t spi_flash_write_enable()
{
    HAL_StatusTypeDef status = HAL_OK;
    uint8_t cmd = W25X_WriteEnable;
    
    spi_flash_cs_low();
    status = HAL_SPI_Transmit(&hspi1, &cmd, 1, 1000);
    spi_flash_cs_high();
    if(status != HAL_OK)
    {
        debug_info("%s %d error\r\n",__FUNCTION__,__LINE__);
        return 0x01;
    }
    return 0;
}

//flash芯片掉电
uint8_t spi_flash_powerdown(void)
{
    HAL_StatusTypeDef status = HAL_OK;
    uint8_t cmd = W25X_PowerDown;
    
    spi_flash_cs_low();
    status = HAL_SPI_Transmit(&hspi1, &cmd, 1, 1000);
    spi_flash_cs_high();
    if(status != HAL_OK)
    {
        debug_info("%s %d error\r\n",__FUNCTION__,__LINE__);
        return 0x01;
    }
    return 0;
}

//芯片唤醒
uint8_t spi_flash_wakeup(void)
{
    HAL_StatusTypeDef status = HAL_OK;
    uint8_t cmd = W25X_ReleasePowerDown;
    
    spi_flash_cs_low();
    status = HAL_SPI_Transmit(&hspi1, &cmd, 1, 1000);
    spi_flash_cs_high();
    if(status != HAL_OK)
    {
        debug_info("%s %d error\r\n",__FUNCTION__,__LINE__);
        return 0x01;
    }
    return 0;
}

static uint8_t spi_flash_wait_for_write_end(void)
{
    HAL_StatusTypeDef status = HAL_OK;
    uint8_t cmd = W25X_ReadStatusReg;
    uint8_t flash_status = 0;
    
    spi_flash_cs_low();
    
    status = HAL_SPI_Transmit(&hspi1, &cmd, 1, 1000);
    if(status != HAL_OK)
    {
        spi_flash_cs_high();
        debug_info("%s %d error\r\n",__FUNCTION__,__LINE__);
        return 1;
    }
        
    do
    {
        status = HAL_SPI_Receive(&hspi1, &flash_status, 1, 100);
        if(status != HAL_OK)
        {
            spi_flash_cs_high();
            debug_info("%s %d error\r\n",__FUNCTION__,__LINE__);
            return 1;
        }
    }while((flash_status & WIP_FLAG) == 1);
    
    spi_flash_cs_high();
    return 0;
}

uint8_t spi_flash_sector_erease(uint32_t sector_addr)
{
    HAL_StatusTypeDef status = HAL_OK;
    uint8_t cmd = W25X_SectorErase;
    uint8_t addr[3] = {sector_addr & 0xFF0000, sector_addr & 0xFF00, sector_addr & 0xFF};
    uint8_t ret = 0;
    
    ret = spi_flash_write_enable();
    if(ret)
    {
        debug_info("%s %d error\r\n",__FUNCTION__,__LINE__);
        goto cmd_fail;
    }
    
    ret = spi_flash_wait_for_write_end();
    if(ret)
    {
        debug_info("%s %d error\r\n",__FUNCTION__,__LINE__);
        goto cmd_fail;
    }
    
    spi_flash_cs_low();
    status = HAL_SPI_Transmit(&hspi1, &cmd, 1, 1000);
    if(status != HAL_OK)
    {
        debug_info("%s %d error\r\n",__FUNCTION__,__LINE__);
        goto cmd_fail;
    }
    
    status = HAL_SPI_Transmit(&hspi1, addr, 3, 1000);
    if(status != HAL_OK)
    {
        debug_info("%s %d error\r\n",__FUNCTION__,__LINE__);
        goto cmd_fail;
    }
    spi_flash_cs_high();
    
    ret = spi_flash_wait_for_write_end();
    if(ret)
    {
        debug_info("%s %d error\r\n",__FUNCTION__,__LINE__);
        goto cmd_fail;
    }
    
    return 0;
cmd_fail:
    spi_flash_cs_high();
    return 1;
}

static uint8_t spi_flash_page_write(uint8_t *buff, uint32_t write_addr, uint16_t len)
{
    uint8_t ret = 0;
    HAL_StatusTypeDef status = HAL_OK;
    uint8_t send[4] = {W25X_PageProgram, write_addr & 0xFF0000, write_addr & 0xFF00, write_addr & 0xFF};
    
    ret = spi_flash_write_enable();
    if(ret)
    {
        debug_info("%s %d error\r\n",__FUNCTION__,__LINE__);
        goto cmd_fail;
    }
    
    spi_flash_cs_low();
    
    status = HAL_SPI_Transmit(&hspi1, send, 4, 1000);
    if(status != HAL_OK)
    {
        debug_info("%s %d error\r\n",__FUNCTION__,__LINE__);
        goto cmd_fail;
    }
    
    if(len > SPI_FLASH_PER_WRITE_PAGE_SIZE)
    {
        len = SPI_FLASH_PER_WRITE_PAGE_SIZE;
        debug_info("spi_flash_page_write too large\r\n");
    }
    
    status = HAL_SPI_Transmit(&hspi1, buff, len, 1000);
    if(status != HAL_OK)
    {
        debug_info("%s %d error\r\n",__FUNCTION__,__LINE__);
        goto cmd_fail;
    }
    spi_flash_cs_high();
    
    ret = spi_flash_wait_for_write_end();
    if(ret)
    {
        debug_info("%s %d error\r\n",__FUNCTION__,__LINE__);
        goto cmd_fail;
    }
    return 0;
cmd_fail:
    spi_flash_cs_high();
    return 1;
}

uint8_t spi_flash_buffer_write(uint8_t *buff, uint32_t write_addr, uint16_t len)
{
    uint8_t ret = 0;
    uint8_t num_of_page = 0, num_of_single = 0, addr = 0, count = 0, temp = 0;
    /* 数据地址是否页对齐 */
    addr = write_addr % SPI_FLASH_PAGE_SIZE;
    /* 差count 个字节可以对其页地址 */
    count = SPI_FLASH_PAGE_SIZE - addr;
    /* 可以写多少整页 */
    num_of_page = len / SPI_FLASH_PAGE_SIZE;
    /* 不满一页的数据长度 */
    num_of_single = len % SPI_FLASH_PAGE_SIZE;
    
    /* 若刚好页对齐 */
    if(addr == 0)
    {
        /* 若写入数据长度不足整页 */
        if(num_of_page == 0)
        {
            ret = spi_flash_page_write(buff, write_addr, len);
            if(ret)
            {
                debug_info("%s %d error\r\n",__FUNCTION__,__LINE__);
                return 1;
            }
        }
        else/* 写入数据大于一整页 */
        {
            /* 写入整页数据 */
            while(num_of_page--)
            {
                ret = spi_flash_page_write(buff, write_addr, SPI_FLASH_PAGE_SIZE);
                if(ret)
                {
                    debug_info("%s %d error\r\n",__FUNCTION__,__LINE__);
                    return 1;
                }
                write_addr += SPI_FLASH_PAGE_SIZE;
                buff += SPI_FLASH_PAGE_SIZE;
            }
            /* 写入不足整页的数据 */
            if(num_of_single)
            {
                ret = spi_flash_page_write(buff, write_addr, num_of_single);
                if(ret)
                {
                    debug_info("%s %d error\r\n",__FUNCTION__,__LINE__);
                    return 1;
                }
            }
        }
    }
    else/* 若写入地址未对其整页 */
    {
        /* 若写入数据长度不足一页 */
        if(num_of_page == 0)
        {
            /* 若当前页写不完数据 */
            if(num_of_single > count)
            {
                /* 写满当前页剩余空间 */
                temp = num_of_single - count;
                ret = spi_flash_page_write(buff, write_addr, count);
                if(ret)
                {
                    debug_info("%s %d error\r\n",__FUNCTION__,__LINE__);
                    return 1;
                }
                write_addr += count;
                buff += count;
                /* 写入剩余数据 */
                ret = spi_flash_page_write(buff, write_addr, temp);
                if(ret)
                {
                    debug_info("%s %d error\r\n",__FUNCTION__,__LINE__);
                    return 1;
                }
            }
            else/* 若当前页可写完数据则全部写入 */
            {
                ret = spi_flash_page_write(buff, write_addr, len);
                if(ret)
                {
                    debug_info("%s %d error\r\n",__FUNCTION__,__LINE__);
                    return 1;
                }
            }
        }
        else/* 若数据长度超过一页 */
        {
            /* 计算地址对齐前的数据长度 */
            len -= count;
            num_of_page = len / SPI_FLASH_PAGE_SIZE;
            num_of_single = len % SPI_FLASH_PAGE_SIZE;
            /* 先写满当前页剩余空间 */
            ret = spi_flash_page_write(buff, write_addr, count);
            if(ret)
            {
                debug_info("%s %d error\r\n",__FUNCTION__,__LINE__);
                return 1;
            }
            /* 对齐地址 */
            write_addr += count;
            buff += count;
            /* 写入整页数据 */
            while(num_of_page--)
            {
                ret = spi_flash_page_write(buff, write_addr, SPI_FLASH_PAGE_SIZE);
                if(ret)
                {
                    debug_info("%s %d error\r\n",__FUNCTION__,__LINE__);
                    return 1;
                }
                write_addr += SPI_FLASH_PAGE_SIZE;
                buff += SPI_FLASH_PAGE_SIZE;
            }
            /* 写入剩余数据 */
            if(num_of_single)
            {
                ret = spi_flash_page_write(buff, write_addr, num_of_single);
                if(ret)
                {
                    debug_info("%s %d error\r\n",__FUNCTION__,__LINE__);
                    return 1;
                }
            }
        }
    }
    
    return 0;
}

uint8_t spi_flash_buffer_read(uint8_t *buff, uint32_t read_addr, uint16_t len)
{
    HAL_StatusTypeDef status = HAL_OK;
    uint8_t send[4] = {W25X_ReadData, read_addr & 0xFF0000, read_addr & 0xFF00, read_addr & 0xFF};
    
    spi_flash_cs_low();
    status = HAL_SPI_Transmit(&hspi1, send, 4, 100);
    if(status != HAL_OK)
    {
        debug_info("%s %d error\r\n",__FUNCTION__,__LINE__);
        goto cmd_fail;
    }
    status = HAL_SPI_Receive(&hspi1, buff, len, 1000);
    if(status != HAL_OK)
    {
        debug_info("%s %d error\r\n",__FUNCTION__,__LINE__);
        goto cmd_fail;
    }
    
    spi_flash_cs_high();
    return 0;
    
cmd_fail:
    spi_flash_cs_high();
    return 1;

}

//芯片擦除
uint8_t spi_flash_bulk_erase(void)
{
    HAL_StatusTypeDef status = HAL_OK;
    uint8_t cmd = W25X_ChipErase, ret = 0;
    
    spi_flash_cs_low();
    status = HAL_SPI_Transmit(&hspi1, &cmd, 1, 100);
    spi_flash_cs_high();
    if(status != HAL_OK)
    {
        debug_info("%s %d error\r\n",__FUNCTION__,__LINE__);
        goto cmd_fail;
    }
    ret = spi_flash_wait_for_write_end();
    if(ret)
    {
        debug_info("%s %d error\r\n",__FUNCTION__,__LINE__);
        goto cmd_fail;
    }
cmd_fail:
    return 1;
}



//flash读写测试
uint8_t spi_flash_test(void)
{
    uint8_t ret = 0;
    uint8_t data_w[100] = {0};
    uint8_t data_r[100] = {0};
    uint32_t id = spi_flash_read_id();
    if(id == 0xffff)
    {
        debug_info("read flash id fail\r\n");
        return 1;
    }
    else
    {
        debug_info("spi flash id:0x%X   \r\n", id);
    }
    for(int i = 0; i < 100; i++)
    {
        data_w[i] = i;
    }
    ret = spi_flash_sector_erease(0x00);
    if(ret)
    {
        debug_info("flash erease sector fail\r\n");
        return 1;
    }
    ret = spi_flash_buffer_write(data_w, 0x00, 100);
    if(ret)
    {
        debug_info("write flash fail\r\n");
        return 1;
    }
    ret = spi_flash_buffer_read(data_r, 0x00, 100);
    if(ret)
    {
        debug_info("read flash fail\r\n");
        return 1;
    }
    for(int i = 0; i < 100; i++)
    {
        if(data_r[i] != data_w[i])
        {
            debug_info("flash data inconformity w[%d]:0x%x    r[%d]:0x%x\r\n",i,data_w[i],i,data_r[i]);
            return 1;
        }
    }
    debug_info("spi flash test ok\r\n");
    return 0;
}