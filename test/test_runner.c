/**
  ******************************************************************************
  * @file    test_runner.c
  * @brief   Main test runner entry point for Actuator Node unit tests.
  * @team    CUERT Embedded & Control
  ******************************************************************************
  */

#include "unity.h"

/* External test function prototypes */
extern void test_Blinky_SingleStep_IncrementsCounterAndTogglesPin(void);
extern void test_Blinky_MultipleCycles_AlternatesPinState(void);
extern void test_Blinky_TimingSimulation_1HzRate(void);

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_Blinky_SingleStep_IncrementsCounterAndTogglesPin);
    RUN_TEST(test_Blinky_MultipleCycles_AlternatesPinState);
    RUN_TEST(test_Blinky_TimingSimulation_1HzRate);

    return UNITY_END();
}
