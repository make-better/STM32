#include "bsp_wwdg.h"
#include "main.h"


WWDG_HandleTypeDef hwwdg;


static void wwdg_nvic_config(void)
{
    HAL_NVIC_SetPriority(WWDG_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(WWDG_IRQn);
}

//    计数器更新时间：= Tpclk1 * 4096 * prv
void wwdg_config(uint8_t tr, uint8_t wr, uint32_t prv)
{
    __HAL_RCC_WWDG_CLK_ENABLE();
    
    wwdg_nvic_config();
    
    hwwdg.Instance = WWDG;
    //预分频器
    hwwdg.Init.Prescaler = prv;
    //上窗口值
    hwwdg.Init.Window = wr;
    //计数器值
    hwwdg.Init.Counter = tr;
    //使能提前唤醒中断
    hwwdg.Init.EWIMode = WWDG_EWI_ENABLE;
    
    HAL_WWDG_Init(&hwwdg);
}

void wwdg_feed(void)
{
    HAL_WWDG_Refresh(&hwwdg);
}

void HAL_WWDG_EarlyWakeupCallback(WWDG_HandleTypeDef *hwwdg)
{
    HAL_GPIO_WritePin(LED_R_GPIO_Port, LED_R_Pin, 0);
    HAL_GPIO_WritePin(LED_G_GPIO_Port, LED_G_Pin, 0);
}