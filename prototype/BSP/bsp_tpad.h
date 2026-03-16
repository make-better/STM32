#ifndef __BSP_TPAD_H__
#define __BSP_TPAD_H__

#ifdef __cplusplus
extern "C" {
#endif
    
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_cortex.h"



uint8_t tpad_init(void);
uint8_t tpad_scan(uint8_t mode);

#ifdef __cplusplus
}
#endif

#endif