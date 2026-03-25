#include "bsp_rtc.h"
#include "bsp_usart.h"
#include "main.h"

#define RTC_CLOCK_SOURCE_LSE
//#define RCC_OSCILLATORTYPE_LSI

#define HOURS           0x12
#define MINUTES         0x12
#define SECONDS         0x12

// 日期宏定义  其中WEEKDAY为无效宏定义，只要给出年月日，HAL库会自动计算出 WEEKDAY
#define WEEKDAY         0x3
#define DATE            0x25
#define MONTH           0x3
#define YEAR            0x26

// 备份域寄存器宏定义
#define RTC_BKP_DRX     RTC_BKP_DR1
// 写入到备份寄存器的数据宏定义
#define RTC_BKP_DATA    0x32f2



RTC_HandleTypeDef hrtc;


void RTC_CalendarConfig(void)
{
    RTC_DateTypeDef sDateStructure;
    RTC_TimeTypeDef sTimeStructure;
    
    sDateStructure.Year = YEAR;
    sDateStructure.Month = MONTH;
    sDateStructure.Date = DATE;
    sDateStructure.WeekDay = WEEKDAY;
    
    if(HAL_RTC_SetDate(&hrtc, &sDateStructure, RTC_FORMAT_BCD) != HAL_OK)
    {
        debug_info("date init fail\r\n");
    }
    sTimeStructure.Hours = HOURS;
    sTimeStructure.Minutes = MINUTES;
    sTimeStructure.Seconds = SECONDS;
    
    if(HAL_RTC_SetTime(&hrtc, &sTimeStructure, RTC_FORMAT_BCD) != HAL_OK)
    {
        debug_info("time init fail\r\n");
    }
    
    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DRX, RTC_BKP_DATA);
}

void RTC_TimeAndDate_Show(void)
{
    uint8_t rtc_temp = 0;
    RTC_TimeTypeDef rtc_time;
    RTC_DateTypeDef rtc_date;
    
    for(int i = 0; i < 8; )
    {
        HAL_RTC_GetTime(&hrtc, &rtc_time, RTC_FORMAT_BIN);
        HAL_RTC_GetDate(&hrtc, &rtc_date, RTC_FORMAT_BIN);
        if(rtc_temp != rtc_time.Seconds)
        {
            debug_info("The Date :  Y:%0.2d - M:%0.2d - D:%0.2d - W:%0.2d\r\n", 
            rtc_date.Year,
            rtc_date.Month, 
            rtc_date.Date,
            rtc_date.WeekDay);
            
            debug_info("The Time :  %0.2d:%0.2d:%0.2d \r\n\r\n", 
            rtc_time.Hours, 
            rtc_time.Minutes, 
            rtc_time.Seconds);
            i++;
        }
        rtc_temp = rtc_time.Seconds;
    }
}

void HAL_RTC_MspInit(RTC_HandleTypeDef *hrtc)
{
    RCC_OscInitTypeDef rcc_osc_init;
    RCC_PeriphCLKInitTypeDef perih_clk_init;
    
    __HAL_RCC_PWR_CLK_ENABLE();
    HAL_PWR_EnableBkUpAccess();
    __HAL_RCC_BKP_CLK_ENABLE();
    
    rcc_osc_init.OscillatorType = RCC_OSCILLATORTYPE_LSE;
    rcc_osc_init.PLL.PLLState = RCC_PLL_NONE;
    rcc_osc_init.LSEState = RCC_LSE_ON;
    if(HAL_RCC_OscConfig(&rcc_osc_init) != HAL_OK)
    {
        debug_info("lse init fail\r\n");
    }
    
    perih_clk_init.PeriphClockSelection = RCC_PERIPHCLK_RTC;
    perih_clk_init.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
    if(HAL_RCCEx_PeriphCLKConfig(&perih_clk_init) != HAL_OK)
    {
        debug_info("rtc init fail\r\n");
    }
    __HAL_RCC_RTC_ENABLE();
}

void rtc_alarm_set(void)
{
    RTC_TimeTypeDef rtc_time;
    RTC_AlarmTypeDef rtc_alarm;
    
    HAL_NVIC_SetPriority(RTC_Alarm_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);
    
    HAL_RTC_GetTime(&hrtc, &rtc_time, RTC_FORMAT_BCD);
    rtc_alarm.Alarm = RTC_ALARM_A;
    rtc_alarm.AlarmTime.Hours = rtc_time.Hours;
    rtc_alarm.AlarmTime.Minutes = rtc_time.Minutes;
    rtc_alarm.AlarmTime.Seconds = rtc_time.Seconds + 5;
    
    HAL_RTC_SetAlarm_IT(&hrtc, &rtc_alarm, RTC_FORMAT_BCD);
}

void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
    HAL_GPIO_WritePin(LED_R_GPIO_Port, LED_R_Pin, 0);
}

void rtc_config(void)
{
    hrtc.Instance = RTC;
    hrtc.Init.AsynchPrediv = RTC_AUTO_1_SECOND;
    HAL_RTC_Init(&hrtc);
}

uint8_t rtc_test(void)
{
    rtc_config();
    RTC_CalendarConfig();
    
    rtc_alarm_set();
    if(__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST) != RESET)
    {
        debug_info("\r\n power reset \r\n");
    }
    else if(__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST) != RESET)
    {
        debug_info("\r\n exit reset \r\n");
    }
    __HAL_RCC_CLEAR_RESET_FLAGS();
    
    RTC_TimeAndDate_Show();
    return 0;
}