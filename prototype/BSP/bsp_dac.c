#include "bsp_dac.h"

#define POINT_NUM                       32
#define DAC_CHANNEL_GPIO_PORT           GPIOA
#define DAC_CHANNEL_PIN                 GPIO_PIN_5
#define DAC_CHANNEL_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOA_CLK_ENABLE()

#define DAC_DMA_CHANNELn                DMA2_Channel4
#define DAC_DMA_CHANNELn_IRQn           DMA2_Channel4_5_IRQn
#define DAC_DMA_CHANNELn_IRQHANDLER     DMA2_Channel4_5_IRQHandler
#define DAC_CHANNEL                     DAC_CHANNEL_2

DAC_HandleTypeDef hdac;
DMA_HandleTypeDef hdma_dac;
TIM_HandleTypeDef htim;


uint16_t wave[POINT_NUM] = {2048, 2460, 2856, 3218, 3532, 3786, 3969, 4072, 4093, 4031, 3887, 3668,
3382, 3042, 2661, 2255, 1841, 1435, 1054, 714, 428, 209, 65, 3, 24, 127,
310, 564, 878, 1240, 1636, 2048};



void HAL_DAC_MspInit(DAC_HandleTypeDef *DAC_InitStructure)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    __HAL_RCC_DAC_CLK_ENABLE();
    __HAL_RCC_DMA2_CLK_ENABLE();
    DAC_CHANNEL_GPIO_CLK_ENABLE();
    
    GPIO_InitStruct.Pin = DAC_CHANNEL_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(DAC_CHANNEL_GPIO_PORT, &GPIO_InitStruct);
    
    hdma_dac.Instance = DAC_DMA_CHANNELn;
    hdma_dac.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_dac.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_dac.Init.MemInc = DMA_MINC_ENABLE;
    hdma_dac.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_dac.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma_dac.Init.Mode = DMA_CIRCULAR;
    hdma_dac.Init.Priority = DMA_PRIORITY_HIGH;
    HAL_DMA_Init(&hdma_dac);
    __HAL_LINKDMA(&hdac, DMA_Handle2, hdma_dac);
}

void dac_config(void)
{
    DAC_ChannelConfTypeDef sConfig;
    TIM_MasterConfigTypeDef sMasterConfig;
    
    __HAL_RCC_TIM6_CLK_ENABLE();
    HAL_NVIC_SetPriority(DAC_DMA_CHANNELn_IRQn, 2, 2);
    HAL_NVIC_EnableIRQ(DAC_DMA_CHANNELn_IRQn);
    
    hdac.Instance = DAC;
    HAL_DAC_Init(&hdac);
    
    sConfig.DAC_Trigger = DAC_TRIGGER_T6_TRGO;
    sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_DISABLE;
    HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL);
    
    htim.Instance = TIM6;
    htim.Init.Prescaler = 71;
    htim.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim.Init.Period = 1000;
    HAL_TIM_Base_Init(&htim);
    
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    HAL_TIMEx_MasterConfigSynchronization(&htim, &sMasterConfig);
}


void dac_sin_test(void)
{
    dac_config();
    HAL_TIM_Base_Start(&htim);
    HAL_DAC_Start_DMA(&hdac,DAC_CHANNEL, (uint32_t *)wave, POINT_NUM, DAC_ALIGN_12B_R);
}

