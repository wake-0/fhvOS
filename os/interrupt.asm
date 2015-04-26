
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
	.global GetContext
	.ref interruptRamVectors
	.ref interruptIrqResetHandlers
	.ref irq_handler1
	;.ref SwiHandler

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
; The SVC Handler switches to system mode if the SVC number is 458752. If the
; SVC number is different, no mode switching will be done.
;
swi_handler:
    STMFD    r13!, {r0-r1, r14}       ; Save context in SVC stack
    SUB      r13, r13, #0x4           ; Adjust the stack pointer
    LDR      r0, [r14, #-4]           ; R0 points to SWI instruction
    BIC      r0, r0, #MASK_SVC_NUM    ; Get the SWI number
    CMP      r0, #458752

    ; swi handler

    MRSEQ    r1, spsr                 ; Copy SPSR
    ORREQ    r1, r1, #0x1F            ; Change the mode to System
    MSREQ    spsr_cf, r1              ; Restore SPSR
    ADD      r13, r13, #0x4           ; Adjust the stack pointer
    LDMFD    r13!, {r0-r1, pc}^       ; Restore registers from IRQ stack



irq_handler:
;	SUB      LR, LR, #4               ; Apply lr correction
;    STMFD    SP, {R0-R13, LR, PC}^    ; Save context in IRQ stack
;
;    SUB 	 SP, SP, #64				  ; SP correction
;    MRS      R12, cpsr                ; Copy cpsr
;    STMFD    SP, {R12}^          	  ; {r1, r12} Save fpscr and spsr
;    SUB		 SP, SP, #4			      ; SP correction
;    B        irq_handler1

	SUB      LR, LR, #4               ; Apply lr correction
    STMFD    SP, {R0-R12}^    ; Save context in IRQ stack
    SUB 	 SP, SP, #52				  ; SP correction
    STMFD	 SP!, {SP}
    STMFD	 SP!, {LR}
    MRS      R12, cpsr                ; Copy cpsr
    STMFD    SP, {R12}^          	  ; {r1, r12} Save fpscr and spsr
    SUB		 SP, SP, #4			      ; SP correction
    B        irq_handler1


GetContext:
	ADD    R0, SP, #40
	;SUB    R0, SP, #64	; GetContext stack pointer overhead
	MOV    PC, LR

	;SUB    R0, R0, #0x68  ; Context stack pointer overhead
	; STR    R12, [R13, #4]
	; LDR    R0, [R13, #4]
.end
