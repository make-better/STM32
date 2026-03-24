#include "sleep_mode_test.h"
#include "bsp_exti.h"
#include "bsp_usart.h"
#include "main.h"

extern UART_HandleTypeDef huart1;
uint8_t rx_buffer;

uint8_t sleep_mode_wfi_test(void)
{
    EXTI_Key_Config();
    HAL_GPIO_WritePin(LED_B_GPIO_Port, LED_B_Pin, 0);
    debug_info("enter sleep mode\r\n");
    //暂停嘀嗒时钟
    HAL_SuspendTick();
    HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
    debug_info("exit sleep mode\r\n");
    HAL_GPIO_WritePin(LED_B_GPIO_Port, LED_B_Pin, 1);
    HAL_GPIO_WritePin(LED_G_GPIO_Port, LED_G_Pin, 0);
    //回复嘀嗒时钟
    HAL_ResumeTick();
    return 0;
}

static void sysclk_config_stop(void)
{
    RCC_ClkInitTypeDef rcc_clk_init = {0};
    RCC_OscInitTypeDef rcc_osc_init = {0};
    uint32_t pFLatency = 0;
    //启用电源控制时钟
    __HAL_RCC_PWR_CLK_ENABLE();
    //根据内部RCC寄存器获取振荡器配置
    HAL_RCC_GetOscConfig(&rcc_osc_init);
    //从停止模式唤醒后重新配置系统时钟 启用HSE和PLL
    rcc_osc_init.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    rcc_osc_init.HSEState = RCC_HSE_ON;
    rcc_osc_init.PLL.PLLState = RCC_PLL_ON;
    if(HAL_RCC_OscConfig(&rcc_osc_init) != HAL_OK)
    {
        Error_Handler();
    }
    //根据内部RCC仅存去获取时钟配置
    HAL_RCC_GetClockConfig(&rcc_clk_init, &pFLatency);
    rcc_clk_init.ClockType = RCC_CLOCKTYPE_SYSCLK;
    rcc_clk_init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    if(HAL_RCC_ClockConfig(&rcc_clk_init, pFLatency) != HAL_OK)
    {
        Error_Handler();
    }
}

uint8_t stop_mode_test(void)
{
    uint32_t sysclk_fre = 0, hclk_fre = 0, pclk1_fre = 0, pclk2_fre = 0, sysclk_sorce = 0;
    
    EXTI_Key_Config();
    
    HAL_GPIO_WritePin(LED_B_GPIO_Port, LED_B_Pin, 0);
    debug_info("enter sleep mode\r\n");
    //暂停滴答定时器，防止通过嘀嗒时钟中断唤醒
    HAL_SuspendTick();
    //进入停止模式
    HAL_PWR_EnterSTOPMode(PWR_MAINREGULATOR_ON, PWR_STOPENTRY_WFI);
    //等待中断唤醒
    HAL_GPIO_WritePin(LED_B_GPIO_Port, LED_B_Pin, 1);
    HAL_GPIO_WritePin(LED_G_GPIO_Port, LED_G_Pin, 0);
    
    sysclk_fre = HAL_RCC_GetSysClockFreq();
    hclk_fre = HAL_RCC_GetHCLKFreq();
    pclk1_fre = HAL_RCC_GetPCLK1Freq();
    pclk2_fre = HAL_RCC_GetPCLK2Freq();
    sysclk_sorce = __HAL_RCC_GET_SYSCLK_SOURCE();
    
    debug_info("after wakeup clk status:\r\n");
    debug_info("sysclk:%d,\r\nhclk:%d,\r\npclk1:%d,\r\npck2:%d,\r\nclock source:%d(0:HSI 8:PLLCLK)\r\n",sysclk_fre,hclk_fre,pclk1_fre,pclk2_fre,sysclk_sorce);
    
    sysclk_config_stop();
    HAL_ResumeTick();
    
    sysclk_fre = HAL_RCC_GetSysClockFreq();
    hclk_fre = HAL_RCC_GetHCLKFreq();
    pclk1_fre = HAL_RCC_GetPCLK1Freq();
    pclk2_fre = HAL_RCC_GetPCLK2Freq();
    sysclk_sorce = __HAL_RCC_GET_SYSCLK_SOURCE();
    
    debug_info("after config clk status:\r\n");
    debug_info("sysclk:%d,\r\nhclk:%d,\r\npclk1:%d,\r\npck2:%d,\r\nclock source:%d(0:HSI 8:PLLCLK)\r\n",sysclk_fre,hclk_fre,pclk1_fre,pclk2_fre,sysclk_sorce);
    return 0;
}