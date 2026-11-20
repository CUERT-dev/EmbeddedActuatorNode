/**
  ******************************************************************************
  * @file    system_stm32g4xx.c
  * @author  MCD Application Team / CUERT
  * @brief   CMSIS Cortex-M4 Device Peripheral Access Layer System Source File
  ******************************************************************************
  */

#include <stdint.h>

#if !defined  (HSE_VALUE)
  #define HSE_VALUE     24000000U /*!< Value of the External oscillator in Hz */
#endif

#if !defined  (HSI_VALUE)
  #define HSI_VALUE     16000000U /*!< Value of the Internal oscillator in Hz*/
#endif

uint32_t SystemCoreClock = 16000000U;

const uint8_t AHBPrescTable[16] = {0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 1U, 2U, 3U, 4U, 6U, 7U, 8U, 9U};
const uint8_t APBPrescTable[8]  = {0U, 0U, 0U, 0U, 1U, 2U, 3U, 4U};

void SystemInit(void)
{
  /* Configure FPU if present */
  #if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
    /* Set CP10 and CP11 Full Access */
    *(volatile uint32_t *)0xE000ED88 |= ((3UL << 20U) | (3UL << 22U));
  #endif

  /* Reset the RCC clock configuration to the default reset state */
  /* Set HSION bit */
  *(volatile uint32_t *)0x40021000 |= 0x00000100U;
}

void SystemCoreClockUpdate(void)
{
  SystemCoreClock = HSI_VALUE;
}
