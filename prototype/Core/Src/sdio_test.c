#include "sdio_test.h"
#include "bsp_sdio_sdcard.h"
#include "bsp_usart.h"

#define BLOCK_START_ADDR    0
#define NUM_OF_BLOCKS       5
#define BUFFER_WORDS_SIZE   ((BLOCKSIZE * NUM_OF_BLOCKS) >> 2)


uint32_t aTxBuffer[BUFFER_WORDS_SIZE];
uint32_t aRxBuffer[BUFFER_WORDS_SIZE];

static void Fill_Buffer(uint32_t *pBuffer, uint32_t uwBufferLenght, uint32_t uwOffset);
static uint8_t Buffercmp(uint32_t* pBuffer1, uint32_t* pBuffer2, uint16_t BufferLength);


void SD_Test(void)
{
    uint8_t sd_state = MSD_OK;
    
    sd_state = BSP_SD_Init();
    if(sd_state != MSD_OK)
    {
        debug_info("sd init failed\r\n");
        return;
    }
    
    debug_info("sd init ok \r\n");
    
    sd_state = BSP_SD_Erase(BLOCK_START_ADDR, NUM_OF_BLOCKS);
    while(BSP_SD_GetCardState() != SD_TRANSFER_OK);
    
    if(sd_state != MSD_OK)
    {
        debug_info("sd erase failed\r\n");
        return;
    }
    debug_info("sd erase ok\r\n");
    Fill_Buffer(aTxBuffer, BUFFER_WORDS_SIZE, 0x22ff);
    sd_state = BSP_SD_WriteBlocks(aTxBuffer, BLOCK_START_ADDR, NUM_OF_BLOCKS, 10);
    while(BSP_SD_GetCardState() != SD_TRANSFER_OK)
    {
        HAL_Delay(1);
    }
    if(sd_state != MSD_OK)
    {
        debug_info("sd write failed\r\n");
        return;
    }
    debug_info("sd write ok\r\n");
    
    sd_state = BSP_SD_ReadBlocks(aRxBuffer, BLOCK_START_ADDR, NUM_OF_BLOCKS, 10);
    while(BSP_SD_GetCardState() != SD_TRANSFER_OK);
    if(sd_state != MSD_OK)
    {
        debug_info("sd read failed\r\n");
        return;
    }
    debug_info("sd read ok\r\n");
    
    if(Buffercmp(aTxBuffer, aRxBuffer, BUFFER_WORDS_SIZE) > 0)
    {
        debug_info("sd compare failed\r\n");
        return;
    }
    debug_info("sd compare ok\r\n");
}

static void Fill_Buffer(uint32_t *pBuffer, uint32_t uwBufferLenght, uint32_t uwOffset)
{
  uint32_t tmpIndex = 0;
  
  /* Put in global buffer different values */
  for (tmpIndex = 0; tmpIndex < uwBufferLenght; tmpIndex++ )
  {
    pBuffer[tmpIndex] = tmpIndex + uwOffset;
  }
}

static uint8_t Buffercmp(uint32_t* pBuffer1, uint32_t* pBuffer2, uint16_t BufferLength)
{
  while (BufferLength--)
  {
    if (*pBuffer1 != *pBuffer2)
    {
      return 1;
    }
    
    pBuffer1++;
    pBuffer2++;
  }
  return 0;
}