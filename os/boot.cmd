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
    internalSRAM:     		o = 0x402F0400  l = 0x0000FC00  	// 64kB internal SRAM
    //L3OCMC0:  				o = 0x40300000  l = 0x00010000  	// 64kB L3 OCMC SRAM
    externalSDRAM:     		o = 0x80000000  l = 0x40000000  	// 1GB external DDR Bank 0
    exceptions:				o = 0x4030CE04	l = 0x38
}


// specify sections allocation into memory
SECTIONS
{
    .intvecs   > exceptions {
		exceptions = .;
		*(.intvecs)
	}

	.interrupts	> internalSRAM

    .text          >  internalSRAM
    .stack         >  internalSRAM
    .bss           >  internalSRAM
                    RUN_START(bss_start)
                    RUN_END(bss_end)
    .cio           >  internalSRAM
    .const         >  internalSRAM
    .data          >  internalSRAM
    .switch        >  internalSRAM
    .sysmem        >  internalSRAM
    .far           >  internalSRAM
    .args          >  internalSRAM
    .ppinfo        >  internalSRAM
    .ppdata        >  internalSRAM

    /* TI-ABI or COFF sections */
    .pinit         >  internalSRAM
    .cinit         >  internalSRAM

    /* EABI sections */
    .binit         >  internalSRAM
    .init_array    >  internalSRAM
    .neardata      >  internalSRAM
    .fardata       >  internalSRAM
    .rodata        >  internalSRAM
    .c6xabi.exidx  >  internalSRAM
    .c6xabi.extab  >  internalSRAM
}
