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


#define ADCx_GPIO_PORT   GPIOC
#define ADCx_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOC_CLK_ENABLE()


#define ADCx1_CLK_ENABLE()        __HAL_RCC_ADC1_CLK_ENABLE()
#define ADCx1_CLK_DISABLE()       __HAL_RCC_ADC1_CLK_DISABLE()

#define ADCx1_DMA_CLK_ENABLE()          __HAL_RCC_DMA1_CLK_ENABLE()
#define ADCx1_DMA_Channeln              DMA1_Channel1
#define ADCx1_DMA_Channeln_IRQn         DMA1_Channel1_IRQn
#define ADCx1_DMA_Channeln_IRQ_HANDLER  DMA1_Channel1_IRQHandler

#define ADCx2_CLK_ENABLE()        __HAL_RCC_ADC2_CLK_ENABLE()
#define ADCx2_CLK_DISABLE()       __HAL_RCC_ADC2_CLK_DISABLE()





extern ADC_HandleTypeDef hadc1;
ADC_HandleTypeDef hadc2;

DMA_HandleTypeDef hdma_adcx1;

__IO uint16_t adc_converted_value[NUMOFCHANNEL] = {0};

uint32_t adc_converted_vlaue_double = 0;

//void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
//{
//    adc_converted_value = HAL_ADC_GetValue(hadc);
//}

static void adc_independent_gpio_config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    ADCx1_CLK_ENABLE();
    ADCx_GPIO_CLK_ENABLE();
    
    GPIO_InitStructure.Pin = ADC_PIN1|ADC_PIN2|ADC_PIN3|ADC_PIN4|ADC_PIN5|ADC_PIN6;
    GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(ADCx_GPIO_PORT, &GPIO_InitStructure);
}

static void adc_independent_mode_config(void)
{
    ADC_ChannelConfTypeDef ADC_Config;
    
    ADCx1_DMA_CLK_ENABLE();
    
    hdma_adcx1.Instance = ADCx1_DMA_Channeln;
    hdma_adcx1.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_adcx1.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_adcx1.Init.MemInc = DMA_MINC_ENABLE;
    hdma_adcx1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_adcx1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma_adcx1.Init.Mode = DMA_CIRCULAR;
    hdma_adcx1.Init.Priority = DMA_PRIORITY_MEDIUM;
    
    HAL_DMA_Init(&hdma_adcx1);
    
    __HAL_LINKDMA(&hadc1, DMA_Handle, hdma_adcx1);
    
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

void adc_independent_init(void)
{
    adc_independent_gpio_config();
    adc_independent_mode_config();
}

//双重ADC同步规则模式采集GPIO/DMA配置
static void adc_double_gpio_config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    ADCx1_CLK_ENABLE();
    ADCx2_CLK_ENABLE();
    ADCx_GPIO_CLK_ENABLE();
    ADCx1_DMA_CLK_ENABLE();
    
    GPIO_InitStructure.Pin = ADC_PIN2;
    GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(ADCx_GPIO_PORT, &GPIO_InitStructure);
    
    hdma_adcx1.Instance = ADCx1_DMA_Channeln;
    hdma_adcx1.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_adcx1.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_adcx1.Init.MemInc = DMA_MINC_DISABLE;
    hdma_adcx1.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hdma_adcx1.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    hdma_adcx1.Init.Mode = DMA_CIRCULAR;
    hdma_adcx1.Init.Priority = DMA_PRIORITY_HIGH;
    
    HAL_DMA_Init(&hdma_adcx1);
    
    __HAL_LINKDMA(&hadc1, DMA_Handle, hdma_adcx1);
    
    HAL_NVIC_SetPriority(ADCx1_DMA_Channeln_IRQn, 2, 3);
    HAL_NVIC_EnableIRQ(ADCx1_DMA_Channeln_IRQn);
    
}

//双重ADC同步规则ADC配置
static void adc_double_mode_config(void)
{
    ADC_MultiModeTypeDef multimode;
    ADC_ChannelConfTypeDef sConfig;
    
    //ADC1
    hadc1.Instance = ADC1;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.ScanConvMode = ENABLE;
    hadc1.Init.ContinuousConvMode = ENABLE;
    hadc1.Init.NbrOfConversion = 1;
    hadc1.Init.DiscontinuousConvMode = DISABLE;
    hadc1.Init.NbrOfDiscConversion = 0;
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    HAL_ADC_Init(&hadc1);
    
    multimode.Mode = ADC_DUALMODE_INTERLSLOW;
    HAL_ADCEx_MultiModeConfigChannel(&hadc1, &multimode);
    
    //采集通道
    sConfig.Channel = ADC_CHANNEL2;
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_55CYCLES_5;
    HAL_ADC_ConfigChannel(&hadc1, &sConfig);
    
    //ADC2
    hadc2.Instance = ADC2;
    hadc2.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc2.Init.ScanConvMode = DISABLE;
    hadc2.Init.ContinuousConvMode = ENABLE;
    hadc2.Init.NbrOfConversion = 1;
    hadc2.Init.DiscontinuousConvMode = DISABLE;
    hadc2.Init.NbrOfDiscConversion = 0;
    hadc2.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    HAL_ADC_Init(&hadc2);
    
    multimode.Mode = ADC_DUALMODE_INTERLSLOW;
    HAL_ADCEx_MultiModeConfigChannel(&hadc2, &multimode);
    
    //采集通道
    sConfig.Channel = ADC_CHANNEL2;
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_55CYCLES_5;
    HAL_ADC_ConfigChannel(&hadc2, &sConfig);
}

void adc_double_init(void)
{
    adc_double_gpio_config();
    adc_double_mode_config();
    
    HAL_ADCEx_Calibration_Start(&hadc1);
    HAL_ADCEx_Calibration_Start(&hadc2);
    
    HAL_ADC_Start(&hadc1);
    HAL_ADC_Start(&hadc2);
    HAL_ADCEx_MultiModeStart_DMA(&hadc1, &adc_converted_vlaue_double, sizeof(adc_converted_vlaue_double));
}
void show_adc_independent_value(void)
{
    for(int i = 0; i < NUMOFCHANNEL; i++)
    {
        debug_info("adc[%d]:%.2fV\r\n",i, (float)adc_converted_value[i]/4096*3.3);
    }
    debug_info("\r\n");
}

void show_adc_double_value(void)
{
    float adc_converted_vlaue_local[2];
    adc_converted_vlaue_local[0] = (float)(adc_converted_vlaue_double&0xfff)/4096*3.3;
    adc_converted_vlaue_local[1] = (float)((adc_converted_vlaue_double >> 16)&0xfff)/4096*3.3;
    
    debug_info("adc1:%.2fV\r\n", adc_converted_vlaue_local[0]);
    debug_info("adc2:%.2fV\r\n", adc_converted_vlaue_local[1]);
 
    debug_info("\r\n");
}