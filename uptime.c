/*
 * uptime.c
 *
 * Project stm32_uart_onewire_test
 *
 * Copyright (C) 2017 OOO NPK V-Real
 *
 *  Created on: Jul 26, 2017
 *      Author: Dmitry Saychenko <sadmitry@gmail.com>
 */

#include "../lib/uptime.h"

static UpTime_t UpTime;
uint32_t Seconds_Counter;

void UpTimeInit(void){
	UpTime.seconds = 0;
	UpTime.minutes = 0;
	UpTime.hours = 0;
	UpTime.days = 0;

	Seconds_Counter = 0;
}

void UpTimeUpdate(void){
	uint32_t days, hours, minutes;

	Seconds_Counter++;

	uint32_t seconds = Seconds_Counter;

	if(seconds >= 86400){
		days = seconds / 60 / 60 / 24;
	}else{
		days = 0;
	}

	if(seconds >= 60){
		minutes = seconds / 60;
	}else{
		minutes = 0;
	}
	if(seconds >= 3600){
		hours = (minutes / 60) - (24 * days);
	}else{
		hours = 0;
	}

	UpTime.days = days;
	UpTime.hours = hours;
	UpTime.minutes = minutes % 60;
	UpTime.seconds = seconds % 60;
}

uint16_t UpTimeGetSeconds(void) { return UpTime.seconds; }
uint16_t UpTimeGetMinutes(void){ return UpTime.minutes; }
uint16_t UpTimeGetHours(void){ return UpTime.hours; }
uint16_t UpTimeGetDays(void){ return UpTime.days; }
uint32_t UpTimeGetSecondsCounter(void){ return Seconds_Counter; }
