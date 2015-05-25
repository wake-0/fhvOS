/*
 * platform.h
 *
 *  Created on: 15.03.2015
 *      Author: Vaio
 */

#ifndef HAL_PLATFORM_PLATFORM_H_
#define HAL_PLATFORM_PLATFORM_H_

#include <inttypes.h>
#include <string.h>
#include <stdlib.h>

typedef signed int		INT;
typedef unsigned int	UINT;

/* These types are assumed as 8-bit integer */
typedef signed char		CHAR;
typedef unsigned char	UCHAR;
typedef unsigned char	BYTE;

/* These types are assumed as 16-bit integer */
typedef signed short	SHORT;
typedef unsigned short	USHORT;
typedef unsigned short	WORD;

/* These types are assumed as 32-bit integer */
typedef signed int		LONG;
typedef unsigned int	ULONG;
typedef unsigned int	DWORD;

/* Boolean type */
typedef enum {
	iFALSE = 0,
	iTRUE
} BOOL;

typedef unsigned char boolean_t;
typedef uint32_t address_t;

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
