-stack  0x0008                             /* SOFTWARE STACK SIZE           */
-heap   0x2000                             /* HEAP AREA SIZE                */
-e Entry
/* Since we used 'Entry' as the entry-point symbol the compiler issues a    */
/* warning (#10063-D: entry-point symbol other than "_c_int00" specified:   */
/* "Entry"). The CCS Version (5.1.0.08000) stops building from command      */
/* line when there is a warning. So this warning is suppressed with the     */
/* below flag. */

--diag_suppress=10063

/* SPECIFY THE SYSTEM MEMORY MAP */

MEMORY
{
    SRAM:     o = 0x402F0400  l = 0x0000FC00  /* 64kB internal SRAM */
    L3OCMC0:  o = 0x40300000  l = 0x00010000  /* 64kB L3 OCMC SRAM */
    M3SHUMEM: o = 0x44D00000  l = 0x00004000  /* 16kB M3 Shared Unified Code Space */
    M3SHDMEM: o = 0x44D80000  l = 0x00002000  /* 8kB M3 Shared Data Memory */
    DDR0:     o = 0x80000000  l = 0x40000000  /* 1GB external DDR Bank 0 */
}

/* SPECIFY THE SECTIONS ALLOCATION INTO MEMORY */

SECTIONS
{
    .init    : {
                 init.obj (.text)
               } load > 0x402F0400
    .text          >  L3OCMC0
    .stack         >  L3OCMC0
    .bss           >  L3OCMC0
                    RUN_START(bss_start)
                    RUN_END(bss_end)
    .cio           >  L3OCMC0
    .const         >  L3OCMC0
    .data          >  L3OCMC0
    .switch        >  L3OCMC0
    .sysmem        >  L3OCMC0
    .far           >  L3OCMC0
    .args          >  L3OCMC0
    .ppinfo        >  L3OCMC0
    .ppdata        >  L3OCMC0

    /* TI-ABI or COFF sections */
    .pinit         >  L3OCMC0
    .cinit         >  L3OCMC0

    /* EABI sections */
    .binit         >  L3OCMC0
    .init_array    >  L3OCMC0
    .neardata      >  L3OCMC0
    .fardata       >  L3OCMC0
    .rodata        >  L3OCMC0
    .c6xabi.exidx  >  L3OCMC0
    .c6xabi.extab  >  L3OCMC0
}
