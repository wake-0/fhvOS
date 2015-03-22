/*
 * boot.cmd
 *
 *  Created on: 16.03.2015
 *      Author: Marko Petrovic
 *				Kevin Wallis
 *				Nicolaj Hoess
 */


-stack  0x0008                             /* SOFTWARE STACK SIZE           */
-heap   0x2000                             /* HEAP AREA SIZE                */


// set program starting point
-e Entry


// surpress compiler warning that default starting point is different
--diag_suppress=10063


// specify CORTEX A-8 memory according to data sheet
MEMORY
{
	bootROM128:				o = 0x40000000	l = 0x1FFFF			// 128kB BOOT ROM
	bootROM48:				o = 0x40020000	l = 0xBFFF			// 48kB BOOT ROM
    internalSRAM:     		o = 0x402F0400  l = 0x0000FBFF  	// 64kB internal SRAM
    L3OCMC0:  				o = 0x40300000  l = 0x00010000  	// 64kB L3 OCMC SRAM
    externalSDRAM:     		o = 0x80000000  l = 0x40000000  	// 1GB external DDR Bank 0
    //exceptions:				o = 0x4030CE04	l = 0x38
}


// specify sections allocation into memory
SECTIONS
{
    .intvecs   > L3OCMC0 {
		L3OCMC0 = .;
		*(.intvecs)
	}

	.interrupts	> L3OCMC0

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
