/*
 * boot.cmd
 *
 *  Created on: 16.03.2015
 *      Author: Marko Petrovic
 *				Kevin Wallis
 *				Nicolaj Hoess
 */


-stack  0x0FFFFF                             /* SOFTWARE STACK SIZE           */
-heap   0x4FFFFF                             /* HEAP AREA SIZE                */


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
    externalSDRAM:   		o = 0x80000000  l = 0x3FFFFFFF		// 1GB external DDR Bank 0
    exceptions:				o = 0x4030CE04	l = 0xC4
    // TODO: when using hivecs, the RAM exception addresses need to be mapped on the hivecs base address 0xFFFF0000
    // hivecs:				o = 0xFFFF0000 l = 0xC4
}


// specify sections allocation into memory
SECTIONS
{
	// TODO: when using hivecs, mapp vector table to base address 0xFFFF0000
    .intvecs   > exceptions {
		exceptions = .;
		*(.intvecs)
	}

	.interrupts	> internalSRAM

    .text          >  externalSDRAM
    .stack         >  externalSDRAM
    .bss           >  externalSDRAM
                    RUN_START(bss_start)
                    RUN_END(bss_end)
    .cio           >  externalSDRAM
    .const         >  externalSDRAM
    .data          >  externalSDRAM
    .switch        >  externalSDRAM
    .sysmem        >  externalSDRAM
    .far           >  externalSDRAM
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
