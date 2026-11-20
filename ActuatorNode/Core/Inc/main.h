/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  * @team           : CUERT (Cairo University Eco-Racing Team)
  * @node           : Embedded Actuator Node (STM32G431 / Nucleo-G431RB)
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics & CUERT.
  * All rights reserved.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>

#ifdef USE_HAL_DRIVER
#include "stm32g4xx_hal.h"
#endif

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* Hardware Pin Definitions for NUCLEO-G431RB */
#define LED_GREEN_PIN                    GPIO_PIN_5
#define LED_GREEN_GPIO_PORT              GPIOA
#define LED_GREEN_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()

#define USER_BUTTON_PIN                  GPIO_PIN_13
#define USER_BUTTON_GPIO_PORT            GPIOC
#define USER_BUTTON_GPIO_CLK_ENABLE()    __HAL_RCC_GPIOC_CLK_ENABLE()

/* Exported macro ------------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);
void SystemClock_Config(void);
void MX_GPIO_Init(void);
void Blinky_Step(void);

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
