/**
 *  \file   cpu.c
 *
 *  \brief  CPU related definitions
 *
 *  This file contains the API definitions for configuring CPU
*/

/*
* Copyright (C) 2010 Texas Instruments Incorporated - http://www.ti.com/
*/
/*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
*
*    Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*
*    Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the
*    distribution.
*
*    Neither the name of Texas Instruments Incorporated nor the names of
*    its contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/


#include "hal_cpu.h"

/**
 * \brief     This API is called when the CPU is aborted or during execution
 *            of any undefined instruction. Both IRQ and FIQ will be disabled
 *            when the CPU gets an abort and calls this API. 
 *
 * \param     None.
 *
 * \return    None.
 *
 * Note : The user can perform error handling such as an immediate reset 
 *        inside this API if required.
 **/
void CPUAbortHandler(void)
{
    ;  /* Perform Nothing */
}

/*
**
** Wrapper function for the IRQ status
**
*/
__asm("    .sect \".text:CPUIntStatus\"\n"
          "    .clink\n"
          "    .global CPUIntStatus\n"
          "CPUIntStatus:\n"
          "    mrs     r0, CPSR \n"
          "    and     r0, r0, #0xC0\n"
          "    bx      lr");


/*
**
** Wrapper function for the IRQ disable function
**
*/
void CPUirqd(void)
{
    /* Disable IRQ in CPSR */
    asm("    mrs     r0, CPSR\n\t"
        "    orr     r0, r0, #0x80\n\t"
        "    msr     CPSR_c, r0");
}

/*
**
** Wrapper function for the IRQ enable function
**
*/
void CPUirqe(void)
{
    /* Enable IRQ in CPSR */
    asm("    mrs     r0, CPSR\n\t"
        "    bic     r0, r0, #0x80\n\t"
        "    msr     CPSR_c, r0");
}

/*
**
** Wrapper function for the FIQ disable function
**
*/
void CPUfiqd(void)
{
    /* Disable FIQ in CPSR */
    asm("    mrs     r0, CPSR\n\t"
        "    orr     r0, r0, #0x40\n\t"
        "    msr     CPSR_c, r0");
}

/*
**
** Wrapper function for the FIQ enable function
**
*/
void CPUfiqe(void)
{
    /* Enable FIQ in CPSR */
    asm("    mrs     r0, CPSR\n\t"
        "    bic     r0, r0, #0x40\n\t"
        "    msr     CPSR_c, r0");
}


void CPUAtomicStart(void) {
	// TODO: stop interrupts
	//CPUIrqEnable
}

void CPUAtomicEnd(void) {
	// TODO: activate interrupts
}

/**************************** End Of File ************************************/
