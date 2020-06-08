/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
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
#define LED_Pin GPIO_PIN_13
#define LED_GPIO_Port GPIOC
#define Motor1_EncoderA_Pin GPIO_PIN_6
#define Motor1_EncoderA_GPIO_Port GPIOA
#define Motor1_EncoderB_Pin GPIO_PIN_7
#define Motor1_EncoderB_GPIO_Port GPIOA
#define Motor1_EN_Pin GPIO_PIN_1
#define Motor1_EN_GPIO_Port GPIOB
#define BOOT1_Pin GPIO_PIN_2
#define BOOT1_GPIO_Port GPIOB
#define Motor1_INA_Pin GPIO_PIN_10
#define Motor1_INA_GPIO_Port GPIOB
#define Motor1_INB_Pin GPIO_PIN_11
#define Motor1_INB_GPIO_Port GPIOB
#define Motor2_INA_Pin GPIO_PIN_12
#define Motor2_INA_GPIO_Port GPIOB
#define Motor2_INB_Pin GPIO_PIN_13
#define Motor2_INB_GPIO_Port GPIOB
#define Motor2_EN_Pin GPIO_PIN_14
#define Motor2_EN_GPIO_Port GPIOB
#define Motor1_PWM_Pin GPIO_PIN_15
#define Motor1_PWM_GPIO_Port GPIOA
#define Motor2_PWM_Pin GPIO_PIN_3
#define Motor2_PWM_GPIO_Port GPIOB
#define Motor2_EncoderA_Pin GPIO_PIN_6
#define Motor2_EncoderA_GPIO_Port GPIOB
#define Motor2_EncoderB_Pin GPIO_PIN_7
#define Motor2_EncoderB_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
