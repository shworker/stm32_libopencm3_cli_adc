/*
 * usart.h
 *
 * Project stm32_usart_we_test
 *
 * Copyright (C) 2017 OOO NPL V-Real
 *
 *  Created on: Nov 23, 2017
 *      Author: Dmitry Saychenko <sadmitry@gmail.com>
 */

#ifndef USART_H_
#define USART_H_

#include <libopencm3/stm32/usart.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

//#define CLI_Usart_1
#define CLI_Usart_2
//#define CLI_Usart_3

//#define Modem_Usart_3
//#define MODEM_USART		USART3

#define CLI_SPEED		115200
#define MODEM_SPEED		19200

#ifdef CLI_Usart_1
#define CLI_USART	USART1
#endif

#ifdef CLI_Usart_2
#define CLI_USART	USART2
#endif

#ifdef CLI_Usart_3
#define CLI_USART	USART3
#endif

// USART Receiver buffer
#define RX_BUFFER_SIZE 512

// USART Transmitter buffer
#define TX_BUFFER_SIZE 512

uint8_t CLI_Getchar(void);
void CLI_USART_init(uint32_t baudrate);
uint8_t CLI_Get_Received(void);
uint8_t CLI_Get_Char(void);
void CLI_Put_Char(uint8_t c);
void CLI_Put_Str(const char *s);
void CLI_Put_Int(int32_t data);
void CLI_Print(const char * string);
void CLI_Send(uint8_t c);
void CLI_Print_Length(const char * string, uint8_t length);

#endif /* USART_H_ */
