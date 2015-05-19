;
; Authors: 	Nicolaj Höss
;			Marko Petrovic
;			Kevin Wallis
;
; Description:
; Assembler interrupt handler

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
MASK_SWI_MODE		.set	0x13
MASK_I_BIT			.set	0x80
MASK_SWI_NUM		.set	0xFF000000
NEWIRQAGR			.set 	0x00000001


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
	.global dabt_handler
	.ref SystemCallHandler
	.ref MMUHandleDataAbortException
	.ref interruptRamVectors
	.ref interruptIrqResetHandlers
	.ref InterruptTimerISR

;
; definition of irq handlers
;
_intIrqHandlers:
	.word interruptRamVectors

;
; definition of irq reset handlers
;
_intIrqResetHandlers:
	.word interruptIrqResetHandlers

;
; The SVC Handler switches to system mode if the SVC number is 458752. It
; is also used to handle system calls.
;
swi_handler:
	STMFD	 SP!, {LR}

    STMFD    SP, {R0-R12}^
    NOP
    SUB 	 SP, SP, #52

	STMFD 	 SP, {R13}^
	NOP
	SUB		 SP, SP, #4

	STMFD 	 SP, {LR}^
	NOP
	SUB		 SP, SP, #4

    MRS      R12, SPSR
    STMFD    SP, {R12}^
    NOP
    SUB		 SP, SP, #4

    ; Prepare switch to system mode
    MRSEQ    R3, CPSR                 		; Copy SPSR
    ORREQ    R3, R3, #MASK_SYS_MODE    		; Change the mode to System

	MOV		 R5, SP
	STR		 R5, [SP, #72]
	MOV		 R2, SP

	MSREQ    CPSR_c, R3					; Switch to system mode

    BL		SystemCallHandler

    MRS	  R3, CPSR                 		; Copy SPSR
    BIC      R3, R3, #0x0F
    ORR      R3, R3, #MASK_SWI_MODE    	; Change the mode to System
	MSR      CPSR_c, R3					; Switch to SWI mode

	LDMFD	 SP!, {R1}
	MSR		 SPSR_cxsf, R1
	NOP

	LDMFD	 SP, {LR}^
	NOP
	ADD		 SP, SP, #4

	LDMFD	 SP, {R13}^
	NOP
	ADD		 SP, SP, #4

	LDMFD	 SP, {R0-R12}^
	NOP
	ADD		 SP, SP, #52
    LDMFD	 SP!, {PC}^


;
; The IRQ handler is used to handle exceptions from peripherial modules and for performing a context switch.
;
irq_handler:
	SUB      LR, LR, #4               ; Apply lr correction (DO NOT CHANGE)
	STMFD	 SP!, {LR}				  ; LR becomes PC in context struct

    STMFD    SP, {R0-R12}^    		  ; Save R0-R12
    NOP								  ; See http://stackoverflow.com/questions/324704/arm-access-user-r13-and-r14-from-supervisor-mode
    SUB 	 SP, SP, #52			  ; SP correction

	STMFD 	 SP, {R13}^ 			  ; Store user SP
	NOP
	SUB		 SP, SP, #4

	STMFD 	 SP, {LR}^ 			  	  ; Store user LR
	NOP
	SUB		 SP, SP, #4

    MRS      R12, SPSR                ; Copy cpsr
    STMFD    SP, {R12}^          	  ; Save cpsr
    NOP
    SUB		 SP, SP, #4			      ; SP correction

	MOV		 R5, SP
	STR		 R5, [SP, #72]
	MOV		 R0, SP

    BL       InterruptTimerISR			  ; Branch to code

	; Restore Context
	LDMFD	 SP!, {R1}
	MSR		 SPSR_cxsf, R1
	NOP

	LDMFD	 SP, {LR}^
	NOP
	ADD		 SP, SP, #4

	LDMFD	 SP, {R13}^
	NOP
	ADD		 SP, SP, #4

	LDMFD	 SP, {R0-R12}^
	NOP
	ADD		 SP, SP, #52
    LDMFD	 SP!, {PC}^

;
; This handler is used by the MMU hardware to handle page faults.
;
dabt_handler:
	SUB      LR, LR, #8               ; Apply lr correction (DO NOT CHANGE)
	STMFD	 SP!, {LR}				  ; LR becomes PC in context struct

    STMFD    SP, {R0-R12}^    		  ; Save R0-R12
    NOP								  ; See http://stackoverflow.com/questions/324704/arm-access-user-r13-and-r14-from-supervisor-mode
    SUB 	 SP, SP, #52			  ; SP correction

	STMFD 	 SP, {R13}^ 			  ; Store user SP
	NOP
	SUB		 SP, SP, #4

	STMFD 	 SP, {LR}^ 			  	  ; Store user LR
	NOP
	SUB		 SP, SP, #4

    MRS      R12, SPSR                ; Copy cpsr
    STMFD    SP, {R12}^          	  ; Save cpsr
    NOP
    SUB		 SP, SP, #4			      ; SP correction

	MOV		 R5, SP
	STR		 R5, [SP, #72]
	MOV		 R0, SP

	BL		 MMUHandleDataAbortException

	LDMFD	 SP!, {R1}
	MSR		 SPSR_cxsf, R1
	NOP

	LDMFD	 SP, {LR}^
	NOP
	ADD		 SP, SP, #4

	LDMFD	 SP, {R13}^
	NOP
	ADD		 SP, SP, #4

	LDMFD	 SP, {R0-R12}^
	NOP
	ADD		 SP, SP, #52
    LDMFD	 SP!, {PC}^

.end
