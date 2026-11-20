/**
  ******************************************************************************
  * @file    test_blinky.c
  * @brief   Unit test suite for Actuator Node Blinky commissioning firmware.
  * @team    CUERT Embedded & Control
  ******************************************************************************
  */

#include "unity.h"
#include "main.h"
#include "stm32_hal_stubs.h"

void setUp(void)
{
    Mock_HAL_Reset();
}

void tearDown(void)
{
}

void test_Blinky_SingleStep_IncrementsCounterAndTogglesPin(void)
{
    TEST_ASSERT_EQUAL_UINT32(0, Mock_HAL_GetToggleCount(GPIO_PIN_5));
    TEST_ASSERT_EQUAL(GPIO_PIN_RESET, Mock_HAL_GetPinState(GPIO_PIN_5));

    Blinky_Step();

    TEST_ASSERT_EQUAL_UINT32(1, Mock_HAL_GetToggleCount(GPIO_PIN_5));
    TEST_ASSERT_EQUAL(GPIO_PIN_SET, Mock_HAL_GetPinState(GPIO_PIN_5));
}

void test_Blinky_MultipleCycles_AlternatesPinState(void)
{
    /* Step 1: Transition LOW -> HIGH */
    Blinky_Step();
    TEST_ASSERT_EQUAL(GPIO_PIN_SET, Mock_HAL_GetPinState(GPIO_PIN_5));

    /* Step 2: Transition HIGH -> LOW */
    Blinky_Step();
    TEST_ASSERT_EQUAL(GPIO_PIN_RESET, Mock_HAL_GetPinState(GPIO_PIN_5));

    /* Step 3: Transition LOW -> HIGH */
    Blinky_Step();
    TEST_ASSERT_EQUAL(GPIO_PIN_SET, Mock_HAL_GetPinState(GPIO_PIN_5));

    /* Step 4: Transition HIGH -> LOW */
    Blinky_Step();
    TEST_ASSERT_EQUAL(GPIO_PIN_RESET, Mock_HAL_GetPinState(GPIO_PIN_5));

    TEST_ASSERT_EQUAL_UINT32(4, Mock_HAL_GetToggleCount(GPIO_PIN_5));
}

void test_Blinky_TimingSimulation_1HzRate(void)
{
    /* Simulate 1 second execution (2 toggles at 500ms delay) */
    Mock_HAL_SetTick(0);

    Blinky_Step();
    HAL_Delay(500);

    Blinky_Step();
    HAL_Delay(500);

    TEST_ASSERT_EQUAL_UINT32(2, Mock_HAL_GetToggleCount(GPIO_PIN_5));
    TEST_ASSERT_EQUAL_UINT32(1000, HAL_GetTick());
}
