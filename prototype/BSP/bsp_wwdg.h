#ifndef __BSP_WWDG_H__
#define __BSP_WWDG_H__

#ifdef __cplusplus
extern "C" {
#endif
    
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_cortex.h"

void wwdg_config(uint8_t tr, uint8_t wr, uint32_t prv);
void wwdg_feed(void);

#ifdef __cplusplus
}
#endif

#endif