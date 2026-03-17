#ifndef __BSP_IWDG_H__
#define __BSP_IWDG_H__

#ifdef __cplusplus
extern "C" {
#endif
    
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_cortex.h"

void iwdg_config(uint8_t prv, uint16_t rlv);
void iwdg_feed(void);


#ifdef __cplusplus
}
#endif

#endif