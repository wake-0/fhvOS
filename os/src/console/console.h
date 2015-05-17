/*
 * console.h
 *
 *  Created on: 08.05.2015
 *      Author: Nicolaj Hoess
 */

#ifndef CONSOLE_CONSOLE_H_
#define CONSOLE_CONSOLE_H_

#include "../platform/platform.h"
#include "../devicemanager/devicemanager.h"

extern void ConsoleInit();
extern void ConsoleProcess(int argc, char** argv);
extern void ConsoleClear();

#endif /* CONSOLE_CONSOLE_H_ */
