#include "bsp_time_base.h"
#include "main.h"

#define BASIC_TIM                   TIM6
#define BASIC_TIM_RCC_CLK_ENABLE()  __HAL_RCC_TIM6_CLK_ENABLE()
#define BASIC_TIM_IRQn              TIM6_IRQn
#define BASIC_TIM_IRQhANDLER        TIM6_IRQHandler

#define BASIC_TIM_PRESCALER         71
#define BASIC_TIM_PERIOD            1000


TIM_HandleTypeDef htimx6;

static int16_t i = 0;

void basic_timx_init(void)
{
    TIM_MasterConfigTypeDef sMasterConfig;
    
    BASIC_TIM_RCC_CLK_ENABLE();
    
    htimx6.Instance = BASIC_TIM;
    htimx6.Init.Prescaler = BASIC_TIM_PRESCALER;
    htimx6.Init.CounterMode = TIM_COUNTERMODE_UP;
    htimx6.Init.Period = BASIC_TIM_PERIOD;
    
    HAL_TIM_Base_Init(&htimx6);
    
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    
    HAL_TIMEx_MasterConfigSynchronization(&htimx6, &sMasterConfig);
    
    HAL_NVIC_SetPriority(BASIC_TIM_IRQn, 2, 4);
    HAL_NVIC_EnableIRQ(BASIC_TIM_IRQn);
    
    HAL_TIM_Base_Start_IT(&htimx6);
}

void basic_timer_callback(void)
{
    i++;
    if(i == 1000)
    {
        i = 0;
        HAL_GPIO_TogglePin(LED_B_GPIO_Port, LED_B_Pin);
    }
}

