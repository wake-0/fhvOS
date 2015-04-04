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
	.global MMUFlushTLB
	.global MMUSetProcessTable
	.global MMUSetKernelTable
	.global MMUSetDomainAccess
	.global MMULoadDabtData
	.global InstructionCacheEnable
	.global InstructionCacheDisable
	.global InstructionCacheFlush
	.global DataCacheEnable


MMUEnable:
    STMFD R13!, {R0,R1}
	MRC P15, #0, R0, C1, C0, #0
	; add SCTLR_ICACHE | SCTLR_DCACHE | SCTLR_PREDICT | SCTLR_MMUEN = 0x1805
	MOV R1, #0x01
	ORR R0, R0, R1
	MCR P15, #0, R0, C1, C0, #0
   	LDMFD R13!, {R0,R1}
   	MOV PC, R14


MMUDisable:
    STMFD R13!, {R0, R1}
	MRC P15, #0, R0, C1, C0, #0
	; clearup flags ~( SCTLR_TRE | SCTLR_AFE |  SCTLR_ICACHE | SCTLR_DCACHE | SCTLR_MMUEN) = ~(0x30001005)
	; = 0xCFFFEFFA
	MOV R1, #0xCFFF
	MOVT R1, #0xEFFA
	AND R0, R0, R1
	MCR P15, #0, R0, C1, C0, #0
  	LDMFD R13!, {R0, R1}
   	MOV PC, R14


MMUFlushTLB:
	STMFD R13!, {R0, R1}
	MOV R0, #0
	MCR P15, #0, R0, C8, C7, #0
  	LDMFD R13!, {R0, R1}
   	MOV PC, R14


MMUSetProcessTable:
	MCR P15, #0, R0, C2, C0, #0
   	MOV PC, R14


MMUSetKernelTable:
	MCR P15, #0, R0, C2, C0, #1
   	MOV PC, R14


MMUSetDomainAccess:
	MCR P15, #0, R0, C3, C0, #0
	MOV PC, R14


MMULoadDabtData:
	MRC   P15, #0, R0, C6, C0, #0
	;LDR   R1, _mmu_accessed_address
	STR   R0, [R1]
	MRC p15, #0, r0, c5, c0, #0
	;LDR r1, _mmu_fault_state
	STR r0, [r1]
	MOV PC, R14


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
