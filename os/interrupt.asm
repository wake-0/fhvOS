
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
	.ref interruptRamVectors
	.ref interruptIrqResetHandlers
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
    MRSEQ    r1, spsr                 ; Copy SPSR
    ORREQ    r1, r1, #0x1F            ; Change the mode to System
    MSREQ    spsr_cf, r1              ; Restore SPSR
    ADD      r13, r13, #0x4           ; Adjust the stack pointer
    LDMFD    r13!, {r0-r1, pc}^       ; Restore registers from IRQ stack


irq_handler1:
	SUB      r14, r14, #4             ; Apply lr correction
    STMFD    r13!, {r0-r3, r12, r14}  ; Save context in IRQ stack
    MRS      r12, spsr                ; Copy spsr
    ;VMRS     r1,  fpscr               ; Copy fpscr
    STMFD    r13!, {r12}          ; {r1, r12} Save fpscr and spsr
    ;VSTMDB   r13!, {d0-d7}            ; Save D0-D7 NEON/VFP registers

    LDR      r0, ADDR_THRESHOLD       ; Get the IRQ Threshold
    LDR      r1, [r0, #0]
    STMFD    r13!, {r1}               ; Save the threshold value

    LDR      r2, ADDR_IRQ_PRIORITY   ; Get the active IRQ priority
    LDR      r3, [r2, #0]
    STR      r3, [r0, #0]             ; Set the priority as threshold
    LDR      r1, ADDR_SIR_IRQ         ; Get the Active IRQ
    LDR      r2, [r1]
    AND      r2, r2, #MASK_ACTIVE_IRQ ; Mask the Active IRQ number

    MOV      r0, #NEWIRQAGR           ; To enable new IRQ Generation
    LDR      r1, ADDR_CONTROL

    CMP      r3, #0                   ; Check if non-maskable priority 0
    STRNE    r0, [r1]                 ; if > 0 priority, acknowledge INTC
    DSB                               ; Make sure acknowledgement is completed

    ;
    ; Enable IRQ and switch to system mode. But IRQ shall be enabled
    ; only if priority level is > 0. Note that priority 0 is non maskable.
    ; Interrupt Service Routines will execute in System Mode.
    ;
    MRS      r14, cpsr                ; Read cpsr
    ORR      r14, r14, #MODE_SYS
    BICNE    r14, r14, #I_BIT         ; Enable IRQ if priority > 0
    MSR      cpsr_cxsf, r14

    STMFD    r13!, {r14}              ; Save lr_usr
    LDR      r0, _intIrqHandlers        ; Load the base of the vector table
    ADD      r14, pc, #0              ; Save return address in LR
    LDR      pc, [r0, r2, lsl #2]     ; Jump to the ISR

    LDMFD    r13!, {r14}              ; Restore lr_usr
    ;
    ; Disable IRQ and change back to IRQ mode
    ;
    CPSID    i, #MODE_IRQ

    LDR      r0, ADDR_THRESHOLD      ; Get the IRQ Threshold
    LDR      r1, [r0, #0]
    CMP      r1, #0                   ; If priority 0
    MOVEQ    r2, #NEWIRQAGR           ; Enable new IRQ Generation
    LDREQ    r1, ADDR_CONTROL
    STREQ    r2, [r1]
    LDMFD    r13!, {r1}
    STR      r1, [r0, #0]             ; Restore the threshold value
    ;VLDMIA   r13!, {d0-d7}            ; Restore D0-D7 Neon/VFP registers
    LDMFD    r13!, {r1, r12}          ; Get fpscr and spsr
    MSR      spsr_cxsf, r12           ; Restore spsr
    ;VMSR     fpscr, r1                ; Restore fpscr
    LDMFD    r13!, {r0-r3, r12, pc}^  ; Restore the context and return
.end
