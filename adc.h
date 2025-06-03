/*
 * adc.h
 *
 *  Created on: May 28, 2025
 *      Author: dmitry
 */

#ifndef LIB_ADC_H_
#define LIB_ADC_H_

#include <libopencm3/stm32/adc.h>

typedef enum {
	ADC_STATE_IDLE,
	ADC_STATE_STARTED,
	ADC_STATE_EOC,
	ADC_STATE_COMPLETED
} ADC_State_t;

typedef enum {
	DIFF_TYPE_UNKNOWN,
	DIFF_TYPE_HYST,
	DIFF_TYPE_EQUAL,
	DIFF_TYPE_LESS,
	DIFF_TYPE_GREATHER,
	DIFF_TYPE_FIRSTRUN
} Diff_Type_t;

#define ADC_SAMPLES_COUNT	64

typedef struct  {
	uint8_t channel;
	ADC_State_t state;
	uint8_t index;
	uint16_t temp_data[ADC_SAMPLES_COUNT];
	uint16_t cur_data;
	uint16_t prev_data;
	uint8_t FirstRun;
} Adc_Channel_t;

#define ADC_CHANNELS_COUNT	1

void ADC_My_Init(void);
void ADC_SetChannels(Adc_Channel_t *channels);
void ADC_Task(void);
uint16_t ADC_GetData(uint8_t channel);
ADC_State_t ADC_GetState(uint8_t channel);
uint16_t *ADC_GetBuffer(void);
Diff_Type_t CompareWithSetting(uint8_t channel, uint16_t setting, uint16_t hyst);

#endif /* LIB_ADC_H_ */
