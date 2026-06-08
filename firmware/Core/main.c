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
#include "My_Led.h"
#include "My_USART.h"
#include "stm32f4xx.h"

/** @addtogroup Template_Project
 * @{
 */

static void Delay(volatile uint32_t count);
static void AdcSingleInit(void);
static uint16_t AdcSingleRead(void);

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
  AdcSingleInit();

  My_Led_Close();
  printf("\r\nboot ok\r\n");
  printf("adc test on PA0\r\n");

  while (1)
  {
    uint16_t adcValue;

    My_Led_Open();
    adcValue = AdcSingleRead();
    printf("ADC(PA0)=%u\r\n", (unsigned int)adcValue);
    Delay(4000000U);
    My_Led_Close();
    Delay(4000000U);
  }
}

static void Delay(volatile uint32_t count)
{
  while (count-- > 0U)
  {
    __NOP();
  }
}

static void AdcSingleInit(void)
{
  GPIO_InitTypeDef gpioInit;
  ADC_CommonInitTypeDef adcCommonInit;
  ADC_InitTypeDef adcInit;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

  gpioInit.GPIO_Pin = GPIO_Pin_0;
  gpioInit.GPIO_Mode = GPIO_Mode_AN;
  gpioInit.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &gpioInit);

  ADC_CommonStructInit(&adcCommonInit);
  adcCommonInit.ADC_Mode = ADC_Mode_Independent;
  adcCommonInit.ADC_Prescaler = ADC_Prescaler_Div4;
  adcCommonInit.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
  adcCommonInit.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
  ADC_CommonInit(&adcCommonInit);

  ADC_StructInit(&adcInit);
  adcInit.ADC_Resolution = ADC_Resolution_12b;
  adcInit.ADC_ScanConvMode = DISABLE;
  adcInit.ADC_ContinuousConvMode = DISABLE;
  adcInit.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  adcInit.ADC_DataAlign = ADC_DataAlign_Right;
  adcInit.ADC_NbrOfConversion = 1;
  ADC_Init(ADC1, &adcInit);

  ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_144Cycles);
  ADC_Cmd(ADC1, ENABLE);
}

static uint16_t AdcSingleRead(void)
{
  ADC_SoftwareStartConv(ADC1);

  while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET)
  {
  }

  return (uint16_t)ADC_GetConversionValue(ADC1);
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
