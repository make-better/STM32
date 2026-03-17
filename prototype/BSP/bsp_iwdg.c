#include "bsp_iwdg.h"

IWDG_HandleTypeDef hiwdg;

void iwdg_config(uint8_t prv, uint16_t rlv)
{
    hiwdg.Instance = IWDG;
    hiwdg.Init.Prescaler = prv;
    hiwdg.Init.Reload = rlv;
    HAL_IWDG_Init(&hiwdg);
    
    __HAL_IWDG_START(&hiwdg);
}


void iwdg_feed(void)
{
    HAL_IWDG_Refresh(&hiwdg);
}



