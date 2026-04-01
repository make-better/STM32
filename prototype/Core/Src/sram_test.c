#include "sram_test.h"
#include "main.h"
#include "bsp_usart.h"
//#include "stdlib.h"

//定义变量到指定节区
#define __EXRAM __attribute__ ((section("EXRAM")))

//__asm(".global __use_no_semihosting\n\t");

uint32_t exram_value __EXRAM = 8;

//void _ttywrch(int ch)
//{
//    ch = ch;
//}

void SRAM_GPIO_TEST(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIOB_CLK_ENABLE();
    
    GPIO_InitStruct.Pin = LED_G_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    HAL_GPIO_WritePin(GPIOB, LED_G_Pin, GPIO_PIN_RESET);

}

/* 节区自定义测试 */
/* 该实验需要手动修改.sct文件，将EXRAM节区单独定义，并且要用到microlib，该库在本工程与freertos冲突 */
void sram_ver_section_test(void)
{
//    uint32_t stack_value = 3;
//    uint32_t *heap_point = malloc(sizeof(uint32_t) * 3);
//    
//    heap_point[0] = 1;
//    heap_point[1] = 2;
//    heap_point[2] = 3;
//    
//    debug_info("exram_value:addr:0x%x  v:0x%x\r\n",&exram_value,exram_value);
//    debug_info("stack_value:addr:0x%x  v:0x%x\r\n",&stack_value,stack_value);
//    debug_info("heap_point:addr:0x%x  v:0x%x\r\n",heap_point,*heap_point);
//    heap_point++;
//    debug_info("heap_point++:addr:0x%x  v:0x%x\r\n",heap_point,*heap_point);
//    heap_point--;
//    debug_info("heap_point--:addr:0x%x  v:0x%x\r\n",heap_point,*heap_point);
//    free(heap_point);
}



