    .armfunc _c_int00
    .global  _c_int00
    .global __TI_auto_init
    .global systemStack
	.ref main
c_r13_system    .long    systemStack

_c_int00: .asmfunc

    BL    __TI_auto_init

    BL    main
.end
