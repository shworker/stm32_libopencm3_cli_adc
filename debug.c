/*
 * debug.c
 *
 *  Created on: Oct 4, 2024
 *      Author: dmitry
 */

#include "debug.h"

#include <stdarg.h>

#include "usart.h"
#include "utils.h"

static uint8_t DebugFlag;
static uint8_t DebugLevel;

void Debug_Init(void){
	DebugFlag = 0;
	DebugLevel = 0;
}

void Debug_SetLevel(uint8_t level){ DebugLevel = level; }

void Debug_OnOff(uint8_t onoff){ DebugFlag = onoff; }

uint8_t Debug_GetState(void){ return DebugFlag; }
uint8_t Debug_GetLevel(void){ return DebugLevel; }

void Debug_Log(uint8_t level, const char *string){
	if(DebugFlag){
		if(level >= DebugLevel){
			CLI_Print(string);
		}
	}
}

void Debug_Printf(uint8_t level, const char *fmt, ...){
	va_list ap;
	int d;
	char c;
	char *s;

    va_start ( ap, fmt );

	if(DebugFlag){
		if(level >= DebugLevel){
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
					PrintHex(d);
				}
			}
		}
	}
	va_end(ap);
}
