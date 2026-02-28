#ifndef __BSP_DMA_H__
#define __BSP_DMA_H__
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_cortex.h"

#define DMA_STREAM          DMA2_Channel1
#define DMA_CHANNEL         DMA2_Channel1
#define DMA_STREAM_CLKCK()  __DMA2_CLK_ENABLE();

#define BUFFER_SIZE         32

uint8_t BufferCmp(uint32_t *pbuffer1, uint32_t *pbuffer2, uint16_t length);
void DMA_Config(void);
uint16_t DMA_Test(void);
void DMA_UART1_Config(void);
#endif