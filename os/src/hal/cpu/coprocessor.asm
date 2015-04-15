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


;	List of coprocessor instructions:
;
;	CDP 	Coprocessor Data Operations. 					See CDP on page A4-23.
;	LDC 	Load Coprocessor Register. 						See LDC on page A4-34.
;	MCR 	Move to Coprocessor from ARM Register. 			See MCR on page A4-62.
;	MCRR 	Move to Coprocessor from two ARM Registers. 	See MCRR on page A4-64.
;	MRC 	Move to ARM Register from Coprocessor. 			See MRC on page A4-70.
;	MRRC 	Move to two ARM Registers from Coprocessor. 	See MRRC on page A4-72.
;	STC 	Store Coprocessor Register. 					See STC on page A4-186.


;Primary registers of the System Control coprocessor:
;
;Reg 	Generic use 					Specific uses 													Details in
;0 		ID codes (read-only) 			Processor ID, Cache, Tightly-coupled Memory and TLB type 		Register 0: ID codes on page B3-7
;1 		Control bits (read/write) 		System Configuration Bits 										Control register on page B3-12, and
;																										Register 1: Control register on page B4-40
;2 		Memory protection and control 	Page Table Control 												Register 2: Translation table base on page B4-41
;3 		Memory protection and control 	Domain Access Control 											Register 3: Domain access control on page B4-42
;4 		Memory protection and control 	Reserved None. This is a reserved register.
;5 		Memory protection and control 	Fault status 													Fault Address and Fault Status registers on page B4-19,
;																										and Register 5: Fault status on page B4-43
;6 		Memory protection and control 	Fault address 													Fault Address and Fault Status registers
;																										on page B4-19, and Register 6: Fault
;																										Address register on page B4-44
;7 		Cache and write buffer 			Cache/write buffer control R									egister 7: cache management functions on page B6-19
;8 		Memory protection and control 	TLB control 													Register 8: TLB functions on page B4-45
;9 		Cache and write buffer 			Cache lockdown Register 										9: cache lockdown functions on page B6-31
;10 		Memory protection and control 	TLB lockdown 													Register 10: TLB lockdown on page B4-47
;11 		Tightly-coupled Memory Control 	DMA Control 													L1 DMA control using CP15 Register 11 on page B7-9
;12 		Reserved 						Reserved None. 													This is a reserved register.
;13 		Process ID 						Process ID 														Register 13: Process ID on page B4-52,
;																										and Register 13: FCSE PID onpage B8-7




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
