	.armfunc boot
	.global boot

boot: .asmfunc
	MRS R0, CPSR ; Use read/modify/write sequence
	BIC R0, R0, #0x1F ; on CPSR to switch to Supervisor
	ORR R0, R0, #0x13 ; mode 0b10011
	MSR CPSR_c, R0


