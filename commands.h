/*
 * commands.h
 *
 * Project stm32sms
 *
 * Copyright (C) 2017 OOO NPK V-Real
 *
 *  Created on: Jun 2, 2017
 *      Author: Dmitry Saychenko <sadmitry@gmail.com>
 */

#ifndef COMMANDS_H_
#define COMMANDS_H_

#include <inttypes.h>

#define COMMAND_NOT_FOUND		0
#define COMMAND_FOUND			1
#define COMMAND_WRONG_ARGC		2

// Function for execute command
typedef void (*CMDFUNC)(int argc, const char * const * argv);

// Function for print string
typedef void (*PRINTFUNC)(const char *string);

// Command item.
// Last command in list MUST BE: {NULL,NULL,NULL} !!!
typedef struct {
	const char *name;
	const char *help_text;
	const char *description;
	uint8_t argc;
	CMDFUNC handler;
} Command_t;

void InitCommands(PRINTFUNC print_func, Command_t *cmd_array);
void Print_Help(void);
//uint8_t Exec(uint8_t argc, const char *command, const char *argv);
Command_t *Find_Command(int argc, const char * const * argv);
void PrintUsage(const char *name, uint8_t ParamCount);

#endif /* COMMANDS_H_ */
