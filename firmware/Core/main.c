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
#include <stdio.h>
#include "My_ADC_DMA.h"
#include "My_Led.h"
#include "My_USART.h"
#include "stm32f4xx.h"

/** @addtogroup Template_Project
 * @{
 */

static void SendOneFrame(void);

/**
 * @brief  Main program
 * @param  None
 * @retval None
 */
int main(void)
{
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

  My_Led_Init();
  My_USART_Init();
  My_ADC_DMA_Init();

  My_Led_Close();
  printf("\r\nrain capture init\r\n");
  printf("sample_rate=%lu frame_points=%u\r\n",
         (unsigned long)ADC_SAMPLE_RATE_HZ,
         (unsigned int)My_ADC_DMA_GetFrameLength());

  My_ADC_DMA_Start();

  while (1)
  {
    if (My_ADC_DMA_FrameReady() != 0U)
    {
      My_Led_Open();
      SendOneFrame();
      My_ADC_DMA_ClearFrameReady();
      My_Led_Close();
      My_ADC_DMA_Start();
    }
  }
}

static void SendOneFrame(void)
{
  const uint16_t *frame = My_ADC_DMA_GetBuffer();
  uint16_t index;

  printf("FRAME_BEGIN,%u\r\n", (unsigned int)My_ADC_DMA_GetFrameLength());

  for (index = 0; index < My_ADC_DMA_GetFrameLength(); index++)
  {
    printf("%u\r\n", (unsigned int)frame[index]);
  }

  printf("FRAME_END\r\n");
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

  while (1)
  {
  }
}
#endif

/**
 * @}
 */
