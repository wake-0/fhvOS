/*
 * platform.h
 *
 *  Created on: 12.03.2015
 *      Author: Nicolaj Hoess
 */

#ifndef HAL_PLATFORM_H_
#define HAL_PLATFORM_H_

#define memory_addr_t				volatile unsigned int *
#define bit_mask_t					unsigned int

#define bit_mask_action_t			unsigned char
#define BIT_MASK_ACTION_SET_ON		0
#define BIT_MASK_ACTION_SET_OFF		1
#define BIT_MASK_ACTION_SET_TOGGLE	3
#define BIT_MASK_ACTION_AND			4
#define BIT_MASK_ACTION_OR			5
#define BIT_MASK_ACTION_XOR			7

extern inline void 			PlatformWriteBitMask(memory_addr_t address, bit_mask_t bit, bit_mask_action_t action);
extern inline void 			PlatformWriteBitMaskBlocking(memory_addr_t address, bit_mask_t bit, bit_mask_action_t action);
extern inline bit_mask_t 	PlatformReadBitMask(memory_addr_t address, bit_mask_t bitmask, bit_mask_action_t bit);

#endif /* HAL_PLATFORM_H_ */
