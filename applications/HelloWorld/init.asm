    .armfunc _c_int00
    .global  _c_int00
    .asg    main,   ARGS_MAIN_RTN
    .global ARGS_MAIN_RTN
    .global __TI_auto_init
    .cdecls C,LIST,"system.h" ;
    .ref 	main
	.ref	exit

_c_int00: .asmfunc

    BL    	__TI_auto_init

	MOV		R10, #0x00100000
	LDR		R0, [R10]

	ADD		R1, R10, #0x10
;	LDR		R1, R10

    BL    	ARGS_MAIN_RTN

	BL		overriden__exit
.end
