#ifndef MY_USART_H_
#define MY_USART_H_
#include <stm32f4xx.h>
#include <stdio.h>
void My_USART_Init(void);
void My_USART_SendByte(uint8_t data);
void My_USART_SendString(const char *string);
void My_USART_SendBuffer(const uint8_t *buffer, uint16_t length);
int fputc(int ch, FILE *f);
#endif
