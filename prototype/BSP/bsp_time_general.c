#include "bsp_time_general.h"
#include "main.h"
#include "string.h"
#include "bsp_usart.h"

// 定时器预分频
#define GENERAL_TIM_PRESCALER               71

// 定时器周期
#define GENERAL_TIM_PERIOD                  0xFFFF

#define GENERAL_TIMx                        TIM5
#define GENERAL_TIM_RCC_CLK_ENABLE()        __HAL_RCC_TIM5_CLK_ENABLE()
#define GENERAL_TIM_RCC_CLK_DISABLE()       __HAL_RCC_TIM5_CLK_DISABLE()

#define GENERAL_TIM_GPIO_RCC_CLK_ENABLE()   __HAL_RCC_GPIOA_CLK_ENABLE()
#define GENERAL_TIM_CHANNELx                TIM_CHANNEL_1
#define GENERAL_TIM_IT_CCx                  TIM_IT_CC1

#define GENERAL_TIM_IRQn                    TIM5_IRQn
#define GENERAL_TIM_IRQHANDLER              TIM5_IRQHandler

#define KEY1_RCC_CLK_ENABLE()               __HAL_RCC_GPIOA_CLK_ENABLE()
#define KEY1_GPIO_PIN                       GPIO_PIN_0
#define KEY1_GPIO                           GPIOA

//测量的起始边沿
#define GENERAL_TIM_START_ICPolarity        TIM_INPUTCHANNELPOLARITY_RISING
//测量的结束边沿
#define GENERAL_TIM_END_ICPolarity          TIM_INPUTCHANNELPOLARITY_FALLING

TIM_HandleTypeDef htimx5;
STRUCT_CAPTURE TIM_ICUserValueStructure;
STRUCT_CAPTURE TIM_ICUserShow;

static uint32_t TIM_PscCLK;

static void general_timer_gpio_config(void)
{
    GPIO_InitTypeDef    GPIO_InitStruct;
    
    KEY1_RCC_CLK_ENABLE();
    
    GPIO_InitStruct.Pin = KEY1_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    HAL_GPIO_Init(KEY1_GPIO, &GPIO_InitStruct);
    
    HAL_NVIC_SetPriority(GENERAL_TIM_IRQn, 1, 1);
    HAL_NVIC_EnableIRQ(GENERAL_TIM_IRQn);
}

static void general_timer_mode_config(void)
{
    TIM_ClockConfigTypeDef CLKSource;
    TIM_MasterConfigTypeDef MasterConfig;
    TIM_IC_InitTypeDef ICConfig;
    
    GENERAL_TIM_RCC_CLK_ENABLE();
    
    htimx5.Instance = GENERAL_TIMx;
    htimx5.Init.Prescaler = GENERAL_TIM_PRESCALER;
    htimx5.Init.CounterMode = TIM_COUNTERMODE_UP;
    htimx5.Init.Period = GENERAL_TIM_PERIOD;
    htimx5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_Base_Init(&htimx5);
    
    CLKSource.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    HAL_TIM_ConfigClockSource(&htimx5, &CLKSource);
    
    MasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    MasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    HAL_TIMEx_MasterConfigSynchronization(&htimx5, &MasterConfig);
    
    ICConfig.ICPolarity = GENERAL_TIM_START_ICPolarity;
    ICConfig.ICSelection = TIM_ICSELECTION_DIRECTTI;
    ICConfig.ICPrescaler = TIM_ICPSC_DIV1;
    ICConfig.ICFilter = 0;
    HAL_TIM_IC_ConfigChannel(&htimx5, &ICConfig, GENERAL_TIM_CHANNELx);
}


void general_tim_ic_config(void)
{
    general_timer_mode_config();
    general_timer_gpio_config();
    
    TIM_PscCLK = HAL_RCC_GetHCLKFreq()/GENERAL_TIM_PRESCALER;
    
    HAL_TIM_Base_Start_IT(&htimx5);
    HAL_TIM_IC_Start_IT(&htimx5, GENERAL_TIM_CHANNELx);
}


void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    TIM_IC_InitTypeDef IC_Config;
    if(htim == &htimx5)
    {
        if(TIM_ICUserValueStructure.ucStartFlag == 0)
        {
            HAL_GPIO_WritePin(LED_B_GPIO_Port, LED_B_Pin, 1);
            __HAL_TIM_SET_COUNTER(&htimx5, 0);//清零定时器计数
            TIM_ICUserValueStructure.usPeriod = 0;
            TIM_ICUserValueStructure.usCtr = 0;
            
            //配置输入捕获参数，主要修改触发电平
            IC_Config.ICPolarity = GENERAL_TIM_END_ICPolarity;
            IC_Config.ICSelection = TIM_ICSELECTION_DIRECTTI;
            IC_Config.ICPrescaler = TIM_ICPSC_DIV1;
            IC_Config.ICFilter = 0;
            HAL_TIM_IC_ConfigChannel(&htimx5, &IC_Config, GENERAL_TIM_CHANNELx);
            
            //清除中断标志位
            __HAL_TIM_CLEAR_IT(&htimx5, GENERAL_TIM_IT_CCx);
            //启动输入捕获并开启中断
            HAL_TIM_IC_Start_IT(&htimx5, GENERAL_TIM_CHANNELx);
            TIM_ICUserValueStructure.ucStartFlag = 1;
        }
        else
        {
            HAL_GPIO_WritePin(LED_B_GPIO_Port, LED_B_Pin, 0);
            //获取定时器计数值
            TIM_ICUserValueStructure.usCtr = HAL_TIM_ReadCapturedValue(&htimx5, GENERAL_TIM_CHANNELx);
            //配置输入捕获参数，主要修改触发电平
            IC_Config.ICPolarity = GENERAL_TIM_START_ICPolarity;
            IC_Config.ICSelection = TIM_ICSELECTION_DIRECTTI;
            IC_Config.ICPrescaler = TIM_ICPSC_DIV1;
            IC_Config.ICFilter = 0;
            HAL_TIM_IC_ConfigChannel(&htimx5, &IC_Config, GENERAL_TIM_CHANNELx);
            //清除中断标志位
            __HAL_TIM_CLEAR_IT(&htimx5, GENERAL_TIM_IT_CCx);
            //启动输入捕获并开启中断
            HAL_TIM_IC_Start_IT(&htimx5, GENERAL_TIM_CHANNELx);
            TIM_ICUserValueStructure.ucStartFlag = 0;
            TIM_ICUserValueStructure.ucFinishFlag = 1;
            memcpy(&TIM_ICUserShow, &TIM_ICUserValueStructure, sizeof(STRUCT_CAPTURE));
        }
    }
}

void show_ic_value(void)
{
    uint32_t time;
    if(TIM_ICUserShow.ucFinishFlag)
    {
        TIM_ICUserShow.ucFinishFlag = 0;
        time = TIM_ICUserShow.usPeriod * GENERAL_TIM_PERIOD + TIM_ICUserShow.usCtr;
        debug_info("high vol time: %d.%d s\r\n",time/TIM_PscCLK, time % TIM_PscCLK);
    }
}






