#include "bsp_usart.h"
#include "main.h"
#include <stdio.h>


//typedef struct ___FILE {
//    int handle;
//}FILE;

FILE __stdout;

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

void _sys_exit(int x) {
    x = x;
    while(1);
}

int fputc(int ch, FILE *f)
{
    HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);
    return (ch);
}

int fgetc(FILE *f)
{
    int ch;
    while (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_RXNE) == RESET);
    HAL_UART_Receive(&huart1, (uint8_t *)&ch, 1, 0xFFFF);
    return (ch);
}