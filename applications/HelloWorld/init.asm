    .global  Entry

    .asg    __args_main,   ARGS_MAIN_RTN
    .global ARGS_MAIN_RTN
    .global __TI_auto_init
    .global systemStack
	.ref main

c_r13_system    .long    systemStack

Entry:

    ; Perform all the required initilizations:
    ;  - Process BINIT Table
    ;  - Perform C auto initialization
    ;  - Call global constructors)
    BL    __TI_auto_init

    LDR   r10, _start_boot
    MOV   lr,pc                           ; Dummy return from start_boot
    BX    r10                             ; Branch to start_boot
    SUB   pc, pc, #0x08                   ; looping
.end

_start_boot:
    .word main
