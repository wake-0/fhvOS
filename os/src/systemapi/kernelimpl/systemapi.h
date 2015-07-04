/*
 * systemcallFunctions.h
 *
 *  Created on: 03.04.2015
 *      Author: Marko
 */

#ifndef SYSTEMAPI_SYSTEMCALLFUNCTIONS_H_
#define SYSTEMAPI_SYSTEMCALLFUNCTIONS_H_

#include <systemcall.h>
#include "../../scheduler/scheduler.h"

extern void SystemCallHandler(systemCallMessage_t* message, unsigned int systemCallNumber, context_t* context);

#endif /* SYSTEMAPI_SYSTEMCALLFUNCTIONS_H_ */
