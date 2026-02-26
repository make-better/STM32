#include "bsp_exti.h"
#include "main.h"

void EXTI_Key_Config(void)
{
    HAL_NVIC_SetPriority(KEY1_EXTI_IRQ, 0, 0);
    HAL_NVIC_EnableIRQ(KEY1_EXTI_IRQ);
    HAL_NVIC_SetPriority(KEY2_EXTI_IRQ, 0, 0);
    HAL_NVIC_EnableIRQ(KEY2_EXTI_IRQ);
}

void KEY1_IRQHandler(void)
{
    if(__HAL_GPIO_EXTI_GET_IT(KEY1_INT_GPIO_PIN) != RESET)
    {
        HAL_GPIO_TogglePin(LED_B_GPIO_Port, LED_B_Pin);
        __HAL_GPIO_EXTI_CLEAR_IT(KEY1_INT_GPIO_PIN);
    }
}

void KEY2_IRQHandler(void)
{
    if(__HAL_GPIO_EXTI_GET_IT(KEY2_INT_GPIO_PIN) != RESET)
    {
        HAL_GPIO_TogglePin(LED_R_GPIO_Port, LED_R_Pin);
        __HAL_GPIO_EXTI_CLEAR_IT(KEY2_INT_GPIO_PIN);
    }
}