# stm32_libopencm3_cli_adc

This project demonstrate how to use:

1. LibOpenCM3 library
2. microrl library for CLI
3. my ADC library
4. my buttons library
   
How to use:

1. Download and compile libopencm3 library
2. Copy source files from repository to your project foler(s)
3. Open adc.h file and change line:
   #define ADC_CHANNELS_COUNT	1
4. Open main.c file. Change this array:
   static Adc_Channel_t ADC_Channels[ADC_CHANNELS_COUNT] = {
		{ADC_CHANNEL1, ADC_STATE_IDLE, 0, {0}, 0, 0}
};
