/*
 * system.h
 *
 *  Created on: 22.05.2015
 *      Author: Nicolaj Hoess
 */

#ifndef SYSTEMAPI_INCLUDES_SYSTEM_H_
#define SYSTEMAPI_INCLUDES_SYSTEM_H_

#define	SYSTEM_FAILURE				-1

extern int execute(char* command);
extern void yield(void);

#endif /* SYSTEMAPI_INCLUDES_SYSTEM_H_ */
