/*
 * loader.h
 *
 *  Created on: 03.06.2015
 *      Author: Kevin Wallis
 */

#ifndef LOADER_LOADER_H_
#define LOADER_LOADER_H_

#include "../platform/platform.h"
#include "../processmanager/processmanager.h"

extern int LoaderLoad(char* programBuf, int length, boolean_t blocking, context_t* context);



#endif /* LOADER_LOADER_H_ */
