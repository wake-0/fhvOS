/*
 * system.h
 *
 *  Created on: 22.05.2015
 *      Author: Nicolaj Hoess
 */

#ifndef SYSTEMAPI_INCLUDES_SYSTEM_H_
#define SYSTEMAPI_INCLUDES_SYSTEM_H_

#include <stdlib.h>

#define exit(x)			overriden__exit(x)

#define	SYSTEM_FAILURE				-1

extern int execute(char* command);
extern void yield(void);
extern void overriden__exit(int);
extern void sleep(unsigned int);
extern void print(const char* message, int len);
extern long uptime(void);

#endif /* SYSTEMAPI_INCLUDES_SYSTEM_H_ */
