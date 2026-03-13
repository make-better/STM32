#ifndef __BSP_TIM_BASE_H__
#define __BSP_TIM_BASE_H__

#ifdef __cplusplus
extern "C" {
#endif
    
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_cortex.h"
    
    
    
void basic_timx_init(void);
void basic_timer_callback(void);
#ifdef __cplusplus
}
#endif

#endif