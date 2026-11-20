/**
  ******************************************************************************
  * @file    stm32_hal_stubs.c
  * @brief   Mock implementation of STM32 HAL GPIO and timing for host tests.
  * @team    CUERT Embedded & Control
  ******************************************************************************
  */

#include "stm32_hal_stubs.h"

GPIO_TypeDef Mock_GPIOA = {0};
GPIO_TypeDef Mock_GPIOC = {0};

static uint32_t s_toggle_count_pa5 = 0;
static GPIO_PinState s_pin_state_pa5 = GPIO_PIN_RESET;
static uint32_t s_mock_tick = 0;

void Mock_HAL_Reset(void)
{
    s_toggle_count_pa5 = 0;
    s_pin_state_pa5 = GPIO_PIN_RESET;
    s_mock_tick = 0;
}

uint32_t Mock_HAL_GetToggleCount(uint16_t pin)
{
    if (pin == GPIO_PIN_5) {
        return s_toggle_count_pa5;
    }
    return 0;
}

GPIO_PinState Mock_HAL_GetPinState(uint16_t pin)
{
    if (pin == GPIO_PIN_5) {
        return s_pin_state_pa5;
    }
    return GPIO_PIN_RESET;
}

void Mock_HAL_SetTick(uint32_t tick)
{
    s_mock_tick = tick;
}

void HAL_GPIO_TogglePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
    (void)GPIOx;
    if (GPIO_Pin == GPIO_PIN_5) {
        s_toggle_count_pa5++;
        s_pin_state_pa5 = (s_pin_state_pa5 == GPIO_PIN_RESET) ? GPIO_PIN_SET : GPIO_PIN_RESET;
    }
}

void HAL_GPIO_WritePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState)
{
    (void)GPIOx;
    if (GPIO_Pin == GPIO_PIN_5) {
        s_pin_state_pa5 = PinState;
    }
}

GPIO_PinState HAL_GPIO_ReadPin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
    (void)GPIOx;
    if (GPIO_Pin == GPIO_PIN_5) {
        return s_pin_state_pa5;
    }
    return GPIO_PIN_RESET;
}

void HAL_Delay(uint32_t Delay)
{
    s_mock_tick += Delay;
}

uint32_t HAL_GetTick(void)
{
    return s_mock_tick;
}
