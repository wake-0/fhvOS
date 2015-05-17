/*
 * hal_interrupt.c
 *
 *  Created on: 16.03.2015
 *      Author: Marko Petrovic
 */



#include "hal_interrupt.h"
#include "../am335x/hw_intc.h"
#include "../am335x/hw_types.h"
#include "../am335x/soc_AM335x.h"
#include "../cpu/hal_cpu.h"
#include "../../kernel/kernel.h"


#define REG_IDX_SHIFT                  	(0x05)
#define REG_BIT_MASK                   	(0x1F)
#define NUMBER_OF_INTERRUPTS            (128u)


intHandler_t interruptRamVectors[NUMBER_OF_INTERRUPTS];
intHandler_t interruptIrqResetHandlers[NUMBER_OF_INTERRUPTS];

static boolean_t InterruptDefaultHandler(void* callbackFunction);


static void InterruptResetController(void)
{
	HWREG(SOC_AINTC_REGS + INTC_SYSCONFIG) = INTC_SYSCONFIG_SOFTRESET;

	while((HWREG(SOC_AINTC_REGS + INTC_SYSSTATUS)
	          & INTC_SYSSTATUS_RESETDONE) != INTC_SYSSTATUS_RESETDONE);
}

static void InterruptEnableGeneration(void)
{
	/* Enable any interrupt generation by setting priority threshold */
	HWREG(SOC_AINTC_REGS + INTC_THRESHOLD) = INTC_THRESHOLD_PRIORITYTHRESHOLD;
}

static void InterruptRegisterDefaultHandler(void)
{
	unsigned int intrNum;

	for(intrNum = 0; intrNum < NUMBER_OF_INTERRUPTS; intrNum++)
	{
		interruptRamVectors[intrNum] = InterruptDefaultHandler;
		interruptIrqResetHandlers[intrNum] = InterruptDefaultHandler;
	}
}


// set 1 according to data sheet
static void InterruptEnableInterfaceClockAutogating(void)
{
	HWREG(SOC_AINTC_REGS + INTC_SYSCONFIG) |= SYSCONFIG_SET_AUTO_CLK_GATE;
}

// set 2 according to data sheet
static void InterruptSetFunctionalClock(void)
{
	HWREG(SOC_AINTC_REGS + INTC_IDLE) |= IDLE_SET_FUNC_CLOCK;
}

// API Function
void InterruptResetAINTC(void)
{
    InterruptResetController();
    InterruptEnableInterfaceClockAutogating();
    InterruptSetFunctionalClock();
    InterruptEnableGeneration();
    InterruptRegisterDefaultHandler();
}



void InterruptPrioritySet(unsigned int intrNum, unsigned int priority)
{
    HWREG(SOC_AINTC_REGS + INTC_ILR(intrNum)) =
                                 ((priority << INTC_ILR_PRIORITY_SHIFT)
                                   & INTC_ILR_PRIORITY) | AINTC_HOSTINT_ROUTE_IRQ ;
}


void InterruptHandlerEnable(unsigned int intrNum)
{
    __asm(" dsb");
    
    /* Disable the system interrupt in the corresponding MIR_CLEAR register */
    HWREG(SOC_AINTC_REGS + INTC_MIR_CLEAR(intrNum >> REG_IDX_SHIFT))
                                   = (0x01 << (intrNum & REG_BIT_MASK));
}


void InterruptHandlerDisable(unsigned int intrNum)
{

    __asm(" dsb");
    
    /* Enable the system interrupt in the corresponding MIR_SET register */
    HWREG(SOC_AINTC_REGS + INTC_MIR_SET(intrNum >> REG_IDX_SHIFT)) 
                                       |= (0x01 << (intrNum & REG_BIT_MASK));
    /*HWREG(SOC_AINTC_REGS + INTC_MIR_SET(intrNum >> REG_IDX_SHIFT))
                                   = (0x01 << (intrNum & REG_BIT_MASK));*/
}

void InterruptAllowNewIrqGeneration()
{
	HWREG(SOC_AINTC_REGS + INTC_CONTROL) |= INTC_CONTROL_NEWIRQAGR;
}

// API Function
void InterruptHandlerRegister(unsigned int interruptNumber, intHandler_t fnHandler)
{
	//InterruptHandlerEnable(intrNum);

	interruptRamVectors[interruptNumber] = fnHandler;
}

// API Function
void InterruptUnRegister(unsigned int interruptNumber)
{
	//InterruptHandlerDisable(intrNum);

	interruptRamVectors[interruptNumber] = InterruptDefaultHandler;
}

void InterruptSetGlobalMaskRegister(unsigned int interruptMaskRegister, unsigned int mask)
{
	HWREG(SOC_AINTC_REGS + INTC_MIR_SET(interruptMaskRegister)) |= mask;
}

void InterruptClearGlobalMaskRegister(unsigned int interruptMaskRegister, unsigned int mask)
{
	HWREG(SOC_AINTC_REGS + INTC_MIR_SET(interruptMaskRegister)) &= ~mask;
}

unsigned int InterruptActiveIrqNumberGet(void)
{
    return (HWREG(SOC_AINTC_REGS + INTC_SIR_IRQ) &  INTC_SIR_IRQ_ACTIVEIRQ);
}

intHandler_t InterruptGetHandler(unsigned int interruptNumber)
{
	if(interruptNumber > 127)
		return 0;
	else
		return interruptRamVectors[interruptNumber];
}

void InterruptSaveUserContext(void)
{
	asm("    STMFD		SP, { R0 - R14 }^\n\t"
		"    SUB		SP, SP, #60\n\t"
		"    STMFD		SP!, { LR }\n\t"
		"    MRS		r1, spsr\n\t"
		"    STMFD		SP!, {r1}\n\t"
		"    MOV 		R0, SP\n\t"
			);
}

void InterruptRestoreUserContext(void)
{
	asm("    LDMFD		SP!, { R1 }\n\t"
		"    MSR		SPSR_cxsf, R1\n\t"
		"    LDMFD		SP!, { LR }\n\t"
		"    LDMFD		SP, { R0 - R14 }^\n\t"
		"    ADD		SP, SP, #60\n\t"
		"    SUBS		PC, LR, #4\n\t"
			);
}

void InterruptMasterIRQEnable(void)
{
    CPUirqe();
}


void InterruptMasterIRQDisable(void)
{
    CPUirqd();
}


void InterruptMasterFIQEnable(void)
{
    CPUfiqe();
}


void InterruptMasterFIQDisable(void)
{
    CPUfiqd();
}

static boolean_t InterruptDefaultHandler(void* callbackFunction)
{
    return FALSE;
}


#pragma INTERRUPT(irq_handler1, IRQ)
interrupt void irq_handler1()
{
	volatile address_t context = GetContext();

	unsigned int activeIrq = InterruptActiveIrqNumberGet();

	intHandler_t interruptHandler = InterruptGetHandler(activeIrq);
	boolean_t restoreRegisters = interruptHandler((void*)context);


	InterruptAllowNewIrqGeneration();

	if(FALSE == restoreRegisters)
	{
		// TODO: Interrupt allow new irq generation must be called mandatory!
		// old place: InterruptAllowNewIrqGeneration();
		RestoreRegisters();
	} else {
		// revert stack pointer
		RevertStackPointer();
	}


}

#pragma INTERRUPT(udef_handler, UDEF)
interrupt void udef_handler() {
	KernelDebug("udef interrupt\n");
	;

}

#pragma INTERRUPT(fiq_handler, FIQ)
interrupt void fiq_handler() {
	KernelDebug("fiq interrupt\n");
	;
}

#pragma INTERRUPT(pabt_handler, PABT)
interrupt void pabt_handler() {
	KernelDebug("pabt interrupt\n");
	;
}

/**
 * Is called on any prefetch abort.
 */
#pragma INTERRUPT(dabt_handler1, PABT)
interrupt void dabt_handler1() {
	KernelDebug("dabt interrupt\n");
}
