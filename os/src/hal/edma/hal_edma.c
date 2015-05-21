/*
 * hal_edma.c
 *
 *  Created on: 21.05.2015
 *      Author: Kevin
 */

#include "hal_edma.h"

#include "../interrupt/hal_interrupt.h"

#include "../am335x/hw_types.h"
#include "../am335x/hw_edma3cc.h"
#include "../am335x/soc_AM335x.h"
#include "../am335x/hw_cm_per.h"

// Forward declarations
static unsigned int edmaVersionGet(void);
static void EDMA3EnableChInShadowReg(unsigned int baseAdd, unsigned int chType, unsigned int chNum);
static void EDMA3MapChToEvtQ(unsigned int baseAdd, unsigned int chType, unsigned int chNum, unsigned int evtQNum);

// Callback
static void Edma3CompletionIsr(void* params);
static void Edma3CCErrorIsr(void* params);
static void HSMMCSDIsr(void* params);

static void EDMA3EnableEvtIntr(unsigned int baseAdd, unsigned int chNum);

// Macros
/** DMAQNUM bits Clear */
#define EDMA3CC_DMAQNUM_CLR(chNum)            ( ~ (0x7u << (((chNum) % 8u) * 4u)))
/** DMAQNUM bits Set */
#define EDMA3CC_DMAQNUM_SET(chNum,queNum)     ((0x7u & (queNum)) << (((chNum) % 8u) * 4u))
/** QDMAQNUM bits Clear */
#define EDMA3CC_QDMAQNUM_CLR(chNum)           ( ~ (0x7u << ((chNum) * 4u)))
/** QDMAQNUM bits Set */
#define EDMA3CC_QDMAQNUM_SET(chNum,queNum)    ((0x7u & (queNum)) << ((chNum) * 4u))
/** OPT-TCC bitfield Set */
#define EDMA3CC_OPT_TCC_SET(tcc)              (((EDMA3CC_OPT_TCC >> \
                                              EDMA3CC_OPT_TCC_SHIFT) & \
                                              (tcc)) << EDMA3CC_OPT_TCC_SHIFT)

#define EDMA_COMPLTN_INT_NUM 			      (SYS_INT_EDMACOMPINT)
#define EDMA_ERROR_INT_NUM 					  (SYS_INT_EDMAERRINT)
#define MMCSD_INT_NUM 						  (SYS_INT_MMCSD0INT)
#define EDMA3CC_OPT_TCC_CLR 				  (~EDMA3CC_OPT_TCC)

// internal variables
static unsigned int regionId;

void EDMAModuleClkConfig(void)
{
    /* Configuring clocks for EDMA3 TPCC and TPTCs. */

    /* Writing to MODULEMODE field of CM_PER_TPCC_CLKCTRL register. */
    HWREG(SOC_CM_PER_REGS + CM_PER_TPCC_CLKCTRL) |=
          CM_PER_TPCC_CLKCTRL_MODULEMODE_ENABLE;

    /* Waiting for MODULEMODE field to reflect the written value. */
    while(CM_PER_TPCC_CLKCTRL_MODULEMODE_ENABLE !=
          (HWREG(SOC_CM_PER_REGS + CM_PER_TPCC_CLKCTRL) &
          CM_PER_TPCC_CLKCTRL_MODULEMODE));

    /* Writing to MODULEMODE field of CM_PER_TPTC0_CLKCTRL register. */
    HWREG(SOC_CM_PER_REGS + CM_PER_TPTC0_CLKCTRL) |=
          CM_PER_TPTC0_CLKCTRL_MODULEMODE_ENABLE;

    /* Waiting for MODULEMODE field to reflect the written value. */
    while(CM_PER_TPTC0_CLKCTRL_MODULEMODE_ENABLE !=
          (HWREG(SOC_CM_PER_REGS + CM_PER_TPTC0_CLKCTRL) &
           CM_PER_TPTC0_CLKCTRL_MODULEMODE));

    /* Writing to MODULEMODE field of CM_PER_TPTC1_CLKCTRL register. */
    HWREG(SOC_CM_PER_REGS + CM_PER_TPTC1_CLKCTRL) |=
          CM_PER_TPTC1_CLKCTRL_MODULEMODE_ENABLE;

    /* Waiting for MODULEMODE field to reflect the written value. */
    while(CM_PER_TPTC1_CLKCTRL_MODULEMODE_ENABLE !=
          (HWREG(SOC_CM_PER_REGS + CM_PER_TPTC1_CLKCTRL) &
           CM_PER_TPTC1_CLKCTRL_MODULEMODE));

    /* Writing to MODULEMODE field of CM_PER_TPTC2_CLKCTRL register. */
    HWREG(SOC_CM_PER_REGS + CM_PER_TPTC2_CLKCTRL) |=
          CM_PER_TPTC2_CLKCTRL_MODULEMODE_ENABLE;

    /* Waiting for MODULEMODE field to reflect the written value. */
    while(CM_PER_TPTC2_CLKCTRL_MODULEMODE_ENABLE !=
          (HWREG(SOC_CM_PER_REGS + CM_PER_TPTC2_CLKCTRL) &
           CM_PER_TPTC2_CLKCTRL_MODULEMODE));


	/*	DMA in non-idle mode	*/
	HWREG(0x49800010) = 0x00000028;
	HWREG(0x49900010) = 0x00000028;
	HWREG(0x49a00010) = 0x00000028;


    /*
    ** Waiting for IDLEST field in CM_PER_TPCC_CLKCTRL register to attain the
    ** desired value.
    */
    while((CM_PER_TPCC_CLKCTRL_IDLEST_FUNC <<
           CM_PER_TPCC_CLKCTRL_IDLEST_SHIFT) !=
           (HWREG(SOC_CM_PER_REGS + CM_PER_TPCC_CLKCTRL) &
            CM_PER_TPCC_CLKCTRL_IDLEST));

    /*
    ** Waiting for IDLEST field in CM_PER_TPTC0_CLKCTRL register to attain the
    ** desired value.
    */
    while((CM_PER_TPTC0_CLKCTRL_IDLEST_FUNC <<
           CM_PER_TPTC0_CLKCTRL_IDLEST_SHIFT) !=
           (HWREG(SOC_CM_PER_REGS + CM_PER_TPTC0_CLKCTRL) &
            CM_PER_TPTC0_CLKCTRL_IDLEST));

    /*
    ** Waiting for STBYST field in CM_PER_TPTC0_CLKCTRL register to attain the
    ** desired value.
    */
    while((CM_PER_TPTC0_CLKCTRL_STBYST_FUNC <<
           CM_PER_TPTC0_CLKCTRL_STBYST_SHIFT) !=
           (HWREG(SOC_CM_PER_REGS + CM_PER_TPTC0_CLKCTRL) &
            CM_PER_TPTC0_CLKCTRL_STBYST));

    /*
    ** Waiting for IDLEST field in CM_PER_TPTC1_CLKCTRL register to attain the
    ** desired value.
    */
    while((CM_PER_TPTC1_CLKCTRL_IDLEST_FUNC <<
           CM_PER_TPTC1_CLKCTRL_IDLEST_SHIFT) !=
           (HWREG(SOC_CM_PER_REGS + CM_PER_TPTC1_CLKCTRL) &
            CM_PER_TPTC1_CLKCTRL_IDLEST));

    /*
    ** Waiting for STBYST field in CM_PER_TPTC1_CLKCTRL register to attain the
    ** desired value.
    */
    while((CM_PER_TPTC1_CLKCTRL_STBYST_FUNC <<
           CM_PER_TPTC1_CLKCTRL_STBYST_SHIFT) !=
           (HWREG(SOC_CM_PER_REGS + CM_PER_TPTC1_CLKCTRL) &
            CM_PER_TPTC1_CLKCTRL_STBYST));

    /*
    ** Waiting for IDLEST field in CM_PER_TPTC2_CLKCTRL register to attain the
    ** desired value.
    */
    while((CM_PER_TPTC2_CLKCTRL_IDLEST_FUNC <<
           CM_PER_TPTC2_CLKCTRL_IDLEST_SHIFT) !=
           (HWREG(SOC_CM_PER_REGS + CM_PER_TPTC2_CLKCTRL) &
            CM_PER_TPTC2_CLKCTRL_IDLEST));

    /*
    ** Waiting for STBYST field in CM_PER_TPTC2_CLKCTRL register to attain the
    ** desired value.
    */
    while((CM_PER_TPTC2_CLKCTRL_STBYST_FUNC <<
           CM_PER_TPTC2_CLKCTRL_STBYST_SHIFT) !=
           (HWREG(SOC_CM_PER_REGS + CM_PER_TPTC2_CLKCTRL) &
            CM_PER_TPTC2_CLKCTRL_STBYST));
}

void EDMA3Init(unsigned int baseAdd, unsigned int queNum)
{
    unsigned int count = 0;
    unsigned int i = 0;

#ifdef _TMS320C6X
    /* For DSP, regionId is assigned here and used globally in the driver */
    regionId = (unsigned int)1u;
#else
    /* FOR ARM, regionId is assigned here and used globally in the driver   */
    regionId = (unsigned int)0u;
#endif

    /* Clear the Event miss Registers                                       */
    HWREG(baseAdd + EDMA3CC_EMCR) = EDMA3_SET_ALL_BITS;
    HWREG(baseAdd + EDMA3CC_EMCRH) = EDMA3_SET_ALL_BITS;

    HWREG(baseAdd + EDMA3CC_QEMCR) = EDMA3_SET_ALL_BITS;

    /* Clear CCERR register                                                 */
    HWREG(baseAdd + EDMA3CC_CCERRCLR) = EDMA3_SET_ALL_BITS;

    /* FOR TYPE EDMA*/
    /* Enable the DMA (0 - 64) channels in the DRAE and DRAEH register */

    HWREG(baseAdd + EDMA3CC_DRAE(regionId)) = EDMA3_SET_ALL_BITS;
    HWREG(baseAdd + EDMA3CC_DRAEH(regionId)) = EDMA3_SET_ALL_BITS;


    if((EDMA_REVID_AM335X == edmaVersionGet()))
    {
         for(i = 0; i < 64; i++)
         {
              /* All events are one to one mapped with the channels */
              HWREG(baseAdd + EDMA3CC_DCHMAP(i)) = i << 5;
         }

    }
    /* Initialize the DMA Queue Number Registers                            */
    for (count = 0;count < SOC_EDMA3_NUM_DMACH; count++)
    {
         HWREG(baseAdd + EDMA3CC_DMAQNUM(count >> 3u)) &= EDMA3CC_DMAQNUM_CLR(count);
         HWREG(baseAdd + EDMA3CC_DMAQNUM(count >> 3u)) |= EDMA3CC_DMAQNUM_SET(count,queNum);
    }

    /* FOR TYPE QDMA */
    /* Enable the DMA (0 - 64) channels in the DRAE register                */
    HWREG(baseAdd + EDMA3CC_QRAE(regionId)) = EDMA3_SET_ALL_BITS;

    /* Initialize the QDMA Queue Number Registers                           */
    for (count = 0;count < SOC_EDMA3_NUM_QDMACH; count++)
    {
        HWREG(baseAdd + EDMA3CC_QDMAQNUM) &= EDMA3CC_QDMAQNUM_CLR(count);
        HWREG(baseAdd + EDMA3CC_QDMAQNUM) |=
                       EDMA3CC_QDMAQNUM_SET(count,queNum);
    }
}
void EDMA3AINTCConfigure(void)
{
    /* Registering EDMA3 Channel Controller transfer completion interrupt.  */
	InterruptHandlerRegister(EDMA_COMPLTN_INT_NUM, Edma3CompletionIsr);

    /* Setting the priority for EDMA3CC completion interrupt in AINTC. */
	// IntPrioritySet(EDMA_COMPLTN_INT_NUM, 0, AINTC_HOSTINT_ROUTE_IRQ);

    /* Registering EDMA3 Channel Controller Error Interrupt. */
	InterruptHandlerRegister(EDMA_ERROR_INT_NUM, Edma3CCErrorIsr);

    /* Setting the priority for EDMA3CC Error interrupt in AINTC. */
    // IntPrioritySet(EDMA_ERROR_INT_NUM, 0, AINTC_HOSTINT_ROUTE_IRQ);

    /* Enabling the EDMA3CC completion interrupt in AINTC. */
	InterruptHandlerEnable(EDMA_COMPLTN_INT_NUM);

    /* Enabling the EDMA3CC Error interrupt in AINTC. */
	InterruptHandlerEnable(EDMA_ERROR_INT_NUM);

    /* Registering HSMMC Interrupt handler */
	InterruptHandlerRegister(MMCSD_INT_NUM, HSMMCSDIsr);

    /* Setting the priority for EDMA3CC completion interrupt in AINTC. */
    // IntPrioritySet(MMCSD_INT_NUM, 0, AINTC_HOSTINT_ROUTE_IRQ);

    /* Enabling the HSMMC interrupt in AINTC. */
	InterruptHandlerEnable(MMCSD_INT_NUM);

    /* Enabling IRQ in CPSR of ARM processor. */
	InterruptMasterIRQEnable();
}

unsigned int EDMA3RequestChannel(unsigned int baseAdd, unsigned int chType, unsigned int chNum, unsigned int tccNum, unsigned int evtQNum)
{
    unsigned int retVal = FALSE;
    if (chNum < SOC_EDMA3_NUM_DMACH)
    {
        /* Enable the DMA channel in the enabled in the shadow region
         * specific register
         */
        EDMA3EnableChInShadowReg(baseAdd, chType, chNum);

        EDMA3MapChToEvtQ( baseAdd, chType, chNum, evtQNum);
        if (EDMA3_CHANNEL_TYPE_DMA == chType)
        {
            /* Interrupt channel nums are < 32 */
            if (tccNum < SOC_EDMA3_NUM_DMACH)
            {
            /* Enable the Event Interrupt                             */
                EDMA3EnableEvtIntr(baseAdd, chNum);
                retVal = TRUE;
            }
            HWREG(baseAdd + EDMA3CC_OPT(chNum)) &= EDMA3CC_OPT_TCC_CLR;
            HWREG(baseAdd + EDMA3CC_OPT(chNum)) |= EDMA3CC_OPT_TCC_SET(tccNum);
        }
        else if (EDMA3_CHANNEL_TYPE_QDMA== chType)
        {
            /* Interrupt channel nums are < 8 */
            if (tccNum < SOC_EDMA3_NUM_QDMACH)
            {
                /* Enable the Event Interrupt                             */
                EDMA3EnableEvtIntr(baseAdd, chNum);
                retVal = TRUE;
            }
            HWREG(baseAdd + EDMA3CC_OPT(chNum)) &= EDMA3CC_OPT_TCC_CLR;
            HWREG(baseAdd + EDMA3CC_OPT(chNum)) |= EDMA3CC_OPT_TCC_SET(tccNum);
        }
    }
    return retVal;
}

static void EDMA3EnableChInShadowReg(unsigned int baseAdd, unsigned int chType, unsigned int chNum)
{
    /* Allocate the DMA/QDMA channel */
    if (EDMA3_CHANNEL_TYPE_DMA == chType)
    {
         /* FOR TYPE EDMA*/
         if(chNum < 32)
         {
              /* Enable the DMA channel in the DRAE registers */
              HWREG(baseAdd + EDMA3CC_DRAE(regionId)) |= (0x01u << chNum);
         }
         else
         {
              /* Enable the DMA channel in the DRAEH registers */
              HWREG(baseAdd + EDMA3CC_DRAEH(regionId)) |= (0x01u << (chNum - 32));
         }

    }
    else if (EDMA3_CHANNEL_TYPE_QDMA == chType)
    {
        /* FOR TYPE QDMA */
        /* Enable the QDMA channel in the DRAE/DRAEH registers */
        HWREG(baseAdd + EDMA3CC_QRAE(regionId)) |= 0x01u << chNum;
    }
}

static void EDMA3EnableEvtIntr(unsigned int baseAdd, unsigned int chNum)
{
    if(chNum < 32)
    {
         /*  Interrupt Enable Set Register (IESR)                                */
         HWREG(baseAdd + EDMA3CC_S_IESR(regionId)) |= (0x01u <<  chNum);
    }
    else
    {
         /*  Interrupt Enable Set Register (IESRH)                                */
         HWREG(baseAdd + EDMA3CC_S_IESRH(regionId)) |= (0x01u << (chNum - 32));
    }
}

static void EDMA3MapChToEvtQ(unsigned int baseAdd, unsigned int chType, unsigned int chNum, unsigned int evtQNum)
{
    if (EDMA3_CHANNEL_TYPE_DMA == chType)
    {
        /* Associate DMA Channel to Event Queue                             */
        HWREG(baseAdd + EDMA3CC_DMAQNUM((chNum) >> 3u)) &=
                             EDMA3CC_DMAQNUM_CLR(chNum);

        HWREG(baseAdd + EDMA3CC_DMAQNUM((chNum) >> 3u)) |=
                      EDMA3CC_DMAQNUM_SET((chNum), evtQNum);
    }
    else if (EDMA3_CHANNEL_TYPE_QDMA == chType)
    {
        /* Associate QDMA Channel to Event Queue                            */
        HWREG(baseAdd + EDMA3CC_QDMAQNUM) |=
                       EDMA3CC_QDMAQNUM_SET(chNum, evtQNum);
    }
}

static unsigned int edmaVersionGet(void)
{
    return 2;
}

static void Edma3CompletionIsr(void* params)
{

}

static void Edma3CCErrorIsr(void* params)
{

}

static void HSMMCSDIsr(void* params)
{

}
