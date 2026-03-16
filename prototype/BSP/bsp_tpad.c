#include "bsp_tpad.h"
#include "bsp_usart.h"


#define TOUCHPAD_GATE_VAL               100

#define TOUCHPAD_TIM_PRESCALER          (48-1)//1.5MHz
#define TOUCHPAD_TIM_ARR                0XFFFF

#define TOUCHPAD_TIMx                   TIM5
#define TOUCHPAD_TIM_RCC_CLK_ENABLE()   __HAL_RCC_TIM5_CLK_ENABLE()
#define TOUCHPAD_TIM_RCC_CLK_DISABLE()  __HAL_RCC_TIM5_CLK_DISABLE()

#define TOUCHPAD_GPIO_RCC_CLK_ENABLE()  __HAL_RCC_GPIOA_CLK_ENABLE()
#define TOUCHPAD_GPIO_PIN               GPIO_PIN_1
#define TOUCHPAD_GPIO                   GPIOA
#define TOUCHPAD_TIM_CHANNEL            TIM_CHANNEL_2
#define TOUCHPAD_TIM_FLAG_CCR           TIM_FLAG_CC2

//保存没按下时定时器计数值
__IO uint16_t tpad_default_val=0;

TIM_HandleTypeDef htimx_tpad;

static void touchpad_gpio_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    
    TOUCHPAD_GPIO_RCC_CLK_ENABLE();
    
    GPIO_InitStruct.Pin = TOUCHPAD_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    HAL_GPIO_Init(TOUCHPAD_GPIO, &GPIO_InitStruct);
}

static void touchpad_tim_mode_config(void)
{
    TIM_ClockConfigTypeDef CLKSource;
    TIM_MasterConfigTypeDef MasterConfig;
    TIM_IC_InitTypeDef ICConfig;
    
    TOUCHPAD_TIM_RCC_CLK_ENABLE();
    
    htimx_tpad.Instance = TOUCHPAD_TIMx;
    htimx_tpad.Init.Prescaler = TOUCHPAD_TIM_PRESCALER;
    htimx_tpad.Init.CounterMode = TIM_COUNTERMODE_UP;
    htimx_tpad.Init.Period = TOUCHPAD_TIM_ARR;
    htimx_tpad.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_Base_Init(&htimx_tpad);
    
    CLKSource.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    HAL_TIM_ConfigClockSource(&htimx_tpad, &CLKSource);
    
    MasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    MasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    HAL_TIMEx_MasterConfigSynchronization(&htimx_tpad, &MasterConfig);
    
    ICConfig.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
    ICConfig.ICSelection = TIM_ICSELECTION_DIRECTTI;
    ICConfig.ICPrescaler = TIM_ICPSC_DIV1;
    ICConfig.ICFilter = 3;
    HAL_TIM_IC_ConfigChannel(&htimx_tpad, &ICConfig, TOUCHPAD_TIM_CHANNEL);
}

static void touchpad_reset(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    
    TOUCHPAD_GPIO_RCC_CLK_ENABLE();
    //电容放电
    HAL_GPIO_WritePin(TOUCHPAD_GPIO, TOUCHPAD_GPIO_PIN, GPIO_PIN_RESET);
    
    GPIO_InitStruct.Pin = TOUCHPAD_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(TOUCHPAD_GPIO, &GPIO_InitStruct);
    
    HAL_Delay(5);
    
    __HAL_TIM_SET_COUNTER(&htimx_tpad, 0);//清零定时器计数
    __HAL_TIM_CLEAR_FLAG(&htimx_tpad, TIM_FLAG_UPDATE | TOUCHPAD_TIM_FLAG_CCR);
    
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(TOUCHPAD_GPIO, &GPIO_InitStruct);
    //开始检测上升沿
    HAL_TIM_IC_Start(&htimx_tpad, TOUCHPAD_TIM_CHANNEL);
}

//获取定时器捕获值
static uint16_t tpad_get_val(void)
{
    touchpad_reset();
    
    while(__HAL_TIM_GET_FLAG(&htimx_tpad, TOUCHPAD_TIM_FLAG_CCR) == RESET)
    {
        uint16_t count;
        count = __HAL_TIM_GET_COUNTER(&htimx_tpad);
        if(count > (TOUCHPAD_TIM_ARR - 500))
        {
            return count;   //超时
        }
    }
    return HAL_TIM_ReadCapturedValue(&htimx_tpad, TOUCHPAD_TIM_CHANNEL);
}

//连续读取n次，取最大值
static uint16_t tpad_get_max_val(uint8_t n)
{
    uint16_t temp = 0;
    uint16_t res = 0;
    while(n--)
    {
        temp = tpad_get_val();
        if(temp > res)
        {
            res = temp;
        }
    }
    return res;
}
//初始化触摸按键，0：初始化成功       1:初始化失败
uint8_t tpad_init(void)
{
    uint16_t buf[10];
    uint32_t temp = 0;
    uint8_t j,i;
    
    touchpad_gpio_init();
    touchpad_tim_mode_config();
    
    HAL_TIM_IC_Start(&htimx_tpad, TOUCHPAD_TIM_CHANNEL);
    //取10次数据
    for(i = 0; i < 10; i++)
    {
        buf[i] = tpad_get_val();
        HAL_Delay(10);
    }
    //排序
    for(i = 0; i < 9; i++)
    {
        for(j = i + 1; j < 10; j++)
        {
            if(buf[i] > buf[j])
            {
                temp = buf[i];
                buf[i] = buf[j];
                buf[j] = temp;
            }
        }
    }
    temp = 0;
    //取中间6个数取平均值
    for(i = 2; i < 8; i++)
    {
        temp += buf[i];
    }
    tpad_default_val = temp/6;
    debug_info("tpad_default_val:%d\r\n", tpad_default_val);
    if(tpad_default_val > TOUCHPAD_TIM_ARR / 2)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

//阈值：捕获时间必须大于(tpad_default_val + TPAD_GATE_VAL),才认为是有效触摸.
#define TPAD_GATE_VAL 	100	

/**
* 扫描触摸按键
* mode: 0 不支持连续触发   1 支持连续触发
* 返回值： 0 未按下    1 按下
*
*/
uint8_t tpad_scan(uint8_t mode)
{
    //0 可以检测   keyen>0还不能开始检测
    static uint8_t keyen = 0;
    uint8_t res = 0;
    uint8_t sample = 3;
    uint16_t rval;
    
    if(mode)
    {//支持连按则采样次数为6
        sample = 6;
        keyen = 0;
    }
    rval = tpad_get_max_val(sample);
    debug_info("scan_rval = %d\r\n", rval);
    if(rval > (tpad_default_val + TPAD_GATE_VAL) && rval < (30 * tpad_default_val))
    {
        if(keyen == 0)
        {
            res = 1;
            
        }
        keyen = 3;//至少再过3次后按键才能再次扫描
    }
    
    if(keyen)
    {
        keyen--;
    }
    return res;
}