#ifndef __BSP_ADC_H__
#define __BSP_ADC_H__

#ifdef __cplusplus
extern "C" {
#endif
    
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_cortex.h"
    
void adc_independent_init(void);
void show_adc_independent_value(void);

void adc_double_init(void);
void show_adc_double_value(void);
#ifdef __cplusplus
}
#endif

#endif