# stm32_libopencm3_cli_adc

This project demonstrate how to use:

1. LibOpenCM3 library
2. microrl library for CLI
3. my ADC library
4. my buttons library
   
How to use:

1. Download and compile libopencm3 library
2. Copy source files from repository to your project foler(s)
3. Маке some changes depending on the number of channels:
4. Open adc.h file and change line:
```
   #define ADC_CHANNELS_COUNT	1
```
6. Open main.c file. Change this array:
```
   static Adc_Channel_t ADC_Channels[ADC_CHANNELS_COUNT] = {
		{ADC_CHANNEL1, ADC_STATE_IDLE, 0, {0}, 0, 0}
};
```
I use Nucleo F103RB board.
Connect board to PC and upload compiled project to it.
Open terminal emulation program and configure it to 115200,8n1
When console opens, press Enter button.
You see:
```
ARmon >
```
Use help command to get list of commands and description
User button switches between continuous printing ADC values to console on or off
