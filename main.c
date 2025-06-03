/*
 * main.c
 *
 *  Created on: Dec 20, 2024
 *      Author: dmitry
 */

#include <string.h>
#include <stdlib.h>

#include <libopencm3/cm3/scb.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/stm32/dma.h>

#include "commands.h"
#include "debug.h"
#include "microrl.h"
#include "microrl_callbacks.h"
#include "usart.h"
#include "utils.h"
#include "uptime.h"
#include "buttons.h"
#include "adc.h"

#define VERSION_MAJOR		1
#define VERSION_MINOR		1

#define LED_ON				1
#define LED_OFF				0

#define DEBUG_OFF			0
#define DEBUG_ON			1

#define BUTTON_GPIO			GPIOC
#define BUTTON_PIN			GPIO13
#define BUTTON_POLL_PERIOD	20
#define BUTTONS_COUNT		1

#define ADC_POLL_PERIOD		50

#define STATUS_LED_GPIO		GPIOA
#define STATUS_LED_PIN		GPIO5
#define STATUS_BLINK_PERIOD	500

#define ANALOG_GPIO			GPIOA
#define ANALOG_PIN			GPIO1

#define LED_MODE_BLINK		1
#define LED_MODE_OFF		2
#define LED_MODE_ON			3

#define CONTMODE_OFF		0
#define CONTMODE_CONT		1

#define DEFAULT_SETTING		1855
#define DEFAULT_HYST		10

volatile unsigned long Timer;
volatile uint8_t SystemFlag;

int Led_Period;
int Led_Mode;
int Led_Flag;
int ContMode;
int ContMode_Old;
int Button_Flag;
int setting;
int hyst;

ButtonState_t ButtonsData[BUTTONS_COUNT];
static Adc_Channel_t ADC_Channels[ADC_CHANNELS_COUNT] = {
		{ADC_CHANNEL1, ADC_STATE_IDLE, 0, {0}, 0, 0}
};
// create microrl object and pointer on it
microrl_t rl;
microrl_t *prl = &rl;

#define ADC_BUFFER_SIZE		16

uint16_t AdcBuffer[ADC_BUFFER_SIZE];

void CmdHyst(int argc, const char * const * argv);
void CmdSetting(int argc, const char * const * argv);
void CmdAdc(int argc, const char * const * argv);
void CmdCont(int argc, const char * const * argv);
void CmdPeriod(int argc, const char * const * argv);
void CmdMode(int argc, const char * const * argv);
void CmdLed(int argc, const char * const * argv);
void CmdDebug(int argc, const char * const * argv);
void CmdUpTime(int argc, const char * const * argv);
void CmdStatus(int argc, const char * const * argv);
void CmdReset(int argc, const char * const * argv);
void CmdHelp(int argc, const char * const * argv);
void CmdVersion(int argc, const char * const * argv);

Command_t Commands[] = {
		{"hyst","Set hysteresis value","hyst N",1,CmdHyst},
		{"hyst","Print hysteresis value","hyst",0,CmdHyst},
		{"setting","Set setting value","setting N",1,CmdSetting},
		{"setting","Print current setting","setting",0,CmdSetting},
		{"adc","Print ADC buffer","adc",0,CmdAdc},
		{"cont","Get continuous mode","cont",0,CmdCont},
		{"cont","Set continuous mode","cont on|off",1,CmdCont},
		{"period","Set blink period","period ###",1,CmdPeriod},
		{"period","Print blink period","period",0,CmdPeriod},
		{"led","Print led status","led",0,CmdLed},
		{"mode","Set led mode","mode b|on|off",1,CmdMode},
		{"debug","Print debug state and level","debug",0,CmdDebug},
		{"debug","Set debug state","debug on|off",1,CmdDebug},
		{"debug","Set debug state and level","debug on|off 0-2",3,CmdDebug},
		{"uptime","Print uptime","uptime",0,CmdUpTime},
		{"status","Print current buttons system","status",0,CmdStatus},
		{"version","Print Armon version","version",0,CmdVersion},
		{"reset","Module reset","reset",0,CmdReset},
		{"help","Print help","help",0,CmdHelp},
		{NULL,NULL,NULL,0,NULL}
};

#define CHK_BIT(number, bit)    ((number & (1 << bit)) >> bit)

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

void PrintADCState(ADC_State_t state){
	if(state == ADC_STATE_IDLE) { CLI_Print("ADC_STATE_IDLE"); }
	if(state == ADC_STATE_STARTED) { CLI_Print("ADC_STATE_STARTED"); }
	if(state == ADC_STATE_EOC) { CLI_Print("ADC_STATE_EOC"); }
	if(state == ADC_STATE_COMPLETED) { CLI_Print("ADC_STATE_COMPLETED"); }
}

void PrintADCStatus(void){
	CLI_Print("ADC data: ");
	CLI_Put_Int(ADC_GetData(0));
	CLI_Print(" ");
	Diff_Type_t diff = CompareWithSetting(0, setting, hyst);
	switch(diff){
	case DIFF_TYPE_HYST:
		CLI_Print("H");
		break;
	case DIFF_TYPE_EQUAL:
		CLI_Print("=");
		break;
	case DIFF_TYPE_LESS:
		CLI_Print("<");
		break;
	case DIFF_TYPE_GREATHER:
		CLI_Print(">");
		break;
	case DIFF_TYPE_FIRSTRUN:
		CLI_Print("F");
		break;
	}
	CLI_Print(" ");
	CLI_Put_Int(setting);
	CLI_Print(" ");
	CLI_Put_Int(hyst);
	CLI_Print("\r\n");
}

void PrintUpTime(void){

	uint32_t d;

	d = UpTimeGetDays(); CLI_Put_Int(d);
	if(1 == d){ CLI_Print(" Day "); } else{ CLI_Print(" Days "); }

	d = UpTimeGetHours(); CLI_Put_Int(d);
	if(1== d){ CLI_Print(" hour "); } else { CLI_Print(" hours "); }

	d = UpTimeGetMinutes(); CLI_Put_Int(d);
	if(1== d) { CLI_Print(" minute "); } else { CLI_Print(" minutes "); }

	d = UpTimeGetSeconds(); CLI_Put_Int(d);
	if(1== d) { CLI_Print(" second"); } else { CLI_Print(" seconds"); }
}

void PrintVersion(void){
	Printf("sdcdsd", "Armon version: ",
			VERSION_MAJOR,'.', VERSION_MINOR,
			" build ", 0);
}

void CmdHyst(int argc, const char * const * argv){
	if(1 == argc){
		CLI_Put_Int(hyst);
	}else{
		if(2 == argc){
			setting  = myatoi(argv[1]);
		}
	}
}

void CmdSetting(int argc, const char * const * argv){
	if(1 == argc){
		CLI_Put_Int(setting);
	}else{
		if(2 == argc){
			setting  = myatoi(argv[1]);
		}
	}
}
void CmdAdc(int argc, const char * const * argv){
	uint16_t *buffer = ADC_GetBuffer();
	for(int i = 0; i < ADC_SAMPLES_COUNT; i++){
		PrintHex(buffer[i]);
		if((i > 1) && (0 == (i % 16))){ CLI_Print("\r\n"); }
		else CLI_Print(" ");
	}
	CLI_Print("\r\n");
}

void CmdCont(int argc, const char * const * argv){
	if(2 == argc){
		if(!strcmp("on", argv[1])){ ContMode = CONTMODE_CONT; }
		if(!strcmp("off", argv[1])){ ContMode = CONTMODE_OFF; }
	}else{
		if(1 == argc){
			CLI_Print("Cont mode ");
			if(CONTMODE_CONT == ContMode){ CLI_Print("on"); }
			if(CONTMODE_OFF == ContMode){ CLI_Print("off"); }
		}
	}
}

void CmdPeriod(int argc, const char * const * argv){

	if(2 == argc){
		Led_Period = atoi(argv[1]);
		if(0 == Led_Period){
			CLI_Print("\r\nUsage: period value\r\n");
		}
	}
	if(1 == argc){
		CLI_Put_Int(Led_Period);
	}
}

void CmdMode(int argc, const char * const * argv){
	if('b' == argv[1][0]){ Led_Mode = LED_MODE_BLINK; }
	if(!strcmp("on", argv[1])){ Led_Mode = LED_MODE_ON; }
	if(!strcmp("off", argv[1])){ Led_Mode = LED_MODE_OFF; }
}

void CmdLed(int argc, const char * const * argv){
	CLI_Print("mode ");
	if(LED_MODE_BLINK == Led_Mode){ CLI_Print("blink"); }
	if(LED_MODE_OFF == Led_Mode){ CLI_Print("off"); }
	if(LED_MODE_OFF == Led_Mode){ CLI_Print("on"); }
}

void CmdDebug(int argc, const char * const * argv){
	uint8_t level;

	if(1 == argc){
		CLI_Print(" Debug ");
		CLI_Print(Debug_GetState() ? "on " : "off");
		Printf("sd"," level ", Debug_GetLevel());
	}else{
		if(argc >= 2){
			if(!strcmp("on", argv[1])){
				Debug_OnOff(DEBUG_ON);
			}
			if(!strcmp("off", argv[1])){
				Debug_OnOff(DEBUG_OFF);
			}
			if(3 == argc){
				if(isdigit(argv[2][0])){
					level = atoi(argv[2]);
					Debug_SetLevel(level);
				}
			}
		}
	}
}

void CmdUpTime(int argc, const char * const * argv){
	PrintUpTime();
}

void CmdStatus(int argc, const char * const * argv){
	PrintADCStatus();
}

void CmdReset(int argc, const char * const * argv){
	SCB_AIRCR = SCB_AIRCR_VECTKEY | SCB_AIRCR_SYSRESETREQ;
	while(1);
}

void CmdHelp(int argc, const char * const * argv){
	Print_Help();
}

void CmdVersion(int argc, const char * const * argv){
	PrintVersion();
}

#pragma GCC diagnostic pop

// MicroRL callbacks
void clitask(void){
	// put received char from USART to microrl lib
	if(CLI_Get_Received()){
		// if received char then
		// put received char from USART to microrl lib
		// without waiting
		microrl_insert_char(prl, CLI_Get_Char());
	}
}

int execute (int argc, const char * const * argv){

	Command_t *cmd;

	CLI_Print("\r\n");

	if(argc > 0){
		cmd = Find_Command(argc, argv);
		if(NULL == cmd){
			CLI_Print("Invalid command !\r\n");
		}else{
			CMDFUNC cmdfunc = cmd->handler;
			(cmdfunc)(argc, argv);
		}
	}

	CLI_Print("\r\n");

	return 0;
}

static void clock_setup(void)
{
	rcc_clock_setup_pll(&rcc_hse_configs[RCC_CLOCK_HSE8_72MHZ]);

	/* Enable GPIOA clock (for LED GPIOs). */
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_GPIOC);

	/* Enable clocks for GPIO port A (for GPIO_USART2_TX) and USART2. */
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_AFIO);
	rcc_periph_clock_enable(RCC_USART2);
}

static void gpio_setup(void)
{
	gpio_set_mode(STATUS_LED_GPIO, GPIO_MODE_OUTPUT_50_MHZ,
			GPIO_CNF_OUTPUT_PUSHPULL, STATUS_LED_PIN);

	gpio_set_mode(BUTTON_GPIO, GPIO_MODE_INPUT,
			GPIO_CNF_INPUT_PULL_UPDOWN, BUTTON_PIN);
	gpio_set(BUTTON_GPIO, BUTTON_PIN);

	gpio_set_mode(ANALOG_GPIO, GPIO_MODE_INPUT, GPIO_CNF_INPUT_ANALOG, ANALOG_PIN);
}

void sys_tick_handler(void)
{
	SystemFlag = 1;
	Timer++;
}

void SysTick_Setup(void){
	/* 72MHz / 8 => 9000000 counts per second */
	systick_set_clocksource(STK_CSR_CLKSOURCE_AHB_DIV8);

	/* 9000000/9000 = 1000 overflows per second - every 1ms one interrupt */
	/* SysTick interrupt every N clock pulses: set reload to N-1 */
	systick_set_reload(8999);

	systick_interrupt_enable();

	/* Start counting. */
	systick_counter_enable();
}

void ButtonsCallback(int i, uint8_t state){
	if(BUTTON_STATE_PRESSED == state){
		//		PrintHex(GPIO_IDR(GPIOC));
		//		Printf("sds", " ", i, " [P] ");
		CLI_Print(" [P] ");
		if(CONTMODE_CONT == ContMode){
			ContMode = CONTMODE_OFF;
		}
		else {
			ContMode = CONTMODE_CONT;
		}
		CLI_Print("\r\n");
	}
	if(BUTTON_STATE_RELEASED == state){
		//		PrintHex(GPIO_IDR(GPIOC));
		//		Printf("sds", " ", i, " [R] ");
		CLI_Print(" [R]\r\n");
	}
}

uint8_t ButtonReadCallback(int i){
	uint8_t state;
	state = CHK_BIT(GPIO_IDR(GPIOC), 13);
	return (state);
}

int main(void)
{
	char c;
	unsigned long CurTimer;

	SystemFlag = 0;
	Timer = 0;
	Button_Flag = 0;

	Led_Period = STATUS_BLINK_PERIOD;
	Led_Mode = LED_MODE_BLINK;
	ContMode = CONTMODE_OFF;
	ContMode_Old = CONTMODE_OFF;

	setting = DEFAULT_SETTING;
	hyst = DEFAULT_HYST;

	clock_setup();
	UpTimeInit();
	SysTick_Setup();

	AFIO_MAPR |= AFIO_MAPR_SWJ_CFG_JTAG_OFF_SW_ON;

	gpio_setup();

	memset(ButtonsData, 0, sizeof(ButtonsData));

	SetButtons(ButtonsData, BUTTONS_COUNT);
	SetButtonCallbacks(ButtonsCallback, ButtonReadCallback);

	CLI_USART_init(115200);

	microrl_init(prl, CLI_Print);
	microrl_set_execute_callback(prl, execute);
	InitCommands(CLI_Print, Commands);

	ADC_My_Init();
	ADC_SetChannels(ADC_Channels);

	Printf("sdcds","\r\nArmon version: ", VERSION_MAJOR, '.', VERSION_MINOR, "\r\n");
	CLI_Print("(C) 2024-2025 Dmitry Saychenko <sadmitry@v-real.ru>\r\n");
	CLI_Print("Type help for help\r\n");
	CLI_Print("Start !\r\n");

	while (1){
		if(1 == SystemFlag){
			SystemFlag = 0;
			CurTimer = Timer;

			if(0 == (CurTimer % BUTTON_POLL_PERIOD)){
				PollButtons();
			}
			if(0 == (CurTimer % ADC_POLL_PERIOD)){
				ADC_Task();
			}
			if(LED_MODE_ON == Led_Mode){
				gpio_set(STATUS_LED_GPIO, STATUS_LED_PIN);
			}
			if(LED_MODE_OFF == Led_Mode){
				gpio_clear(STATUS_LED_GPIO, STATUS_LED_PIN);
			}
			if(LED_MODE_BLINK == Led_Mode){
				if(0 == (CurTimer % Led_Period)){
					if(Led_Flag){
						gpio_set(STATUS_LED_GPIO, STATUS_LED_PIN);
					}else{
						gpio_clear(STATUS_LED_GPIO, STATUS_LED_PIN);
					}
					Led_Flag = (!Led_Flag);
				}
			}

			if(0 == (CurTimer % 1000)){
				UpTimeUpdate();
				if(CONTMODE_CONT == ContMode){ PrintADCStatus(); }
			}
		}
		clitask();
	}

	return 0;
}




