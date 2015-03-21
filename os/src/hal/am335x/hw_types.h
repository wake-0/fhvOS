#ifndef _HW_TYPES_H_
#define _HW_TYPES_H_

//*****************************************************************************
//
// Macros for hardware access, both direct and via the bit-band region.
//
//*****************************************************************************
#define HWREG(address) 					(*((volatile unsigned int *)(address)))
#define HWREG_SET(address,bit) 			(HWREG(address) |= bit)
#define HWREG_UNSET(address,bit) 		(HWREG(address) &= ~bit)
#define HWREG_CHECK(address, checkbit)	((HWREG(address) & checkbit) == checkbit)
#define HWREG_CLEAR(address)			(HWREG(address) &= ~0xFFFF)
#define HWREG_WRITE(address, value)		(HWREG(address) = value)

#define HWREGH(x)                                                             \
        (*((volatile unsigned short *)(x)))
#define HWREGB(x)                                                             \
        (*((volatile unsigned char *)(x)))
#define HWREGBITW(x, b)                                                       \
        HWREG(((unsigned int)(x) & 0xF0000000) | 0x02000000 |                \
              (((unsigned int)(x) & 0x000FFFFF) << 5) | ((b) << 2))
#define HWREGBITH(x, b)                                                       \
        HWREGH(((unsigned int)(x) & 0xF0000000) | 0x02000000 |               \
               (((unsigned int)(x) & 0x000FFFFF) << 5) | ((b) << 2))
#define HWREGBITB(x, b)                                                       \
        HWREGB(((unsigned int)(x) & 0xF0000000) | 0x02000000 |               \
               (((unsigned int)(x) & 0x000FFFFF) << 5) | ((b) << 2))

#endif // __HW_TYPES_H__
