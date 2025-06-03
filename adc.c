/*
 * adc.c
 *
 *  Created on: May 28, 2025
 *      Author: dmitry
 */

#include <string.h>
#include <stdlib.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/nvic.h>
#include "adc.h"

// Pointer to channels array
static Adc_Channel_t *Adc_Channels;
// Array of channel numbers for adc_set_regular_sequence()
static uint8_t ADC_Channels_Set[ADC_CHANNELS_COUNT];
// Index in channels array
static int ADC_Current_Channel;

/********************/
/* Helper functions */
/********************/
static void ADC_SearchAndZeroMinMax(uint16_t *array, size_t size){
	size_t min_index, max_index;

	min_index = 0;
	max_index = 0;

	for(size_t i = 0; i < size; i++){
		if(array[i] < array[min_index]){ min_index = i; }
		if(array[i] > array[max_index]){ max_index = i; }
	}

	array[min_index] = 0;
	array[max_index] = 0;
}

static int ADC_CalculateAverage(uint16_t *array, size_t size){
	unsigned long Summa = 0;
	for(size_t i = 0; i < size; i++){
		Summa += array[i];
	}

	return (Summa / size);
}

/*
 * Filter ADC values
 *
 * 1. search for maximum and minimum array values
 * 2. Set this values to zero
 * 3. Calculating the average
 *
 * */
static int ADC_Filter(uint16_t *array, size_t size){
	ADC_SearchAndZeroMinMax(array, size);
	return ADC_CalculateAverage(array, size);
}

static void ADC_StartNewConversion(void){
	adc_set_sample_time(ADC1, Adc_Channels[ADC_Current_Channel].channel, ADC_SMPR_SMP_13DOT5CYC);
	adc_enable_eoc_interrupt(ADC1);
	nvic_enable_irq(NVIC_ADC1_2_IRQ);
	adc_start_conversion_regular(ADC1);
}

/*
 * ADC interrupt handler
 *
 * */
void adc1_2_isr(void){
	// disable interrupt
	adc_disable_eoc_interrupt(ADC1);
	// if A/D convertion is complete
	if(adc_eoc(ADC1)){
		// Read value from ADC data register and put it into buffer
		Adc_Channels[ADC_Current_Channel].temp_data[Adc_Channels[ADC_Current_Channel].index] = \
				adc_read_regular(ADC1);
		// Increase buffer index
		Adc_Channels[ADC_Current_Channel].index++;
		// if buffer is full
		if(ADC_SAMPLES_COUNT == Adc_Channels[ADC_Current_Channel].index){
			// set COMPLETED state
			Adc_Channels[ADC_Current_Channel].state = ADC_STATE_COMPLETED;
			// Zero buffer index
			Adc_Channels[ADC_Current_Channel].index = 0;
		}else{
			// Start new A/D convertion
			ADC_StartNewConversion();
			// set STARTED state
			Adc_Channels[ADC_Current_Channel].state = ADC_STATE_STARTED;
		}
	}
}

void ADC_Hardware_Init(void){
	rcc_periph_clock_enable(RCC_ADC1);
	adc_power_off(ADC1);
	rcc_periph_reset_pulse(RST_ADC1);
	rcc_set_adcpre(RCC_CFGR_ADCPRE_PCLK2_DIV4);
	adc_set_dual_mode(ADC_CR1_DUALMOD_IND);
	adc_disable_scan_mode(ADC1);
	adc_set_single_conversion_mode(ADC1);
	adc_enable_external_trigger_regular(ADC1, ADC_CR2_EXTSEL_SWSTART);
	adc_set_right_aligned(ADC1);
	adc_power_on(ADC1);
	adc_reset_calibration(ADC1);
	adc_calibrate(ADC1);
}

void ADC_My_Init(void){
	ADC_Current_Channel = 0;
	ADC_Hardware_Init();
}

/*
 * Set pointer to channels array
 * This function must be called from main()
 *
 * */
void ADC_SetChannels(Adc_Channel_t *channels){
	Adc_Channels = channels;
	// fill in channels array
	for(int i = 0; i < ADC_CHANNELS_COUNT; i++){
		ADC_Channels_Set[i] = Adc_Channels[i].channel;
	}
	adc_set_regular_sequence(ADC1, ADC_CHANNELS_COUNT, ADC_Channels_Set);
}

/*
 * This function must be called periodically from main() ( 50 ms in my case )
 *
 * */
void ADC_Task(void){
	uint8_t i;
	int result;
	unsigned long Temp = 0;

	if(ADC_STATE_IDLE == Adc_Channels[ADC_Current_Channel].state){
		Adc_Channels[ADC_Current_Channel].index = 0;
		ADC_StartNewConversion();
		Adc_Channels[ADC_Current_Channel].state = ADC_STATE_STARTED;
	}
	if(ADC_STATE_COMPLETED == Adc_Channels[ADC_Current_Channel].state){
		result = ADC_Filter(Adc_Channels[ADC_Current_Channel].temp_data, ADC_SAMPLES_COUNT);
		Adc_Channels[ADC_Current_Channel].cur_data = result;
		Adc_Channels[ADC_Current_Channel].state = ADC_STATE_IDLE;
		ADC_Current_Channel++;
		if(ADC_CHANNELS_COUNT == ADC_Current_Channel){ ADC_Current_Channel = 0; }
	}
}

uint16_t ADC_GetData(uint8_t channel){ return Adc_Channels[channel].cur_data; }
ADC_State_t ADC_GetState(uint8_t channel){ return Adc_Channels[channel].state; }
uint16_t *ADC_GetBuffer(void){ return Adc_Channels[ADC_Current_Channel].temp_data; }

Diff_Type_t CompareWithSetting(uint8_t channel, uint16_t setting, uint16_t hyst){
	if(Adc_Channels[channel].cur_data > 0){
		// Calculate the difference between the current and previous value
		int diff = Adc_Channels[channel].cur_data - Adc_Channels[channel].prev_data;
		// Store the current value as previous value
		Adc_Channels[channel].prev_data = Adc_Channels[channel].cur_data;
		// If the absolute value of the difference is less than the hysteresis value
		if(abs(diff) < hyst){
			// We inside hysteresis value
			return DIFF_TYPE_HYST;
		}

		// Calculate the difference between the current and setting value
		diff = Adc_Channels[channel].cur_data - setting;
		if(diff > 0){ return DIFF_TYPE_GREATHER; }
		if(diff < 0){ return DIFF_TYPE_LESS; }
		if(diff == 0){ return DIFF_TYPE_EQUAL; }
	}else{
		// First run. Current value is zero
		return DIFF_TYPE_FIRSTRUN;
	}
	return DIFF_TYPE_UNKNOWN;
}
