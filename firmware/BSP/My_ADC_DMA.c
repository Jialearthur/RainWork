#include "My_ADC_DMA.h"

static __IO uint16_t g_adcFrame[ADC_FRAME_SIZE];
static __IO uint8_t g_frameReady;

static void My_ADC_GPIO_Init(void)
{
    GPIO_InitTypeDef gpioInit;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

    gpioInit.GPIO_Pin = GPIO_Pin_0;
    gpioInit.GPIO_Mode = GPIO_Mode_AN;
    gpioInit.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &gpioInit);
}

static void My_ADC_DMA_Config(void)
{
    DMA_InitTypeDef dmaInit;
    NVIC_InitTypeDef nvicInit;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);

    DMA_DeInit(DMA2_Stream0);
    while (DMA_GetCmdStatus(DMA2_Stream0) != DISABLE)
    {
    }

    dmaInit.DMA_Channel = DMA_Channel_0;
    dmaInit.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
    dmaInit.DMA_Memory0BaseAddr = (uint32_t)g_adcFrame;
    dmaInit.DMA_DIR = DMA_DIR_PeripheralToMemory;
    dmaInit.DMA_BufferSize = ADC_FRAME_SIZE;
    dmaInit.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    dmaInit.DMA_MemoryInc = DMA_MemoryInc_Enable;
    dmaInit.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    dmaInit.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    dmaInit.DMA_Mode = DMA_Mode_Normal;
    dmaInit.DMA_Priority = DMA_Priority_High;
    dmaInit.DMA_FIFOMode = DMA_FIFOMode_Disable;
    dmaInit.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
    dmaInit.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    dmaInit.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    DMA_Init(DMA2_Stream0, &dmaInit);

    DMA_ClearFlag(DMA2_Stream0, DMA_FLAG_TCIF0 | DMA_FLAG_TEIF0 | DMA_FLAG_HTIF0);
    DMA_ITConfig(DMA2_Stream0, DMA_IT_TC, ENABLE);

    nvicInit.NVIC_IRQChannel = DMA2_Stream0_IRQn;
    nvicInit.NVIC_IRQChannelPreemptionPriority = 1;
    nvicInit.NVIC_IRQChannelSubPriority = 0;
    nvicInit.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvicInit);
}

static void My_TIM2_Init(void)
{
    TIM_TimeBaseInitTypeDef timInit;
    uint32_t timerClock = 84000000U;
    uint32_t period = (timerClock / ADC_SAMPLE_RATE_HZ) - 1U;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    TIM_TimeBaseStructInit(&timInit);
    timInit.TIM_Period = period;
    timInit.TIM_Prescaler = 0;
    timInit.TIM_ClockDivision = TIM_CKD_DIV1;
    timInit.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &timInit);

    TIM_SelectOutputTrigger(TIM2, TIM_TRGOSource_Update);
    TIM_Cmd(TIM2, DISABLE);
}

static void My_ADC1_Init(void)
{
    ADC_CommonInitTypeDef adcCommonInit;
    ADC_InitTypeDef adcInit;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

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
    adcInit.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_Rising;
    adcInit.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T2_TRGO;
    adcInit.ADC_DataAlign = ADC_DataAlign_Right;
    adcInit.ADC_NbrOfConversion = 1;
    ADC_Init(ADC1, &adcInit);

    ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_15Cycles);
    ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);
    ADC_DMACmd(ADC1, ENABLE);
}

void My_ADC_DMA_Init(void)
{
    My_ADC_GPIO_Init();
    My_ADC_DMA_Config();
    My_TIM2_Init();
    My_ADC1_Init();
    g_frameReady = 0;
}

void My_ADC_DMA_Start(void)
{
    g_frameReady = 0;

    DMA_Cmd(DMA2_Stream0, DISABLE);
    while (DMA_GetCmdStatus(DMA2_Stream0) != DISABLE)
    {
    }

    DMA_SetCurrDataCounter(DMA2_Stream0, ADC_FRAME_SIZE);
    DMA_ClearFlag(DMA2_Stream0, DMA_FLAG_TCIF0 | DMA_FLAG_TEIF0 | DMA_FLAG_HTIF0);

    ADC_Cmd(ADC1, ENABLE);
    DMA_Cmd(DMA2_Stream0, ENABLE);
    TIM_SetCounter(TIM2, 0);
    TIM_Cmd(TIM2, ENABLE);
}

void My_ADC_DMA_Stop(void)
{
    TIM_Cmd(TIM2, DISABLE);
    DMA_Cmd(DMA2_Stream0, DISABLE);
    ADC_Cmd(ADC1, DISABLE);
}

uint8_t My_ADC_DMA_FrameReady(void)
{
    return g_frameReady;
}

void My_ADC_DMA_ClearFrameReady(void)
{
    g_frameReady = 0;
}

const uint16_t *My_ADC_DMA_GetBuffer(void)
{
    return (const uint16_t *)g_adcFrame;
}

uint16_t My_ADC_DMA_GetFrameLength(void)
{
    return ADC_FRAME_SIZE;
}

void My_ADC_DMA_IRQHandler(void)
{
    if (DMA_GetITStatus(DMA2_Stream0, DMA_IT_TCIF0) != RESET)
    {
        DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_TCIF0);
        My_ADC_DMA_Stop();
        g_frameReady = 1;
    }
}
