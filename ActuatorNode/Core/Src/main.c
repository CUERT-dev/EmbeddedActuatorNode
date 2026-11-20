/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Embedded Actuator Node - Nucleo Board Commissioning Blinky
  * @team           : CUERT (Cairo University Eco-Racing Team)
  * @node           : Embedded Actuator Node
  * @target         : NUCLEO-G431RB (STM32G431RBT6)
  ******************************************************************************
  * @details
  * This is the initial commissioning firmware for the CUERT Actuator Node.
  * It verifies toolchain setup, clock distribution, GPIO output operation,
  * and CI integration by blinking onboard green LED LD2 (PA5) at 1 Hz.
  *
  * Hardware mapping (NUCLEO-G431RB):
  *   - User LED (LD2)   : PA5 (Active High, Green)
  *   - User Button (B1) : PC13 (Active Low, Blue)
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private variables ---------------------------------------------------------*/
static volatile uint32_t s_blink_counter = 0;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_GPIO_Init(void);

/**
  * @brief  Single execution step for the LED blinky state machine.
  *         Isolated for direct host unit testing and static analysis.
  */
void Blinky_Step(void)
{
#ifdef USE_HAL_DRIVER
  HAL_GPIO_TogglePin(LED_GREEN_GPIO_PORT, LED_GREEN_PIN);
#endif
  s_blink_counter++;
}

/**
  * @brief  Get total blink toggle cycles executed.
  * @retval Counter value.
  */
uint32_t Blinky_GetCount(void)
{
  return s_blink_counter;
}

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
#ifdef USE_HAL_DRIVER
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals (PA5 User LED) */
  MX_GPIO_Init();
#endif

  /* Infinite loop */
  while (1)
  {
    Blinky_Step();
#ifdef USE_HAL_DRIVER
    HAL_Delay(500); /* 500 ms delay -> 1 Hz toggle rate (0.5s ON, 0.5s OFF) */
#endif
  }
}

#ifdef USE_HAL_DRIVER
/**
  * @brief System Clock Configuration for STM32G431
  *        Configures HSI16 (16 MHz) as system clock source for commissioning.
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /* Configure the main internal regulator output voltage */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

  /* Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /* Initializes the CPU, AHB and APB buses clocks */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                              | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief GPIO Initialization Function
  *        Configures PA5 as Output Push-Pull (NUCLEO-G431RB LD2 Green LED)
  * @retval None
  */
void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  LED_GREEN_GPIO_CLK_ENABLE();

  /* Configure GPIO pin Output Level: initially OFF (LOW) */
  HAL_GPIO_WritePin(LED_GREEN_GPIO_PORT, LED_GREEN_PIN, GPIO_PIN_RESET);

  /* Configure GPIO pin : PA5 (LD2) */
  GPIO_InitStruct.Pin = LED_GREEN_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_GREEN_GPIO_PORT, &GPIO_InitStruct);
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
    /* Trap in fault state */
  }
}
#endif
