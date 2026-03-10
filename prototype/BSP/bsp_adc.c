#include "bsp_adc.h"

#define RHEOSTAT_ADC_DMA_CLK_ENABLE()   __HAL_RCC_DMA1_CLK_ENABLE()
#define RHEOSTAT_ADC_DMA_STREAM         DMA1_Channel1

uint16_t adc_converted_value = 0;

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    adc_converted_value = HAL_ADC_GetValue(hadc);
}
    