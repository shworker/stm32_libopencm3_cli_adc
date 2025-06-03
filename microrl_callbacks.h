/*
 * microrl_callbacks.h
 *
 * Project stm32_eth_test1
 *
 * Copyright (C) 2015 OOO NPK V-Real
 *
 *  Created on: Dec 9, 2015
 *      Author: Dmitry Saychenko <sadmitry@gmail.com>
 */

#ifndef MICRORL_CALLBACKS_H_
#define MICRORL_CALLBACKS_H_

// print to stream callback
void print1 (const char * string);

// get_char from stream
char USART1_get_char (void);

// execute callback
int execute (int argc, const char * const * argv);

// completion callback
char ** complet (int argc, const char * const * argv);

// ctrl+c callback
void sigint (void);

#endif /* MICRORL_CALLBACKS_H_ */
