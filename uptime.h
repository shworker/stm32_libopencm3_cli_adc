/*
 * uptime.h
 *
 * Project stm32_uart_onewire_test
 *
 * Copyright (C) 2017 OOO NPK V-Real
 *
 *  Created on: Jul 26, 2017
 *      Author: Dmitry Saychenko <sadmitry@gmail.com>
 */

#ifndef UPTIME_H_
#define UPTIME_H_

#include <stdint.h>

typedef struct {
	uint32_t seconds;
	uint32_t minutes;
	uint32_t hours;
	uint32_t days;
} UpTime_t;

#define SECONDS_IN_DAY		86400

void UpTimeInit(void);
void UpTimeUpdate(void);
uint16_t UpTimeGetSeconds(void);
uint16_t UpTimeGetMinutes(void);
uint16_t UpTimeGetHours(void);
uint16_t UpTimeGetDays(void);
uint32_t UpTimeGetSecondsCounter(void);

#endif /* UPTIME_H_ */
