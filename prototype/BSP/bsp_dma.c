#include "bsp_dma.h"
#include "stdio.h"

DMA_HandleTypeDef DMA_Handle;

const uint32_t aSRC_Const_Buffer[BUFFER_SIZE] = {
    0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
    0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,
    0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,
    0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F
};

uint32_t aDST_Buffer[BUFFER_SIZE] = {0};

void DMA_Config(void)
{
    HAL_StatusTypeDef DMA_Status = HAL_ERROR;
    
    DMA_STREAM_CLKCK();
    DMA_Handle.Instance = DMA_STREAM;
    DMA_Handle.Init.Direction = DMA_MEMORY_TO_MEMORY;
    DMA_Handle.Init.PeriphInc = DMA_PINC_ENABLE;
    DMA_Handle.Init.MemInc = DMA_MINC_ENABLE;
    DMA_Handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    DMA_Handle.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    DMA_Handle.Init.Mode = DMA_NORMAL;
    DMA_Handle.Init.Priority = DMA_PRIORITY_MEDIUM;
    
    HAL_DMA_Init(&DMA_Handle);
    DMA_Status = HAL_DMA_Start(&DMA_Handle, (uint32_t)aSRC_Const_Buffer, (uint32_t)aDST_Buffer,BUFFER_SIZE);
    if(DMA_Status != HAL_OK)
    {
        printf("dma fail\r\n");
    }
}

uint8_t BufferCmp(uint32_t *pbuffer1, uint32_t *pbuffer2, uint16_t length)
{
    while(length--)
    {
        if(*pbuffer1 != *pbuffer2)
        {
            return 1;
        }
        pbuffer1++;
        pbuffer2++;
    };
    return 0;
}

uint16_t DMA_Test(void)
{
    DMA_Config();
    while (__HAL_DMA_GET_FLAG(&DMA_Handle,DMA_FLAG_TC6)==DISABLE) {
     }
    return BufferCmp((uint32_t*)aSRC_Const_Buffer, (uint32_t*)aDST_Buffer, BUFFER_SIZE);
}