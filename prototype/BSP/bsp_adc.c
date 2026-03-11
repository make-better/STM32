#include "bsp_adc.h"
#include "bsp_usart.h"


#define RHEOSTAT_ADC_DMA_CLK_ENABLE()   __HAL_RCC_DMA1_CLK_ENABLE()
#define RHEOSTAT_ADC_DMA_STREAM         DMA1_Channel1


extern ADC_HandleTypeDef hadc1;

uint16_t adc_converted_value = 0;

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    adc_converted_value = HAL_ADC_GetValue(hadc);
}
    

void show_adc_value(void)
{
    debug_info("adc1_11:%.2fV\r\n",(float)adc_converted_value/4096*3.3);
}