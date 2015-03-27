;
; intvecs.asm - contains definitions and branch instructions for the exception handlers of ARM335x
;
;  Created on: 16.03.2015
;      Author: Marko Petrovic
;

; global definition of interrupt exception handlers

	.global _c_int00
	.global udef_handler
	.global swi_handler
	.global pabt_handler
	.global dabt_handler
	.global irq_handler
	.global fiq_handler


; define .intvecs memory section (AM335x Technical Reference Manual.pdf Page 4100 Table 26-3 RAM Exception Vectors)
	.sect ".intvecs"
		;B boot			; Reset
		B udef_handler	; Interrupt Undefined		0x4030CE04
		B swi_handler	; Interrupt SWI				0x4030CE08
		B pabt_handler	; Interrupt Prefetch Abort	0x4030CE0C
		B dabt_handler	; Interrupt Data Abort		0x4030CE10
		.word 0			; Unused					0x4030CE14
		B irq_handler	; Interrupt IRQ				0x4030CE18
		B fiq_handler	; Interrupt FIQ				0x4030CE1C
