/*
 * commands.c
 *
 * Project stm32sms
 *
 * Copyright (C) 2017 OOO NPK V-Real
 *
 *  Created on: Jun 2, 2017
 *      Author: Dmitry Saychenko <sadmitry@gmail.com>
 */

#include "../lib/commands.h"

#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "../lib/usart.h"

static PRINTFUNC printfunc;
static Command_t *cmdarray;

void InitCommands(PRINTFUNC print_func, Command_t *cmd_array){
	printfunc = print_func;
	cmdarray = cmd_array;
}

void Print_Help(void){
	Command_t *cmd = cmdarray;
	while(cmd->name != NULL){
		(printfunc)(cmd->description);
		(printfunc)("\t");
		(printfunc)(cmd->help_text);
		(printfunc)("\r\n");
		cmd++;
	}
}

void PrintUsage(const char *name, uint8_t ParamCount){
	Command_t *cmd = cmdarray;
	uint8_t found = COMMAND_NOT_FOUND;
//	uint8_t cmd_found = 0;

	while(cmd->name != NULL){
		if(!strcmp(cmd->name, name)){
//			cmd_found = 1;
			if((ParamCount - 1) == cmd->argc){
				found = COMMAND_FOUND;
				break;
			}else{
				cmd++;
				continue;
			}
		}
//		else{
//			cmd_found = 0;
//		}
		cmd++;
	}

	if(COMMAND_FOUND == found){
		CLI_Print("Usage: ");
		CLI_Print(cmd->help_text);
		CLI_Print("\r\n");
	}
}

//uint8_t Exec(uint8_t argc, const char *command, const char *argv){
//	// Find command
//	Command_t *cmd = cmdarray;
//	uint8_t found = COMMAND_NOT_FOUND;
//	uint8_t cmd_found = 0;
//	while(cmd->name != NULL){
//		if(!strcmp(cmd->name, command)){
//			cmd_found = 1;
//			if(argc == cmd->argc){
//				found = COMMAND_FOUND;
//				break;
//			}else{
//				cmd++;
//				continue;
//			}
//		}else{
//			cmd_found = 0;
//		}
//		cmd++;
//	}
//
//	// If found execute command
//	if(COMMAND_FOUND == found){
//		CMDFUNC cmdfunc = cmd->handler;
//		(cmdfunc)(argv);
//	}else{
//		if(cmd_found){ found = COMMAND_WRONG_ARGC; }
//	}
//
//	return found;
//}

Command_t *Find_Command(int argc, const char * const * argv){

	Command_t *cmd = cmdarray;
	uint8_t found = COMMAND_NOT_FOUND;
//	uint8_t cmd_found = 0;

	while(cmd->name != NULL){
		if(!strcmp(cmd->name, argv[0])){
//			cmd_found = 1;
			if((argc - 1) >= cmd->argc){
				found = COMMAND_FOUND;
				break;
			}else{
				cmd++;
				continue;
			}
		}
//		else{
//			cmd_found = 0;
//		}
		cmd++;
	}

	if(COMMAND_FOUND == found){ return cmd; }

	return NULL;
}
