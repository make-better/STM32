#ifndef __BSP_DAC_H__
#define __BSP_DAC_H__

#ifdef __cplusplus
extern "C" {
#endif
    
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_cortex.h"



void dac_config(void);
void dac_sin_test(void);
#ifdef __cplusplus
}
#endif

#endif