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
extern void ConsoleDisplayInfo();
extern void ConsoleDisplayDebug();
extern void ConsoleDisplayError();

#endif /* CONSOLE_CONSOLE_H_ */
