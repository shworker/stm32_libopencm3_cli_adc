/*
 * debug.h
 *
 *  Created on: Oct 4, 2024
 *      Author: dmitry
 */

#ifndef LIB_DEBUG_H_
#define LIB_DEBUG_H_

#include <stdint.h>

void Debug_Init(void);
void Debug_SetLevel(uint8_t level);
void Debug_OnOff(uint8_t onoff);
uint8_t Debug_GetState(void);
uint8_t Debug_GetLevel(void);
void Debug_Log(uint8_t level, const char *string);
void Debug_Printf(uint8_t level, const char *fmt, ...);
#endif /* LIB_DEBUG_H_ */
