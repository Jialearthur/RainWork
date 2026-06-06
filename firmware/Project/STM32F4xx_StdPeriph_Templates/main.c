/**
 ******************************************************************************
 * @file    Project/STM32F4xx_StdPeriph_Templates/main.c
 * @author  MCD Application Team
 * @version V1.8.1
 * @date    27-January-2022
 * @brief   Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2016 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "My_Led.h"
#include "stm32f4xx.h"

/** @addtogroup Template_Project
 * @{
 */

/* Private function prototypes -----------------------------------------------*/
static void LedTask(void *argument);

/* Private functions ---------------------------------------------------------*/

/**
 * @brief  Main program
 * @param  None
 * @retval None
 */
int main(void)
{
  /*!< At this stage the microcontroller clock setting is already configured,
        this is done through SystemInit() function which is called from startup
        files before to branch to application main.
        To reconfigure the default setting of SystemInit() function,
        refer to system_stm32f4xx.c file */

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

  configASSERT(xTaskCreate(LedTask,
                           "led",
                           configMINIMAL_STACK_SIZE,
                           NULL,
                           tskIDLE_PRIORITY + 1,
                           NULL) == pdPASS);

  vTaskStartScheduler();

  while (1)
  {
  }
}

static void LedTask(void *argument)
{
  (void)argument;

  My_Led_Init();

  for (;;)
  {
    My_Led_Open();
    vTaskDelay(pdMS_TO_TICKS(300));

    My_Led_Close();
    vTaskDelay(pdMS_TO_TICKS(300));
  }
}

#ifdef USE_FULL_ASSERT

/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
  (void)file;
  (void)line;

  taskDISABLE_INTERRUPTS();
  while (1)
  {
  }
}
#endif

/**
 * @}
 */
