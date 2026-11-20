/**
  ******************************************************************************
  * @file         stm32g4xx_hal_msp.c
  * @brief        MSP Initialization and de-Initialization codes.
  * @team         CUERT Embedded & Control
  ******************************************************************************
  */

#include "main.h"

#ifdef USE_HAL_DRIVER
void HAL_MspInit(void)
{
  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();
}
#endif
