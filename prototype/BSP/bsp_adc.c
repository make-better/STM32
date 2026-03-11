#include "bsp_adc.h"
#include "bsp_usart.h"


#define NUMOFCHANNEL 6
#define ADC_PIN1        GPIO_PIN_0
#define ADC_CHANNEL1    ADC_CHANNEL_10

#define ADC_PIN2        GPIO_PIN_1
#define ADC_CHANNEL2    ADC_CHANNEL_11

#define ADC_PIN3        GPIO_PIN_2
#define ADC_CHANNEL3    ADC_CHANNEL_12

#define ADC_PIN4        GPIO_PIN_3
#define ADC_CHANNEL4    ADC_CHANNEL_13

#define ADC_PIN5        GPIO_PIN_4
#define ADC_CHANNEL5    ADC_CHANNEL_14

#define ADC_PIN6        GPIO_PIN_5
#define ADC_CHANNEL6    ADC_CHANNEL_15


#define ADC_GPIO_PORT   GPIOC
#define ADC_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOC_CLK_ENABLE()
#define ADC_CLK_ENABLE()        __HAL_RCC_ADC1_CLK_ENABLE()

#define ADC_DMA_CLK_ENABLE()   __HAL_RCC_DMA1_CLK_ENABLE()
#define ADC_DMA_STREAM         DMA1_Channel1


extern ADC_HandleTypeDef hadc1;

DMA_HandleTypeDef hdma_adcx;

__IO uint16_t adc_converted_value[NUMOFCHANNEL] = {0};

//void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
//{
//    adc_converted_value = HAL_ADC_GetValue(hadc);
//}

static void adc_gpio_config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    ADC_CLK_ENABLE();
    ADC_GPIO_CLK_ENABLE();
    
    GPIO_InitStructure.Pin = ADC_PIN1|ADC_PIN2|ADC_PIN3|ADC_PIN4|ADC_PIN5|ADC_PIN6;
    GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(ADC_GPIO_PORT, &GPIO_InitStructure);
}

static void adc_mode_config(void)
{
    ADC_ChannelConfTypeDef ADC_Config;
    
    ADC_DMA_CLK_ENABLE();
    
    hdma_adcx.Instance = ADC_DMA_STREAM;
    hdma_adcx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_adcx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_adcx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_adcx.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_adcx.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma_adcx.Init.Mode = DMA_CIRCULAR;
    hdma_adcx.Init.Priority = DMA_PRIORITY_MEDIUM;
    
    HAL_DMA_Init(&hdma_adcx);
    
    __HAL_LINKDMA(&hadc1, DMA_Handle, hdma_adcx);
    
    hadc1.Instance = ADC1;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.ScanConvMode = ENABLE;
    hadc1.Init.ContinuousConvMode = ENABLE;
    hadc1.Init.NbrOfConversion = NUMOFCHANNEL;
    hadc1.Init.DiscontinuousConvMode = DISABLE;
    hadc1.Init.NbrOfDiscConversion = 0;
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    
    HAL_ADC_Init(&hadc1);
    
    ADC_Config.Channel = ADC_CHANNEL1;
    ADC_Config.Rank = 1;
    ADC_Config.SamplingTime = ADC_SAMPLETIME_55CYCLES_5;
    HAL_ADC_ConfigChannel(&hadc1, &ADC_Config);
    
    ADC_Config.Channel = ADC_CHANNEL2;
    ADC_Config.Rank = 2;
    HAL_ADC_ConfigChannel(&hadc1, &ADC_Config);
    
    ADC_Config.Channel = ADC_CHANNEL3;
    ADC_Config.Rank = 3;
    HAL_ADC_ConfigChannel(&hadc1, &ADC_Config);
    
    ADC_Config.Channel = ADC_CHANNEL4;
    ADC_Config.Rank = 4;
    HAL_ADC_ConfigChannel(&hadc1, &ADC_Config);
    
    ADC_Config.Channel = ADC_CHANNEL5;
    ADC_Config.Rank = 5;
    HAL_ADC_ConfigChannel(&hadc1, &ADC_Config);
    
    ADC_Config.Channel = ADC_CHANNEL6;
    ADC_Config.Rank = 6;
    HAL_ADC_ConfigChannel(&hadc1, &ADC_Config);
    
    HAL_ADC_Start_DMA(&hadc1, (uint32_t *)&adc_converted_value, 6);
}

void adc_init(void)
{
    adc_gpio_config();
    adc_mode_config();
}

    
void show_adc_value(void)
{
    for(int i = 0; i < NUMOFCHANNEL; i++)
    {
        debug_info("adc[%d]:%.2fV\r\n",i, (float)adc_converted_value[i]/4096*3.3);
    }
    debug_info("\r\n");
}