/*
 * utils.c
 *
 * Project stm32_eth_test1
 *
 * Copyright (C) 2015 OOO NPK V-Real
 *
 *  Created on: Dec 10, 2015
 *      Author: Dmitry Saychenko <sadmitry@gmail.com>
 */
#include <stdarg.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "utils.h"

#include "../lib/usart.h"

uint8_t int2hex(uint32_t decimalNumber, char *hexadecimalNumber){
	uint32_t quotient;
	uint8_t i = 0;
	uint8_t temp;
	quotient = decimalNumber;
	while(quotient != 0) {
		temp = quotient % 16;
		//To convert integer into character
		if( temp < 10)
			temp = temp + 48;
		else
			temp = temp + 55;
		hexadecimalNumber[i++]= temp;
		quotient = quotient / 16;
	}
	if(0 == decimalNumber){
		hexadecimalNumber[0] = '0';
		hexadecimalNumber[1] = '0';
	}
	return i;
}

uint8_t dig2ascii(uint8_t dig){
	return (0x30 + dig);
}

uint32_t hex2int(const char *string){
	uint32_t number;
	if(('0' == string[0]) && ('x' == string[1])){
		// string begin with 0x
		number = (uint32_t)strtol(string, NULL, 0);
	}else{
		number = (uint32_t)strtol(string, NULL, 16);
	}
	return number;
}

int myatoi(const char *s)
{
	int i, n=0;
	for(i=0; s[i]>='0' && s[i]<='9'; i++)
		n = 10*n + (s[i] - '0');
	return n;
}

void swap(char *a, char *b)
{
	char z = *a;
	*a = *b;
	*b = z;
}

/* A utility function to reverse a string  */
void reverse(char str[], int length)
{
	int start = 0;
	int end = length -1;
	while (start < end)
	{
		swap((str+start), (str+end));
		start++;
		end--;
	}
}

// Implementation of itoa()
char* itoa(int num, char* str, int base)
{
	int i = 0;
	_Bool isNegative = false;

	/* Handle 0 explicitely, otherwise empty string is printed for 0 */
	if (num == 0)
	{
		str[i++] = '0';
		str[i] = '\0';
		return str;
	}

	// In standard itoa(), negative numbers are handled only with
	// base 10. Otherwise numbers are considered unsigned.
	if (num < 0 && base == 10)
	{
		isNegative = true;
		num = -num;
	}

	// Process individual digits
	while (num != 0)
	{
		int rem = num % base;
		str[i++] = (rem > 9)? (rem-10) + 'a' : rem + '0';
		num = num/base;
	}

	// If number is negative, append '-'
	if (isNegative)
		str[i++] = '-';

	str[i] = '\0'; // Append string terminator

	// Reverse the string
	reverse(str, i);

	return str;
}

uint8_t ifalldigits(const char *str, int len){
	uint8_t i;

	for (i = 0; i < len; i++) {
		if(!isdigit((int)str[i])){ return 1; }
	}
	return 0;
}

void PrintHex(int value){
	uint8_t HexStr[5];
	memset(HexStr, 0, 5);
	int2hex(value, HexStr);
	CLI_Print("0x");
	CLI_Print(HexStr);
}

int swapNibbles(int x)
{
	return ( (x & 0x0F) << 4 | (x & 0xF0) >> 4 );
}

// Implementation of citoa()
char* citoa(int num, char* str, int base)
{
	int i = 0;
	bool isNegative = false;

	/* Handle 0 explicitly, otherwise empty string is
	 * printed for 0 */
	if (num == 0) {
		str[i++] = '0';
		str[i] = '\0';
		return str;
	}

	// In standard itoa(), negative numbers are handled
	// only with base 10. Otherwise numbers are
	// considered unsigned.
	if (num < 0 && base == 10) {
		isNegative = true;
		num = -num;
	}

	// Process individual digits
	while (num != 0) {
		int rem = num % base;
		str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
		num = num / base;
	}

	// if number of characters is odd - add '0'
	if(0 != (i % 2)){
		str[i++] = '0';
	}

	// If number is negative, append '-'
	if (isNegative)
		str[i++] = '-';

	str[i] = '\0'; // Append string terminator

	// Reverse the string
	reverse(str, i);

	return str;
}

void PrintBuffer(uint8_t *Buffer, int len){
	int i;

	CLI_Print("[ ");
	for(i = 0; i < len; i++){
		Printf("xc", Buffer[i], ' ');
	}
	CLI_Print("]");
}

void Printf(const char *fmt, ...){
	va_list ap;
	int d;
	char c;
	char *s;
	char hexstr[16];

	va_start(ap, fmt);

	while (*fmt){
		switch (*fmt++) {
		case 's':              /* string */
			s = va_arg(ap, char *);
			CLI_Print(s);
			break;
		case 'd':              /* int */
			d = va_arg(ap, int);
			CLI_Put_Int(d);
			break;
		case 'c':              /* char */
			/* need a cast here since va_arg only
               takes fully promoted types */
			c = (char) va_arg(ap, int);
			CLI_Put_Char(c);
			break;
		case 'x':
			d = va_arg(ap, int);
			CLI_Print("0x");
			CLI_Print(citoa(d, hexstr, 16));
			break;
		}
	}
	va_end(ap);
}

void PrintHexDump(uint8_t *buffer, int length){
	int i;
	int remain;

	for(i = 0; i < length; i++){
		if(0 == (7 % i)){ CLI_Put_Char(' '); }
		if(0 == (15 % i)){ CLI_Print("\r\n"); }
		PrintHex(buffer[i]); CLI_Put_Char(' ');
	}
}

