#ifndef __BSP_TIM_GENERAL_H__
#define __BSP_TIM_GENERAL_H__

#ifdef __cplusplus
extern "C" {
#endif
    
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_cortex.h"

// 定时器输入捕获用户自定义变量结构体声明
typedef struct              
{   
	uint8_t   ucFinishFlag;   // 捕获结束标志位
	uint8_t   ucStartFlag;    // 捕获开始标志位
	uint16_t  usCtr;          // 捕获寄存器的值
	uint16_t  usPeriod;       // 自动重装载寄存器更新标志 
}STRUCT_CAPTURE; 


extern TIM_HandleTypeDef htimx5;
extern STRUCT_CAPTURE TIM_ICUserValueStructure;
extern STRUCT_CAPTURE TIM_ICUserShow;

void general_tim_ic_config(void);
void show_ic_value(void);
#ifdef __cplusplus
}
#endif

#endif