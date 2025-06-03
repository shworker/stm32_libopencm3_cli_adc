/*
 * utils.h
 *
 * Project stm32_eth_test1
 *
 * Copyright (C) 2015 OOO NPK V-Real
 *
 *  Created on: Dec 10, 2015
 *      Author: Dmitry Saychenko <sadmitry@gmail.com>
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>

#include <stdint.h>

#define HEX_BYTE_LENGTH			8
#define NUMBER_LENGTH			5
#define UINT32_T_WIDTH			11
#define FLOAT_CHAR_BUFF_SIZE	64

//typedef bool uint8_t;

uint8_t int2hex(uint32_t decimalNumber, char *hexadecimalNumber);
uint8_t dig2ascii(uint8_t dig);
uint32_t hex2int(const char *string);
int myatoi(const char *s);
char* itoa(int num, char* str, int base);
void reverse(char str[], int length);
void swap(char *a, char *b);
void print_hex(uint16_t data);
void print_u32_hex(uint32_t data);
uint8_t StringContainsHexDigits(char *string);
uint8_t ifalldigits(const char *str, int len);
void PrintHex(int value);
int swapNibbles(int x);
void PrintBuffer(uint8_t *Buffer, int len);
void Printf(const char *fmt, ...);
void PrintHexDump(uint8_t *buffer, int length);
/*
 * Bitbanging in RAM macros
 *
 * */

#define varClrBitBB(var, BitNumber)(*(vu32 *)\
(SRAM_BB_BASE | ((((u32)&var) - SRAM_BASE) >> 5) | ((BitNumber) >> 2)) = 0)
#define varSetBitBB(var, BitNumber)(*(vu32 *)\
(SRAM_BB_BASE | ((((u32)&var) - SRAM_BASE) >> 5) | ((BitNumber) >> 2)) = 1)
#define varGetBitBB(var, BitNumber)(*(vu32 *)\
(SRAM_BB_BASE | ((((u32)&var) - SRAM_BASE) >> 5) | ((BitNumber) >> 2)))

#endif /* UTILS_H_ */
