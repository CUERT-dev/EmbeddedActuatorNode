/**
  ******************************************************************************
  * @file    stm32_hal_stubs.h
  * @brief   Mock definitions and test fixtures for STM32 HAL GPIO & Timing.
  * @team    CUERT Embedded & Control
  ******************************************************************************
  */

#ifndef STM32_HAL_STUBS_H
#define STM32_HAL_STUBS_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Type stubs mimicking STM32 HAL */
typedef struct {
    volatile uint32_t MODER;
    volatile uint32_t OTYPER;
    volatile uint32_t OSPEEDR;
    volatile uint32_t PUPDR;
    volatile uint32_t IDR;
    volatile uint32_t ODR;
} GPIO_TypeDef;

typedef enum {
    GPIO_PIN_RESET = 0U,
    GPIO_PIN_SET = 1U
} GPIO_PinState;

#define GPIO_PIN_0    ((uint16_t)0x0001)
#define GPIO_PIN_5    ((uint16_t)0x0020)
#define GPIO_PIN_13   ((uint16_t)0x2000)

extern GPIO_TypeDef Mock_GPIOA;
extern GPIO_TypeDef Mock_GPIOC;

#define GPIOA (&Mock_GPIOA)
#define GPIOC (&Mock_GPIOC)

/* HAL API Mock Signatures */
void HAL_GPIO_TogglePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
void HAL_GPIO_WritePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState);
GPIO_PinState HAL_GPIO_ReadPin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
void HAL_Delay(uint32_t Delay);
uint32_t HAL_GetTick(void);

/* Mock Test Helpers */
void Mock_HAL_Reset(void);
uint32_t Mock_HAL_GetToggleCount(uint16_t pin);
GPIO_PinState Mock_HAL_GetPinState(uint16_t pin);
void Mock_HAL_SetTick(uint32_t tick);

#ifdef __cplusplus
}
#endif

#endif /* STM32_HAL_STUBS_H */
