#include "bsp_pvd.h"
#include "main.h"

void pvd_config(void)
{
    PWR_PVDTypeDef config_pvd;
    //使能PWR时钟
    __PWR_CLK_ENABLE();
    //配置中断
    HAL_NVIC_SetPriority(PVD_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(PVD_IRQn);
    
    config_pvd.PVDLevel = PWR_PVDLEVEL_5;//2.8V
    config_pvd.Mode = PWR_PVD_MODE_IT_RISING_FALLING;
    HAL_PWR_ConfigPVD(&config_pvd);
    //使能PVD
    HAL_PWR_EnablePVD();
}


void HAL_PWR_PVDCallback(void)
{
    HAL_GPIO_WritePin(LED_R_GPIO_Port, LED_R_Pin, 0);
}