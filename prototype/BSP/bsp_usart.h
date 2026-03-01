#ifndef __BSP_USART_H__
#define __BSP_USART_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_cortex.h"

void The_UART_Callback(UART_HandleTypeDef *huart);


#ifdef __cplusplus
}
#endif

#endif
