#ifndef __BSP_EXTI_H__
#define __BSP_EXTI_H__

#ifdef __cplusplus
extern "C" {
#endif
    
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_cortex.h"

#define KEY1_INT_GPIO_PORT      GPIOA
#define KEY1_INT_GPIO_PIN       GPIO_PIN_0
#define KEY1_EXTI_IRQ           EXTI0_IRQn
#define KEY1_IRQHandler         EXTI0_IRQHandler

#define KEY2_INT_GPIO_PORT      GPIOC
#define KEY2_INT_GPIO_PIN       GPIO_PIN_13
#define KEY2_EXTI_IRQ           EXTI15_10_IRQn
#define KEY2_IRQHandler         EXTI15_10_IRQHandler

void EXTI_Key_Config(void);

#ifdef __cplusplus
}
#endif

#endif
