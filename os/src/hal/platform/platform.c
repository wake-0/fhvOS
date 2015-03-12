/*
 * platform.c
 *
 *  Created on: 12.03.2015
 *      Author: Nicolaj Hoess
 */

#include "platform.h"

extern inline void PlatformWriteBitMask(memory_addr_t address, bit_mask_t bitmask, bit_mask_action_t action)
{
	switch (action) {
		case BIT_MASK_ACTION_SET_ON:
			*address |= bitmask;
			break;
		case BIT_MASK_ACTION_SET_OFF:
			*address &= ~(bitmask);
			break;
		case BIT_MASK_ACTION_SET_TOGGLE:
			*address ^= bitmask;
			break;
		default:
			break;
	}

}
extern inline void PlatformWriteBitMaskBlocking(memory_addr_t address, bit_mask_t bitmask, bit_mask_action_t action)
{

}

extern inline bit_mask_t PlatformReadBitMask(memory_addr_t address, bit_mask_t bitmask, bit_mask_action_t action) {
	switch (action) {
		case BIT_MASK_ACTION_AND:
			return *address & bitmask;
		case BIT_MASK_ACTION_OR:
			return *address | bitmask;
		case BIT_MASK_ACTION_XOR:
			return *address ^ bitmask;
		default:
			break;
	}
}
