    .armfunc _c_int00
    .global  _c_int00
    .asg    main,   ARGS_MAIN_RTN
    .global ARGS_MAIN_RTN
    .global __TI_auto_init
    .global systemStack

c_r13_system    .long    systemStack

_c_int00: .asmfunc

    BL    __TI_auto_init

    BL    ARGS_MAIN_RTN
.end
