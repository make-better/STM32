#include "bsp_time_advance.h"

#define ADVANCED_TIMx                       TIM8
#define ADVANCED_TIM_RCC_CLK_ENABLE()       __HAL_RCC_TIM8_CLK_ENABLE()
#define ADVANCED_TIM_RCC_CLK_DISABLE()      __HAL_RCC_TIM8_CLK_DISABLE()
#define ADVANCED_TIM_GPIO_RCC_CLK_ENBALE()  {\
                                            __HAL_RCC_GPIOA_CLK_ENABLE();\
                                            __HAL_RCC_GPIOC_CLK_ENABLE();}

#define ADVANCED_TIM_PRESCALER              (72 - 1)
#define ADVANCED_TIM_PERIOD                 1000
#define ADVANCED_TIM_REPETITIONCOUNTER      9
//TIM8 CH1
#define ADVANCED_OCPWM_PIN                  GPIO_PIN_6
#define ADVANCED_OCPWM_GPIO_PORT            GPIOC

//TIM8 CH1N
#define ADVANCED_OCNPWM_PIN                 GPIO_PIN_7
#define ADVANCED_OCNPWM_GPIO_PORT           GPIOA

TIM_HandleTypeDef htimx8;

void HAL_TIM_MspPostInit(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    
    ADVANCED_TIM_GPIO_RCC_CLK_ENBALE();
    
    GPIO_InitStruct.Pin = ADVANCED_OCPWM_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(ADVANCED_OCPWM_GPIO_PORT, &GPIO_InitStruct);
    
    GPIO_InitStruct.Pin = ADVANCED_OCNPWM_PIN;
    HAL_GPIO_Init(ADVANCED_OCNPWM_GPIO_PORT, &GPIO_InitStruct);
}

void ADVANCED_TIM_Mode_Config(void)
{
    TIM_ClockConfigTypeDef Clock_Config;
    TIM_MasterConfigTypeDef Master_Config;
    TIM_BreakDeadTimeConfigTypeDef BreakDeadTime_Config;
    TIM_OC_InitTypeDef OC_Config;
    
    ADVANCED_TIM_RCC_CLK_ENABLE();
    
    htimx8.Instance = ADVANCED_TIMx;
    htimx8.Init.Prescaler = ADVANCED_TIM_PRESCALER;
    htimx8.Init.CounterMode = TIM_COUNTERMODE_UP;
    htimx8.Init.Period = ADVANCED_TIM_PERIOD;
    htimx8.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htimx8.Init.RepetitionCounter = ADVANCED_TIM_REPETITIONCOUNTER;
    HAL_TIM_Base_Init(&htimx8);
    
    Clock_Config.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    HAL_TIM_ConfigClockSource(&htimx8, &Clock_Config);
    
    HAL_TIM_PWM_Init(&htimx8);
    
    Master_Config.MasterOutputTrigger = TIM_TRGO_RESET;
    Master_Config.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    HAL_TIMEx_MasterConfigSynchronization(&htimx8, &Master_Config);
    
    BreakDeadTime_Config.OffStateRunMode = TIM_OSSR_DISABLE;
    BreakDeadTime_Config.OffStateIDLEMode = TIM_OSSI_DISABLE;
    BreakDeadTime_Config.LockLevel = TIM_LOCKLEVEL_OFF;
    BreakDeadTime_Config.DeadTime = 0;
    BreakDeadTime_Config.BreakState = TIM_BREAK_DISABLE;
    BreakDeadTime_Config.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
    BreakDeadTime_Config.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
    HAL_TIMEx_ConfigBreakDeadTime(&htimx8, &BreakDeadTime_Config);
    
    OC_Config.OCMode = TIM_OCMODE_PWM1;
    OC_Config.Pulse = 900;
    OC_Config.OCPolarity = TIM_OCPOLARITY_HIGH;
    OC_Config.OCNPolarity = TIM_OCNPOLARITY_LOW;
    OC_Config.OCFastMode = TIM_OCFAST_DISABLE;
    OC_Config.OCIdleState = TIM_OCIDLESTATE_RESET;
    OC_Config.OCNIdleState = TIM_OCNIDLESTATE_RESET;
    HAL_TIM_PWM_ConfigChannel(&htimx8, &OC_Config, TIM_CHANNEL_1);
}

void advanced_tim_pwm_output_test(void)
{
    HAL_TIM_MspPostInit();
    ADVANCED_TIM_Mode_Config();
    
    HAL_TIM_PWM_Start(&htimx8, TIM_CHANNEL_1);
    HAL_TIMEx_PWMN_Start(&htimx8, TIM_CHANNEL_1);
}