;
; coprocessor.asm
;
;	This file contains functions for operating MMU and Cache.
;
;	NOTE: all of these functions require access to the coprocessor
;	register CP15.
;
;  Created on: 04.04.2015
;      Author: Marko Petrovic
;

	.global MMUEnable
	.global MMUDisable
	.global InstructionCacheEnable
	.global InstructionCacheDisable
	.global InstructionCacheFlush
	.global DataCacheEnable


MMUEnable:
    MRC     p15, #0, r0, c1, c0, #0
    ORR     r0, r0, #0x001
    MCR     p15, #0, r0, c1, c0, #0    ; Set MMU Enable bit
    DSB
    BX      lr


MMUDisable:
    MCR     p15, #0, r0, c8, c7, #0    ; Invalidate TLB
    MRC     p15, #0, r0, c1, c0, #0
    BIC     r0, r0, #1
    MCR     p15, #0, r0, c1, c0, #0    ; Clear MMU bit
    DSB
    BX      lr


InstructionCacheEnable:
    MRC     p15, #0, r0, c1, c0, #0
    ORR     r0,  r0, #0x00001000
    MCR     p15, #0, r0, c1, c0, #0
    BX      lr


InstructionCacheDisable:
    PUSH    {lr}
    MRC     p15, #0, r0, c1, c0, #0
    BIC     r0,  r0, #0x00001000
    MCR     p15, #0, r0, c1, c0, #0
    BL      InstructionCacheFlush
    POP     {lr}
    BX      lr


InstructionCacheFlush:
    MOV     r0, #0
    MCR     p15, #0, r0, c7, c5, #0
    DSB
    BX      lr

DataCacheEnable:
    MRC     p15, #0, r0, c1, c0, #0
    ORR     r0,  r0, #0x00000004
    MCR     p15, #0, r0, c1, c0, #0
    BX      lr
