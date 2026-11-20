/**
  ******************************************************************************
  * @file    stm32g4xx_it.c
  * @brief   Interrupt Service Routines for STM32G431.
  * @team    CUERT Embedded & Control
  ******************************************************************************
  */

#include "main.h"
#include "stm32g4xx_it.h"

/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */
/******************************************************************************/

void NMI_Handler(void)
{
  while (1) {}
}

void HardFault_Handler(void)
{
  while (1) {}
}

void MemManage_Handler(void)
{
  while (1) {}
}

void BusFault_Handler(void)
{
  while (1) {}
}

void UsageFault_Handler(void)
{
  while (1) {}
}

void SVC_Handler(void)
{
}

void DebugMon_Handler(void)
{
}

void PendSV_Handler(void)
{
}

void SysTick_Handler(void)
{
#ifdef USE_HAL_DRIVER
  HAL_IncTick();
#endif
}
