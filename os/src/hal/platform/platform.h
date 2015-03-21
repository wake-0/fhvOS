/*
 * platform.h
 *
 *  Created on: 15.03.2015
 *      Author: Vaio
 */

#ifndef HAL_PLATFORM_PLATFORM_H_
#define HAL_PLATFORM_PLATFORM_H_

#include <inttypes.h>

//*****************************************************************************
//
// Define a boolean type, and values for true and false.
//
//*****************************************************************************
typedef unsigned char tBoolean;

#ifndef true
#define true 1
#endif

#ifndef false
#define false 0
#endif

#ifndef NULL
#define NULL ((void*) 0)
#endif

#define TRUE    1
#define FALSE   0



#endif /* HAL_PLATFORM_PLATFORM_H_ */
