#include "fatfs_test.h"
#include "bsp_usart.h"
#include "string.h"
#include "bsp_spi_flash.h"

static FATFS fs;                         /* 文件系统对象 */

uint8_t fs_work_buff[FF_MAX_SS] = {0};
static BYTE buffer[256]= {0};           /* 读缓冲区 */
static BYTE textFileBuffer[] =           /* 写缓冲区*/
     //"欢迎使用野火STM32开发板，今天是个好日子，新建文件系统测试文件\r\n";
"1234\r\n";
static FIL fnew;                         /* 文件对象 */
static FRESULT res_flash;                /* 文件操作结果 */
static UINT fnum;                        /* 文件成功读写数量 */


#define TEST_FILE_NAME "1:4321"

uint8_t fatfs_test(void)
{
    //挂载设备1(spi_flash)的文件系统并立刻初始化
    res_flash = f_mount(&fs, "1:", 1);
    //若没有文件系统，进行格式化并创建文件系统
    if(res_flash == FR_NO_FILESYSTEM)
//    if(1)
    {
        res_flash = f_mkfs("1:", 0, fs_work_buff, FF_MAX_SS);
        if(res_flash == FR_OK)
        {
            debug_info("flash file system format ok\r\n");
            /* 格式化后先取消挂载 */
            res_flash = f_mount(NULL, "1:", 1);
            /* 重新挂载 */
            res_flash = f_mount(&fs, "1:", 1);
            if(res_flash != FR_OK)
            {
                debug_info("flash file system mount fail %d \r\n", res_flash);
                return 1;
            }
            else
            {
                debug_info("flash file system mount ok\r\n");
            }
        }
        else
        {
            debug_info("flash file system format fail\r\n");
            return 1;
        }
    }
    else if(res_flash == FR_OK)
    {
        debug_info("flash file system mount ok\r\n");
    }
    else
    {
        debug_info("flash file system mount fail %d \r\n", res_flash);
        return 1;
    }
    /* 打开或创建一个文件 写入数据*/
    res_flash = f_open(&fnew,TEST_FILE_NAME, FA_CREATE_ALWAYS | FA_WRITE);
    if(res_flash == FR_OK)
    {
        debug_info("open file %s to write ok\r\n", TEST_FILE_NAME);
        /* 写文件 */
        res_flash = f_write(&fnew, textFileBuffer, sizeof(textFileBuffer), &fnum);
        if(res_flash == FR_OK)
        {
            debug_info("file %s write ok, size:%d\r\n",TEST_FILE_NAME, fnum);
            debug_info(":\r\n%s\r\n",textFileBuffer);
            /* 关闭文件 */
            res_flash = f_close(&fnew);
            if(res_flash != FR_OK)
            {
                debug_info("close file %s to write fail %d\r\n", TEST_FILE_NAME, res_flash);
                goto fail_mount;
            }
            debug_info("file %s close\r\n", TEST_FILE_NAME);
        }
        else
        {
            debug_info("file %s write fail %d\r\n", TEST_FILE_NAME, res_flash);
            goto fail_file;
        }
    }
    else
    {
        debug_info("open file %s to write fail %d\r\n", TEST_FILE_NAME, res_flash);
        goto fail_mount;
    }
    
    /* 打开一个文件 读取数据 */
    res_flash = f_open(&fnew, TEST_FILE_NAME, FA_OPEN_EXISTING | FA_READ);
    if(res_flash == FR_OK)
    {
        debug_info("open file %s to read ok\r\n", TEST_FILE_NAME);
        /* 读文件 */
        res_flash = f_read(&fnew, buffer, sizeof(buffer), &fnum);
        if(res_flash == FR_OK)
        {
            debug_info("file %s read ok, size:%d\r\n", TEST_FILE_NAME, fnum);
            debug_info(":\r\n%s\r\n",buffer);
            /* 关闭文件 */
            res_flash = f_close(&fnew);
            if(res_flash != FR_OK)
            {
                debug_info("close file %s to read fail %d\r\n", TEST_FILE_NAME, res_flash);
                goto fail_mount;
            }
            debug_info("file %s close\r\n", TEST_FILE_NAME);
        }
        else
        {
            debug_info("file %s read fail %d\r\n", TEST_FILE_NAME, res_flash);
            goto fail_file;
        }
    }
    else
    {
        debug_info("open file %s to read fail %d\r\n", TEST_FILE_NAME, res_flash);
        goto fail_mount;
    }
    
    /* 比较内容 */
    if(strcmp((const char *)textFileBuffer, (const char *)buffer) != 0)
    {
        debug_info("Error in %s content\r\n", TEST_FILE_NAME);
        goto fail_mount;
    }
    else
    {
        debug_info("fatfs test ok\r\n");
    }
    f_mount(NULL, "1:", 1);
    return 0;
fail_file:
    /* 关闭文件 */
    f_close(&fnew);
fail_mount:
    /* 取消挂载文件系统 */
    f_mount(NULL, "1:", 1);
    return 1;
}


