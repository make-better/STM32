#include "bsp_usart.h"
#include "main.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "FreeRTOS.h"
#include "semphr.h"

// 禁用半主机模式
__asm(".global __use_no_semihosting\n\t");
// 禁用命令行
__asm(".global __ARM_use_no_argv \n\t");

void _sys_exit(int x) 
{ 
    x = x; 
}


static SemaphoreHandle_t printf_mutex = NULL;

extern UART_HandleTypeDef huart1;
void Led_Ctrl(uint8_t c);
static uint8_t rx_buff[100] = {0};

#define UART1_BUFFER_SIZE 100
uint8_t uart1_tx_buffer[UART1_BUFFER_SIZE] = {0};
uint8_t uart1_rx_buffer[UART1_BUFFER_SIZE] = {0};


void The_UART_Callback(UART_HandleTypeDef *huart)
{
    uint8_t temp = 0;
    if(__HAL_UART_GET_FLAG(huart, UART_FLAG_RXNE))
    {
        temp = huart->Instance->DR;
        Led_Ctrl(temp);
        HAL_UART_Transmit(huart, &temp, 1, 10);
    }
    __HAL_UART_CLEAR_PEFLAG(huart);
}


//LED ctrl test
void Led_Ctrl(uint8_t c)
{
    if(c == 'r')
    {
        HAL_GPIO_WritePin(LED_R_GPIO_Port,LED_R_Pin,0);
    }
    else if(c == 'g')
    {
        HAL_GPIO_WritePin(LED_G_GPIO_Port,LED_G_Pin,0);
    }
    else if(c == 'b')
    {
        HAL_GPIO_WritePin(LED_B_GPIO_Port,LED_B_Pin,0);
    }
    if(c == 'c')
    {
        HAL_GPIO_WritePin(LED_R_GPIO_Port,LED_R_Pin,1);
        HAL_GPIO_WritePin(LED_G_GPIO_Port,LED_G_Pin,1);
        HAL_GPIO_WritePin(LED_B_GPIO_Port,LED_B_Pin,1);
    }
}

void debug_info(const char *format, ...)
{
    va_list args;
    if(printf_mutex == NULL)
    {
        printf_mutex = xSemaphoreCreateMutex();
    }
    if(xSemaphoreTake(printf_mutex, portMAX_DELAY) == pdTRUE)
    {
        va_start(args, format);
        vsnprintf((char*)uart1_tx_buffer, sizeof(uart1_tx_buffer), format, args);
        va_end(args);
        HAL_UART_Transmit(&huart1,uart1_tx_buffer,strlen((char*)uart1_tx_buffer), 0xfff);
        xSemaphoreGive(printf_mutex);
    }
}


//int fputc(int ch, FILE *f)
//{
//    HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);
//    return (ch);
//}

//int fgetc(FILE *f)
//{
//    int ch;
//    while (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_RXNE) == RESET);
//    HAL_UART_Receive(&huart1, (uint8_t *)&ch, 1, 0xFFFF);
//    return (ch);
//}