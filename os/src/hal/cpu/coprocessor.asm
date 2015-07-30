;
; coprocessor.asm
;
;	This file contains functions for operating MMU and Cache.
;
;	NOTE: all of these functions require access to the coprocessor
;	register CP15.
;
;	All documentation hints refer to the ARM Architecture Reference Manual
;	ARMv7-A and ARMv7-R edition, rev.: ARM DDI 0406C.b (ID072512)
;
;  Created on: 04.04.2015
;      Author: Marko Petrovic
;

	.global MMUEnable
	.global MMUDisable
	.global MMUFlushTLB
	.global MMUSetProcessTable
	.global MMUSetKernelTable
	.global MMUReadProcessTableAddress
	.global MMUReadKernelTableAddress
	.global MMUSetDomainAccess
	.global MMULoadDabtData
	.global MMUSetTranslationTableControlRegister
	.global MMUReadSystemControlRegister
	.global MMUReadTTBCR
	.global MMUChangeTTBR0andContextId
	.global MMUReadContextIdRegister
	.global InstructionCacheEnable
	.global InstructionCacheDisable
	.global InstructionCacheFlush
	.global DataCacheEnable
	.global dabtAccessedVirtualAddress
	.global dabtFaultStatusRegisterValue
	.global currentAddressInTTBR0
	.global currentAddressInTTBR1
	.global currentStatusInSCTLR
	.global currentStatusInTTBCR
	.global currentContextIdRegisterValue
	.global CleanDataCache

accessedAddress: .field dabtAccessedVirtualAddress, 32
faultState: .field dabtFaultStatusRegisterValue, 32
processTableAddress: .field currentAddressInTTBR0, 32
kernelTableAddress: .field currentAddressInTTBR1, 32
currentStatus: .field currentStatusInSCTLR, 32
currentTTBCRValue: .field currentStatusInTTBCR, 32
contextId: .field currentContextIdRegisterValue, 32

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

;
; SCTLR Settings:
;
; FLAG			Bit		Value				Meaning
; 				31		0					should be zero
; TE			30		0					Exceptions are taken in ARM state, not in Thumb state
; AFE			29		0					disable access flags
; TRE			28		0					C and B bits describe memory regions
; NMFI			27		0					Software can mask FIQ's by setting CPSR
; EE			26		0					reserved
; 				25		0					little endian
; VE			24		0					use FIQ und IRQ vectors from table
; 				23		1					reserved
; U				22		1					use of alignment model described on p. A3-108
; FI			21		0					all performance features enabled
; UWXN			20		0					regions with unprivileged access are not forced to XN, see p. B3-1361
; WXN			19		0					regions with write permission are not froced to XN
; 				18		1					reserved
; HA			17		0					Hardware management of access flag is disabled
; 				16		1					reserved
; 				15		0					reserved
; RR			14		0					normal replacement strategy
; V				13		0					low exception vectors, base address is 0x00000000
; I				12		0					instruction caches disabled
; Z				11		1					program flow prediction enabled
; SW			10		0					SWP and SWBP undefined
; 				9		0					reserved
; 				8		0					reserved
; B				7		0					should be zero
; 				6		1					reserved
; CP15BEN		5		1					CP15 barrier operations enabled
; 				4		1					reserved
; 				3		1					reserved
; C				2		0					data and unified caches disabled
; A				1		0					alignment fault checking disabled
; M				0		0					Pl1&0 stage 1 MMU disabled

; see p. B4-1711, accessing SCTLR register
MMUEnable:
    STMFD	SP!, {R0}
    MOV 	R0, #0
	MRC 	p15, #0, R0, C1, C0, #0
	; add SCTLR_ICACHE | SCTLR_DCACHE | SCTLR_PREDICT | SCTLR_MMUEN = 0x1805
	ORR 	R0, R0, #1
	; comment in for hivecs
	; ORR	R0, R0, #0x2000			; turns on hivecs, bit 13 (V)
    DSB
	MCR		p15, #0, R0, C1, C0, #0
	ISB
   	LDMFD 	SP!, {R0}
   	MOV 	PC, LR


; see p. B4-1711, accessing SCTLR register
MMUDisable:
    STMFD 	SP!, {R0, R1}
	MRC 	P15, #0, R0, C1, C0, #0
	; clearup flags ~( SCTLR_TRE | SCTLR_AFE |  SCTLR_ICACHE | SCTLR_DCACHE | SCTLR_MMUEN) = ~(0x30001005)
	; = 0xCFFFEFFA
	MOV 	R1, #0xCFFF
	MOVT 	R1, #0xEFFA
	AND 	R0, R0, R1
	MCR 	p15, #0, R0, C1, C0, #0
  	LDMFD 	SP!, {R0, R1}
   	MOV 	PC, LR


; see p. B4-1711, accessing SCTLR register
MMUReadSystemControlRegister:
	MOV		R0, #0
	MRC 	p15, #0, R0, C1, C0, #0
	LDR		R1, currentStatus
	STR 	R0, [R1]
	MOV		PC, LR


MMUReadTTBCR:
	MOV		R0, #0
	MRC 	p15, #0, R0, c2, c0, #2
	LDR		R1, currentTTBCRValue
	STR 	R0, [R1]
	MOV		PC, LR

MMUFlushTLB:
	STMFD 	SP!, {R0, R1}
	MOV 	R0, #0
	MCR 	p15, #0, R0, C8, C7, #0
  	LDMFD 	SP!, {R0, R1}
   	MOV 	PC, LR


; see p. B4-1729
MMUSetProcessTable:
	MCR 	p15, #0, R0, C2, C0, #0
   	MOV 	PC, LR


; see p. B4-1729
MMUReadProcessTableAddress:
	MOV 	R0, #0
	MRC 	p15, #0, R0, C2, C0, #0
	MOV 	R1, #0
	LDR		R1, processTableAddress
	STR 	R0, [R1]
   	MOV 	PC, LR


; see p. B4-1731
MMUSetKernelTable:
	MCR 	p15, #0, R0, C2, C0, #1
   	MOV 	PC, LR


; see p. B4-1731
MMUReadKernelTableAddress:
	MOV 	R0, #0
	MRC 	p15, #0, R0, C2, C0, #1
	MOV 	R1, #0
	LDR 	R1, kernelTableAddress
	STR 	R0, [R1]
   	MOV 	PC, LR


; see p. B4-1549, p. B4-1387
MMUChangeTTBR0andContextId:
	; read TTBCR register value into R!
	MOV		R2, #0
	MRC 	p15, #0, R2, c2, c0, #2
	; disable use of TTBR0 during ASID change
	; by doing so it is avoided that the old ASID being associated
	; with translation table walks from the new translation tables
	; or the new ASID being associated with translation table walks
	; from the old translation tables
	; this is done setting TTBCR.PD0 = 1 (bit 4)
	ORR		R2, R2, #0x10
	MCR 	p15, #0, R2, c2, c0, #2
	ISB
	; change asid to new value
	MCR 	p15, #0, R1, c13, c0, #1
	; change process L1 page table base address
	MCR 	p15, #0, R0, C2, C0, #0
	ISB
	; enable usage of TTBR0 again
	MOV		R2, #0
	MRC 	p15, #0, R2, c2, c0, #2
	MOV 	R3, #0xFFEF
	AND		R2, R2, R3
	MCR 	p15, #0, R2, C2, C0, #0
	; return
	MOV		PC, LR

; see p. B4-1549
MMUReadContextIdRegister:
	MOV		R0, #0
	MRC 	p15, #0, R0, C13, C0, #1
	MOV		R1, #0
	LDR		R1, contextId
	STR		R0, [R1]
	MOV		PC, LR

MMUSetDomainAccess:
	MCR 	P15, #0, R0, C3, C0, #0
	MOV 	PC, LR


MMULoadDabtData:
	MRC		p15, #0, R0, C6, C0, #0
	LDR 	R1, accessedAddress
	STR 	R0, [R1]
	MRC 	p15, #0, R0, c5, c0, #0
	LDR 	R1, faultState
	STR 	R0, [R1]
	MOV 	PC, LR


; see p. B4-1725
MMUSetTranslationTableControlRegister:
	MOV		R1, #0
	MRC		p15, #0, R1, c2, c0, #2
	ORR 	R1, R1, R0
	MCR		p15, #0, R1, c2, c0, #2
	MOV 	PC, LR


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

CleanDataCache:
	MOV		r0, #0
	MRC 	p15, #0, r0, c7, c10, #3
	MOV		r0, #0
	MCR		p15, #0, r0, c7, c10, #4
	BX		lr

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
