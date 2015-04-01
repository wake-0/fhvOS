/**
 *  \file   interrupt.c
 *
 *  \brief  Interrupt related APIs.
 *
 *   This file contains the APIs for configuring AINTC
 */



#include "hal_interrupt.h"

#include "../am335x/hw_intc.h"
#include "../am335x/hw_types.h"
#include "../am335x/soc_AM335x.h"
#include "../cpu/hal_cpu.h"


#define REG_IDX_SHIFT                  	(0x05)
#define REG_BIT_MASK                   	(0x1F)
#define NUMBER_OF_INTERRUPTS            (128u)
#define TRUE    1
#define FALSE   0


intHandler_t interruptRamVectors[NUMBER_OF_INTERRUPTS];
intHandler_t interruptIrqResetHandlers[NUMBER_OF_INTERRUPTS];

static void InterruptDefaultHandler(void);




static void resetAintc(void)
{
	HWREG(SOC_AINTC_REGS + INTC_SYSCONFIG) = INTC_SYSCONFIG_SOFTRESET;

	while((HWREG(SOC_AINTC_REGS + INTC_SYSSTATUS)
	          & INTC_SYSSTATUS_RESETDONE) != INTC_SYSSTATUS_RESETDONE);
}

static void enableInterruptGeneration(void)
{
	/* Enable any interrupt generation by setting priority threshold */
	HWREG(SOC_AINTC_REGS + INTC_THRESHOLD) = INTC_THRESHOLD_PRIORITYTHRESHOLD;
}

static void registerDefaultHandlers(void)
{
	unsigned int intrNum;

	for(intrNum = 0; intrNum < NUMBER_OF_INTERRUPTS; intrNum++)
	{
		interruptRamVectors[intrNum] = InterruptDefaultHandler;
		interruptIrqResetHandlers[intrNum] = InterruptDefaultHandler;
	}
}


// set 1 according to data sheet
static void enableInterfaceClockAutogating(void)
{
	HWREG(SOC_AINTC_REGS + INTC_SYSCONFIG) |= SYSCONFIG_SET_AUTO_CLK_GATE;
}

// set 2 according to data sheet
static void setFunctionalClock(void)
{
	HWREG(SOC_AINTC_REGS + INTC_IDLE) |= IDLE_SET_FUNC_CLOCK;
}

// API Function
void AintcInit(void)
{
    resetAintc();
    enableInterfaceClockAutogating();
    setFunctionalClock();
    enableInterruptGeneration();
    registerDefaultHandlers();
}


/**
 * \brief   This API assigns a priority to an interrupt and routes it to
 *          either IRQ or to FIQ. Priority 0 is the highest priority level
 *          Among the host interrupts, FIQ has more priority than IRQ.
 * \param   intrNum  - Interrupt number
 * \param   priority - Interrupt priority level
 *     'priority' can take any value from 0 to 127, 0 being the highest and
 *     127 being the lowest priority.              
 **/
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
		return -1;
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

static void InterruptDefaultHandler(void)
{
    ;
}
