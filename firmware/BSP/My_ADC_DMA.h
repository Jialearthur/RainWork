#ifndef __MY_ADC_DMA_H
#define __MY_ADC_DMA_H

#include "stm32f4xx.h"

#define ADC_FRAME_SIZE 600U
#define ADC_SAMPLE_RATE_HZ 200000U

void My_ADC_DMA_Init(void);
void My_ADC_DMA_Start(void);
void My_ADC_DMA_Stop(void);
uint8_t My_ADC_DMA_FrameReady(void);
void My_ADC_DMA_ClearFrameReady(void);
const uint16_t *My_ADC_DMA_GetBuffer(void);
uint16_t My_ADC_DMA_GetFrameLength(void);
void My_ADC_DMA_IRQHandler(void);

#endif
