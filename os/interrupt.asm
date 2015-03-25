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
	.ref interruptRamVectors
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


irq_handler:
	SUBS pc, lr, #4


fig_handler:
	SUBS pc, lr, #4


udef_handler:
	MOVS pc,lr


pabt_handler:
	SUBS pc,lr, #4


dabt_handler:
	SUBS pc, lr, #8
