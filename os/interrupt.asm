;
; 	interrupt.asm - contains the implementations of the exception handlers of ARM335x
;
;  	Created on: 16.03.2015
;      Author: Marko Petrovic
;


;
; define section of memory
;
	.sect ".interrupts"

;
; global regiter definitions
;
ADDR_THRESHOLD		.word	0x48200000 + 0x68
ADDR_IRQ_PRIORITY	.word	0x48200000 + 0x60
ADDR_SIR_IRQ		.word	0x48200000 + 0x40
ADDR_CONTROL		.word	0x48200000 + 0x48

;
; bit masks
;
MASK_ACTIVE_IRQ		.set	0x0000007F
MASK_NEW_IRQ		.set	0x00000001
MASK_SYS_MODE		.set	0x1F
MASK_IRQ_MODE		.set	0x12
MASK_I_BIT			.set	0x80
MASK_SWI_NUM		.set	0xFF000000


MASK_SVC_NUM      .set   0xFF000000
MODE_SYS          .set   0x1F
MODE_IRQ          .set   0x12
I_BIT             .set   0x80


;
; source file is assembled for ARM instructions
;
	.state32

;
; define global symbols (share between c and asm)
;
	.global irq_handler
	.global swi_handler
	.ref interruptRamVectors			; in interrupt.c, contains registerable irq handlers
	;.ref intIrqResetHandlers

;
; Definition of registerable IRQ handlers.
; Implemented in interrupt.c in HAL.
;
_intIrqHandlers:
	.word interruptRamVectors

;
; definition of irq reset handlers
;
;_intIrqResetHandlers:
;	.word intIrqResetHandlers


;
; The SWI Handler switches to system mode (0x1F).
; User context will be saved before switching and restored after.
;
swi_handler:
	STMFD sp!,{r0-r12,lr} 			; Store registers.
	LDR r0,[lr,#-4] 				; Calculate address of SWI instruction and load it into r0.
	BIC r0,r0,#0xff000000 			; Mask off top 8 bits of instruction to give SWI number.
	 ;
	 ; for future development: call user-defined swi_handler in c here in needed
	 ;
	LDMFD sp!, {r0-r12,pc}^ 		; Restore registers and return.


;
; Handler for IRQ Exceptions.
;
irq_handler:
	STMFD	SP, { R0 - R14 }^			; backup user context in irq stack
	SUB		SP, SP, #60					; LR correction
	STMFD	SP!, { LR }					; store LR in stack
	MRS		r1, spsr					; copy SPSR
	STMFD	SP!, {r1}					; backup SPSR in stack
	MOV 	R0, SP						; pointer to SP in R0, to point to Context-struct, first function parameter

	;
	; read active IRQ number
	;
	LDR		r1, ADDR_SIR_IRQ			; store IRQ status registe in r1
	LDR		r2, [r1, #0]				; load value from ram (address in r1 + offset 0)
	AND		r2, r2, #MASK_ACTIVE_IRQ	; mask active IRQ number

	;
	; start interrupt handler
	;	+ r2	= contains active IRQ number
	;	+ r14	= link register
	;	+ pc	= program counter
	;
	;CPS		#MASK_SYS_MODE				; change to sys mode
	LDR		r3, _intIrqHandlers			; load base of interrupt handler (implemented in interrupt.c)
	ADD		r14, pc, #0					; save return address in link register (return point)
	LDR		pc, [r3, r2, lsl #2]		; jump to interrupt handler

	;
	; read active IRQ number
	;
	LDR		r1, ADDR_SIR_IRQ			; store IRQ status registe in r1
	LDR		r2, [r1, #0]				; load value from ram (address in r1 + offset 0)
	AND		r2, r2, #MASK_ACTIVE_IRQ	; mask active IRQ number

	;
	; reset interrupt flags
	;
	;LDR		r3, _intIrqResetHandlers	; load base of interrupt handler (implemented in interrupt.c)
	ADD		r14, pc, #0					; save return address in link register (return point)
	LDR		pc, [r3, r2, lsl #2]		; jump to interrupt handler

	;CPS		#MASK_IRQ_MODE				; change to irq mode

	;
	; enable IRQ generation
	;
	MOV		r3, #MASK_NEW_IRQ			; load mask for new IRQ generation in r0
	LDR		r4, ADDR_CONTROL			; load address for interrupt control register in r1
	STR		r3, [r4, #0]				; store content of r2 in RAM address in r1 + offset 0

	;
	; TODO change comments
	;
	LDMFD	SP!, { R1 }					; restore SPSR, if changed by scheduler
	MSR		SPSR_cxsf, R1				; set stored cpsr from user to the current CPSR - will be restored later during SUBS

	LDMFD	SP!, { LR }					; restore LR, if changed by scheduler

	LDMFD	SP, { R0 - R14 }^			; restore user-registers, if changed by scheduler
	ADD		SP, SP, #60					; increment stack-pointer: 15 * 4 bytes = 60bytes

 	; TODO: when a process-switch was performed: MOVS	PC, LR should be enough, otherwise we must return to the instruction which was canceled by IRQ thus using SUBS
 	SUBS	PC, LR, #4					; return from IRQ


;
; Handler for Fast Interrupts.
; Returns without doing anything.
;
fig_handler:
	SUBS pc, lr, #4


;
; Handler for Undefined Exceptions.
; Returns without doing anything.
;
udef_handler:
	MOVS pc,lr


;
; Handler for Prefetch Abort Exceptions.
; Returns without doing anything.
;
pabt_handler:
	SUBS pc,lr, #4


;
; Handler for Data Abort Exceptions.
; Returns without doing anything.
;
dabt_handler:
	SUBS pc, lr, #8
