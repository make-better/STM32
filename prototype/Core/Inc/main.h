/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define KEY2_Pin GPIO_PIN_13
#define KEY2_GPIO_Port GPIOC
#define SPIFLASH_CS_Pin GPIO_PIN_0
#define SPIFLASH_CS_GPIO_Port GPIOC
#define C_HS_Pin GPIO_PIN_2
#define C_HS_GPIO_Port GPIOC
#define C_VS_Pin GPIO_PIN_3
#define C_VS_GPIO_Port GPIOC
#define KEY1_Pin GPIO_PIN_0
#define KEY1_GPIO_Port GPIOA
#define ADC_INPUT_KEY_Pin GPIO_PIN_1
#define ADC_INPUT_KEY_GPIO_Port GPIOA
#define C_RRST_Pin GPIO_PIN_2
#define C_RRST_GPIO_Port GPIOA
#define C_OE_Pin GPIO_PIN_3
#define C_OE_GPIO_Port GPIOA
#define SPI1_SCK_Pin GPIO_PIN_5
#define SPI1_SCK_GPIO_Port GPIOA
#define C_WRST_Pin GPIO_PIN_4
#define C_WRST_GPIO_Port GPIOC
#define C_RCLK_Pin GPIO_PIN_5
#define C_RCLK_GPIO_Port GPIOC
#define LED_G_Pin GPIO_PIN_0
#define LED_G_GPIO_Port GPIOB
#define LED_B_Pin GPIO_PIN_1
#define LED_B_GPIO_Port GPIOB
#define C_D2_Pin GPIO_PIN_10
#define C_D2_GPIO_Port GPIOB
#define C_D3_Pin GPIO_PIN_11
#define C_D3_GPIO_Port GPIOB
#define C_D4_Pin GPIO_PIN_12
#define C_D4_GPIO_Port GPIOB
#define C_D5_Pin GPIO_PIN_13
#define C_D5_GPIO_Port GPIOB
#define C_D6_Pin GPIO_PIN_14
#define C_D6_GPIO_Port GPIOB
#define C_D7_Pin GPIO_PIN_15
#define C_D7_GPIO_Port GPIOB
#define C_SCL_Pin GPIO_PIN_6
#define C_SCL_GPIO_Port GPIOC
#define C_SDA_Pin GPIO_PIN_7
#define C_SDA_GPIO_Port GPIOC
#define C_XCLK_BEEP_Pin GPIO_PIN_8
#define C_XCLK_BEEP_GPIO_Port GPIOA
#define C_WEN_Pin GPIO_PIN_3
#define C_WEN_GPIO_Port GPIOD
#define USB_E_Pin GPIO_PIN_6
#define USB_E_GPIO_Port GPIOD
#define LED_R_Pin GPIO_PIN_5
#define LED_R_GPIO_Port GPIOB
#define I2C1_SCL_Pin GPIO_PIN_6
#define I2C1_SCL_GPIO_Port GPIOB
#define I2C1_SDA_Pin GPIO_PIN_7
#define I2C1_SDA_GPIO_Port GPIOB
#define C_D0_Pin GPIO_PIN_8
#define C_D0_GPIO_Port GPIOB
#define C_D1_Pin GPIO_PIN_9
#define C_D1_GPIO_Port GPIOB
#define LCD_TP_Pin GPIO_PIN_0
#define LCD_TP_GPIO_Port GPIOE
#define LCD_RST_Pin GPIO_PIN_1
#define LCD_RST_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
