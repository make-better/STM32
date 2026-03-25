#ifndef __BSP_RTC_H__
#define __BSP_RTC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_cortex.h"

void RTC_CalendarConfig(void);
void RTC_TimeAndDate_Show(void);
void rtc_config(void);
uint8_t rtc_test(void);
void rtc_alarm_set(void);
#ifdef __cplusplus
}
#endif

#endif
