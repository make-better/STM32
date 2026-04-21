#include "bsp_dma.h"
#include "stdio.h"
#include "main.h"
#include "bsp_usart.h"
DMA_HandleTypeDef DMA_Handle;
DMA_HandleTypeDef DMA_UART1_TX_Handle;
DMA_HandleTypeDef DMA_UART1_RX_Handle;


extern UART_HandleTypeDef huart1;
extern uint8_t rx_buff[UART1_BUFFER_SIZE];

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
    while (__HAL_DMA_GET_FLAG(&DMA_Handle,DMA_FLAG_TC1)==DISABLE) {
     }
    return BufferCmp((uint32_t*)aSRC_Const_Buffer, (uint32_t*)aDST_Buffer, BUFFER_SIZE);
}

void DMA_UART1_Config(void)
{
    __DMA1_CLK_ENABLE();
    
    DMA_UART1_TX_Handle.Instance = DMA1_Channel4;
    DMA_UART1_TX_Handle.Init.Direction = DMA_MEMORY_TO_PERIPH;
    DMA_UART1_TX_Handle.Init.PeriphInc = DMA_PINC_DISABLE;
    DMA_UART1_TX_Handle.Init.MemInc = DMA_MINC_ENABLE;
    DMA_UART1_TX_Handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    DMA_UART1_TX_Handle.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    DMA_UART1_TX_Handle.Init.Mode = DMA_NORMAL;
    DMA_UART1_TX_Handle.Init.Priority = DMA_PRIORITY_MEDIUM;
    
    if (HAL_DMA_Init(&DMA_UART1_TX_Handle) != HAL_OK)
    {
      Error_Handler();
    }
    __HAL_LINKDMA(&huart1, hdmatx, DMA_UART1_TX_Handle);
    HAL_NVIC_SetPriority(DMA1_Channel4_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel4_IRQn);
    
    DMA_UART1_RX_Handle.Instance = DMA1_Channel5;
    DMA_UART1_RX_Handle.Init.Direction = DMA_PERIPH_TO_MEMORY;
    DMA_UART1_RX_Handle.Init.PeriphInc = DMA_PINC_DISABLE;
    DMA_UART1_RX_Handle.Init.MemInc = DMA_MINC_ENABLE;
    DMA_UART1_RX_Handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    DMA_UART1_RX_Handle.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    DMA_UART1_RX_Handle.Init.Mode = DMA_CIRCULAR;
    DMA_UART1_RX_Handle.Init.Priority = DMA_PRIORITY_MEDIUM;
    
    if (HAL_DMA_Init(&DMA_UART1_RX_Handle) != HAL_OK)
    {
      Error_Handler();
    }
    __HAL_LINKDMA(&huart1, hdmarx, DMA_UART1_RX_Handle);
    
    HAL_UART_Receive_DMA(&huart1, rx_buff, UART1_BUFFER_SIZE);
    
    HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 1, 1);
    HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);
}