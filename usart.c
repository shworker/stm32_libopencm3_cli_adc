/*
 * usart.c
 *
 * Project stm32_usart_we_test
 *
 * Copyright (C) 2017 OOO NPL V-Real
 *
 *  Created on: Nov 23, 2017
 *      Author: Dmitry Saychenko <sadmitry@gmail.com>
 */

#include "../lib/usart.h"

#include "string.h"

volatile uint8_t rx_buffer[RX_BUFFER_SIZE];
volatile unsigned int rx_wr_index = 0, rx_rd_index = 0, rx_counter = 0;
volatile uint8_t rx_buffer_overflow = 0;

volatile uint8_t tx_buffer[TX_BUFFER_SIZE];
volatile unsigned int tx_wr_index = 0, tx_rd_index = 0, tx_counter = 0;

void usart_setup(uint32_t baudrate)
{
	/* Enable the USART2 interrupt. */
	nvic_enable_irq(NVIC_USART2_IRQ);

	/* Setup GPIO pin GPIO_USART2_TX on GPIO port A for transmit. */
	gpio_set_mode(GPIO_BANK_USART2_TX, GPIO_MODE_OUTPUT_50_MHZ,
			GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART2_TX);

	/* Setup GPIO pin GPIO_USART2_RX on GPIO port A for receive. */
	gpio_set_mode(GPIO_BANK_USART2_RX, GPIO_MODE_INPUT,
			GPIO_CNF_INPUT_FLOAT, GPIO_USART2_RX);

	/* Setup UART parameters. */
	usart_set_baudrate(USART2, baudrate);
	usart_set_databits(USART2, 8);
	usart_set_stopbits(USART2, USART_STOPBITS_1);
	usart_set_parity(USART2, USART_PARITY_NONE);
	usart_set_flow_control(USART2, USART_FLOWCONTROL_NONE);
	usart_set_mode(USART2, USART_MODE_TX_RX);

	/* Enable USART2 Receive interrupt. */
	USART_CR1(USART2) |= USART_CR1_RXNEIE;

	/* Finally enable the USART. */
	usart_enable(USART2);
}

void usart2_isr(void)
{
	/* Check if we were called because of RXNE. */
	if (((USART_CR1(USART2) & USART_CR1_RXNEIE) != 0) &&
			((USART_SR(USART2) & USART_SR_RXNE) != 0)) {

		rx_buffer[rx_wr_index++] = usart_recv(USART2);

		if (rx_wr_index == RX_BUFFER_SIZE) rx_wr_index=0;
		if (++rx_counter == RX_BUFFER_SIZE)
		{
			rx_counter=0;
			rx_buffer_overflow=1;
		}
	}

	/* Check if we were called because of TXE. */
	if (((USART_CR1(USART2) & USART_CR1_TXEIE) != 0) &&
			((USART_SR(USART2) & USART_SR_TXE) != 0)) {
		if (tx_counter)
		{
			--tx_counter;
			/* Put data into the transmit register. */
			usart_send(USART2, tx_buffer[tx_rd_index++]);
			if (tx_rd_index == TX_BUFFER_SIZE) tx_rd_index=0;
		}
		else
		{
			/* Disable the TXE interrupt as we don't need it anymore. */
			USART_CR1(USART2) &= ~USART_CR1_TXEIE;
		}
	}
}

void CLI_USART_init(uint32_t baudrate){
	usart_setup(baudrate);
}

uint8_t CLI_Get_Received(void){ return (rx_counter); }

uint8_t CLI_Get_Char(void){
	uint8_t data;

	data = rx_buffer[rx_rd_index++];

	if (rx_rd_index == RX_BUFFER_SIZE) rx_rd_index = 0;

	USART_CR1(USART2) &= ~(USART_CR1_RXNEIE);
	--rx_counter;
	USART_CR1(USART2) |= USART_CR1_RXNEIE;

	return data;
}

void CLI_Put_Char(uint8_t c){
	while (tx_counter == TX_BUFFER_SIZE){}

	USART_CR1(USART2) &= ~USART_CR1_TXEIE;
	if (tx_counter || ((USART_CR1(USART2) & USART_CR1_TXEIE) == 0))
	{
		tx_buffer[tx_wr_index++] = c;
		if (tx_wr_index == TX_BUFFER_SIZE) tx_wr_index = 0;
		++tx_counter;
		USART_CR1(USART2) |= USART_CR1_TXEIE;
	}
	else
		usart_send(CLI_USART, c);
}

void CLI_Put_Str(const char *s){
	while (*s != 0){
		CLI_Put_Char(*s++);
	}
}

void CLI_Put_Int(int32_t data){
	unsigned char temp[10],count=0;
	if (data<0)
	{
		data=-data;
		CLI_Put_Char('-');
	}

	if (data)
	{
		while (data)
		{
			temp[count++]=data%10+'0';
			data/=10;
		}

		while (count)
		{
			CLI_Put_Char(temp[--count]);
		}
	}
	else CLI_Put_Char('0');
}

void CLI_Print(const char * string){
	CLI_Put_Str(string);
}

void CLI_Send(uint8_t c){
	CLI_Put_Char(c);
}

uint8_t CLI_Getchar(void){
	uint8_t data = 0;
	if(CLI_Get_Received()){
		data = CLI_Get_Char();
	}
	return data;
}

void CLI_Print_Length(const char * string, uint8_t length){
	uint8_t l = strlen(string);
	uint8_t i;

	CLI_Print(string);

	if(l < length){
		for(i = 0; i < (length - l); i++){
			CLI_Send(' ');
		}
	}
}

