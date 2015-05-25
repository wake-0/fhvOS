/*
 * driver_sdcard.c
 *
 *  Created on: 20.05.2015
 *      Author: Kevin
 */

#include "driver_sdcard.h"
#include "mmcsd_proto.h"
#include "hs_mmcsdlib.h"

#include "../../hal/edma/hs_mmcsd.h"
#include "../../filesystem/ff.h"
#include "../../hal/edma/hal_edma.h"
#include "../../hal/am335x/soc_AM335x.h"
#include "../../hal/am335x/hw_edma3cc.h"
#include "../../hal/interrupt/hal_interrupt.h"

#include <stdio.h>

// Struct forward declarations
struct _mmcsdCtrlInfo;

// Defines

/** @brief Base address of Channel controller  memory mapped registers        */
#define SOC_EDMA30CC_0_REGS                  (0x49000000)
#define SOC_EDMA3_NUM_EVQUE 				 (4)
#define EVT_QUEUE_NUM                  		 (0)
#define MMCSD_INT_NUM 						 (SYS_INT_MMCSD0INT)

#define EDMA_COMPLTN_INT_NUM 			     (SYS_INT_EDMACOMPINT)
#define EDMA_ERROR_INT_NUM 					 (SYS_INT_EDMAERRINT)
#define EDMA_INST_BASE 						 (SOC_EDMA30CC_0_REGS)
#define EDMA3CC_OPT_TCC_CLR 				 (~EDMA3CC_OPT_TCC)
#define EDMA3_TRIG_MODE_EVENT 				 (2u)
#define EDMA3_TRIG_MODE_MANUAL 				 (0u)
#define EDMA3_TRIG_MODE_QDMA 				 (1u)

#define HSMMCSD_CARD_DETECT_PINNUM 			 (6)
#define HSMMCSD_IN_FREQ 					 (96000000) /* 96MHz */
#define HSMMCSD_INIT_FREQ 					 (400000) /* 400kHz */
#define HS_MMCSD_SUPPORT_VOLT_1P8       	 (MMCHS_CAPA_VS18)
#define HS_MMCSD_SUPPORT_VOLT_3P0       	 (MMCHS_CAPA_VS30)
#define MMCSD_INST_BASE 					 (SOC_MMCHS_0_REGS)
#define MMCSD_RX_EDMA_CHAN 					 (EDMA3_CHA_MMCSD0_RX)

#define PATH_BUF_SIZE   					 (512)

// Callback functions
static void Edma3CompletionIsr(void* params);
static void Edma3CCErrorIsr(void* params);
static void HSMMCSDIsr(void* params);
/* EDMA callback function array */
static void (*cb_Fxn[EDMA3_NUM_TCC])(unsigned int tcc, unsigned int status);
static void callback(unsigned int tccNum, unsigned int status);

// Forward declarations
// EDMA (Enhanced direct memory access)
static void EDMAInit(void);
static void EDMA3AINTCConfigure(void);

// HSMMCSD (High speed multi media ...)
static void HSMMCSDControllerSetup(void);
static void HSMMCSDXferSetup(mmcsdCtrlInfo *ctrl, unsigned char rwFlag,
		void *ptr, unsigned int blkSize, unsigned int nBlks);
static unsigned int HSMMCSDCmdStatusGet(mmcsdCtrlInfo *ctrl);
static unsigned int HSMMCSDXferStatusGet(mmcsdCtrlInfo *ctrl);
static void HSMMCSDRxDmaConfig(void *ptr, unsigned int blkSize,
		unsigned int nblks);
static void HSMMCSDFsMount(unsigned int driveNum, void *ptr);

#pragma DATA_ALIGN(g_sFatFs, SOC_CACHELINE_SIZE);
static FATFS g_sFatFs;

volatile unsigned int callbackOccured = 0;
volatile unsigned int xferCompFlag = 0;
volatile unsigned int dataTimeout = 0;
volatile unsigned int cmdCompFlag = 0;
volatile unsigned int cmdTimeout = 0;
volatile unsigned int errFlag = 0;

static DIR g_sDirObject;
static FILINFO g_sFileInfo;
// This buffer holds the full path to the current working directory.  Initially it is root ("/").
static char g_cCwdBuf[PATH_BUF_SIZE] = "/";
/* SD Controller info structure */
static mmcsdCtrlInfo ctrlInfo;
/* SD card info structure */
static mmcsdCardInfo sdCard;

/* Fat devices registered */
typedef struct _fatDevice {
	/* Pointer to underlying device/controller */
	void *dev;

	/* File system pointer */
	FATFS *fs;

	/* state */
	unsigned int initDone;

} fatDevice;
extern fatDevice fat_devices[2];

int SDCardInit(uint16_t id) {

	EDMAModuleClkConfig();
	EDMAInit();
	EDMAPinMuxSetup();
	HSMMCSDModuleClkConfig();
	HSMMCSDControllerSetup();
	MMCSDCtrlInit(&ctrlInfo);
	MMCSDIntEnable(&ctrlInfo);

	return DRIVER_OK;
}

int SDCardOpen(uint16_t id) {
	if ((HSMMCSDCardPresent(&ctrlInfo)) == 1) {
		HSMMCSDFsMount(0, &sdCard);
		return (f_opendir(&g_sDirObject, g_cCwdBuf) == FR_OK);
	}

	return DRIVER_ERROR;
}

int SDCardClose(uint16_t id) {
	return DRIVER_OK;
}

int SDCardWrite(uint16_t id, char* buf, uint16_t len) {
	return DRIVER_OK;
}

int SDCardRead(uint16_t id, char* buf, uint16_t len) {
	volatile unsigned int i = 0;
	volatile FRESULT fresult;

	volatile int ulTotalSize = 0;
	volatile int ulFileCount = 0;
	volatile int ulDirCount = 0;

	if ((HSMMCSDCardPresent(&ctrlInfo)) == 1) {
		while (1) {
			// Read an entry from the directory.
			fresult = f_readdir(&g_sDirObject, &g_sFileInfo);

			// Check for error and return if there is a problem.
			if (fresult != FR_OK) { return (fresult); }

			// If the file name is blank, then this is the end of the listing.
			if (!g_sFileInfo.fname[0]) { break; }

			// If the attribute is directory, then increment the directory count.
			if (g_sFileInfo.fattrib & AM_DIR) { ulDirCount++; }

			 // Otherwise, it is a file.  Increment the file count, and add in the file size to the total.
			else {
				ulFileCount++;
				ulTotalSize += g_sFileInfo.fsize;
			}

			 // Print the entry information on a single line with formatting to show
			 // the attributes, date, time, size, and name.
			printf("%c%c%c%c%c %u/%02u/%02u %02u:%02u %9u  %s\n",
					(g_sFileInfo.fattrib & AM_DIR) ? 'D' : '-',
					(g_sFileInfo.fattrib & AM_RDO) ? 'R' : '-',
					(g_sFileInfo.fattrib & AM_HID) ? 'H' : '-',
					(g_sFileInfo.fattrib & AM_SYS) ? 'S' : '-',
					(g_sFileInfo.fattrib & AM_ARC) ? 'A' : '-',
					(g_sFileInfo.fdate >> 9) + 1980,
					(g_sFileInfo.fdate >> 5) & 15, g_sFileInfo.fdate & 31,
					(g_sFileInfo.ftime >> 11),
					(g_sFileInfo.ftime >> 5) & 63, g_sFileInfo.fsize,
					g_sFileInfo.fname);
		}

		printf("\n%4u File(s),%10u bytes total\n%4u Dir(s)", ulFileCount, ulTotalSize, ulDirCount);
	}
	else
	{
		return DRIVER_ERROR;
	}

	return DRIVER_OK;
}

int SDCardIoctl(uint16_t id, uint16_t cmd, uint8_t mode, char* buf,
		uint16_t len) {
	return DRIVER_OK;
}

static void EDMAInit(void) {
	// Initialization of EDMA3
	EDMA3Init(EDMA_INST_BASE, EVT_QUEUE_NUM);
	// Configuring the AINTC to receive EDMA3 interrupts.
	EDMA3AINTCConfigure();

	// Request DMA Channel and TCC for MMCSD Receive
	EDMA3RequestChannel(EDMA_INST_BASE, EDMA3_CHANNEL_TYPE_DMA,
	MMCSD_RX_EDMA_CHAN, MMCSD_RX_EDMA_CHAN, EVT_QUEUE_NUM);

	// Registering Callback Function for RX
	cb_Fxn[MMCSD_RX_EDMA_CHAN] = &callback;
}

/*
 ** This function is used as a callback from EDMA3 Completion Handler.
 */
static void callback(unsigned int tccNum, unsigned int status) {
	//callbackOccured = 1;
	//EDMA3DisableTransfer(EDMA_INST_BASE, tccNum, EDMA3_TRIG_MODE_EVENT);
}

static void HSMMCSDControllerSetup(void) {
	ctrlInfo.memBase = MMCSD_INST_BASE;
	ctrlInfo.ctrlInit = HSMMCSDControllerInit;
	ctrlInfo.xferSetup = HSMMCSDXferSetup;
	ctrlInfo.cmdStatusGet = HSMMCSDCmdStatusGet;
	ctrlInfo.xferStatusGet = HSMMCSDXferStatusGet;
	/* Use the funciton HSMMCSDCDPinStatusGet() to use the card presence
	 using the controller.
	 */
	ctrlInfo.cardPresent = HSMMCSDCardPresent;
	ctrlInfo.cmdSend = HSMMCSDCmdSend;
	ctrlInfo.busWidthConfig = HSMMCSDBusWidthConfig;
	ctrlInfo.busFreqConfig = HSMMCSDBusFreqConfig;
	ctrlInfo.intrMask = (HS_MMCSD_INTR_CMDCOMP | HS_MMCSD_INTR_CMDTIMEOUT |
	HS_MMCSD_INTR_DATATIMEOUT | HS_MMCSD_INTR_TRNFCOMP);
	ctrlInfo.intrEnable = HSMMCSDIntEnable;
	ctrlInfo.busWidth = (SD_BUS_WIDTH_1BIT | SD_BUS_WIDTH_4BIT);
	ctrlInfo.highspeed = 1;
	ctrlInfo.ocr = (SD_OCR_VDD_3P0_3P1 | SD_OCR_VDD_3P1_3P2);
	ctrlInfo.card = &sdCard;
	ctrlInfo.ipClk = HSMMCSD_IN_FREQ;
	ctrlInfo.opClk = HSMMCSD_INIT_FREQ;
	ctrlInfo.cdPinNum = HSMMCSD_CARD_DETECT_PINNUM;
	sdCard.ctrl = &ctrlInfo;

	callbackOccured = 0;
	xferCompFlag = 0;
	dataTimeout = 0;
	cmdCompFlag = 0;
	cmdTimeout = 0;
}

static void HSMMCSDXferSetup(mmcsdCtrlInfo *ctrl, unsigned char rwFlag,
		void *ptr, unsigned int blkSize, unsigned int nBlks) {
	if (rwFlag == 1) {
		HSMMCSDRxDmaConfig(ptr, blkSize, nBlks);
	} else {
		//HSMMCSDTxDmaConfig(ptr, blkSize, nBlks);
	}

	ctrl->dmaEnable = 1;
	HSMMCSDBlkLenSet(ctrl->memBase, blkSize);
}

static unsigned int HSMMCSDCmdStatusGet(mmcsdCtrlInfo *ctrl) {
	unsigned int status = 0;

	while ((cmdCompFlag == 0) && (cmdTimeout == 0))
		;

	if (cmdCompFlag) {
		status = 1;
		cmdCompFlag = 0;
	}

	if (cmdTimeout) {
		status = 0;
		cmdTimeout = 0;
	}

	return status;
}

static unsigned int HSMMCSDXferStatusGet(mmcsdCtrlInfo *ctrl) {
	unsigned int status = 0;
	volatile unsigned int timeOut = 0xFFFF;

	while ((xferCompFlag == 0) && (dataTimeout == 0))
		;

	if (xferCompFlag) {
		status = 1;
		xferCompFlag = 0;
	}

	if (dataTimeout) {
		status = 0;
		dataTimeout = 0;
	}

	/* Also, poll for the callback */
	if (HWREG(ctrl->memBase + MMCHS_CMD) & MMCHS_CMD_DP) {
		while (callbackOccured == 0 && ((timeOut--) != 0))
			;
		callbackOccured = 0;

		if (timeOut == 0) {
			status = 0;
		}
	}

	ctrlInfo.dmaEnable = 0;

	return status;
}

static void HSMMCSDRxDmaConfig(void *ptr, unsigned int blkSize,
		unsigned int nblks) {
	EDMA3CCPaRAMEntry paramSet;

	paramSet.srcAddr = ctrlInfo.memBase + MMCHS_DATA;
	paramSet.destAddr = (unsigned int) ptr;
	paramSet.srcBIdx = 0;
	paramSet.srcCIdx = 0;
	paramSet.destBIdx = 4;
	paramSet.destCIdx = (unsigned short) blkSize;
	paramSet.aCnt = 0x4;
	paramSet.bCnt = (unsigned short) blkSize / 4;
	paramSet.cCnt = (unsigned short) nblks;
	paramSet.bCntReload = 0x0;
	paramSet.linkAddr = 0xffff;
	paramSet.opt = 0;

	/* Set OPT */
	paramSet.opt |= ((MMCSD_RX_EDMA_CHAN << EDMA3CC_OPT_TCC_SHIFT)
			& EDMA3CC_OPT_TCC);

	/* 1. Transmission complition interrupt enable */
	paramSet.opt |= (1 << EDMA3CC_OPT_TCINTEN_SHIFT);

	/* 2. Read FIFO : SRC Constant addr mode */
	paramSet.opt |= (1 << 0);

	/* 3. SRC FIFO width is 32 bit */
	paramSet.opt |= (2 << 8);

	/* 4.  AB-Sync mode */
	paramSet.opt |= (1 << 2);

	/* configure PaRAM Set */
	EDMA3SetPaRAM(EDMA_INST_BASE, MMCSD_RX_EDMA_CHAN, &paramSet);

	/* Enable the transfer */
	EDMA3EnableTransfer(EDMA_INST_BASE, MMCSD_RX_EDMA_CHAN,
	EDMA3_TRIG_MODE_EVENT);
}

static void HSMMCSDFsMount(unsigned int driveNum, void *ptr) {
	// TODO: this is important
	f_mount(driveNum, &g_sFatFs);
	fat_devices[driveNum].dev = ptr;
	fat_devices[driveNum].fs = &g_sFatFs;
	fat_devices[driveNum].initDone = 0;
}

static void EDMA3AINTCConfigure(void) {
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

static void Edma3CompletionIsr(void* params) {
	volatile unsigned int pendingIrqs;
	volatile unsigned int isIPR = 0;

	unsigned int indexl;
	unsigned int Cnt = 0;

	indexl = 1;

	isIPR = EDMA3GetIntrStatus(EDMA_INST_BASE);

	if (isIPR) {
		while ((Cnt < EDMA3CC_COMPL_HANDLER_RETRY_COUNT) && (indexl != 0u)) {
			indexl = 0u;
			pendingIrqs = EDMA3GetIntrStatus(EDMA_INST_BASE);

			while (pendingIrqs) {
				if ((pendingIrqs & 1u) == TRUE) {
					/**
					 * If the user has not given any callback function
					 * while requesting the TCC, its TCC specific bit
					 * in the IPR register will NOT be cleared.
					 */
					/* here write to ICR to clear the corresponding IPR bits */

					EDMA3ClrIntr(EDMA_INST_BASE, indexl);

					if (cb_Fxn[indexl] != NULL) {
						(*cb_Fxn[indexl])(indexl, EDMA3_XFER_COMPLETE);
					}
				}
				++indexl;
				pendingIrqs >>= 1u;
			}
			Cnt++;
		}
	}
}

static void Edma3CCErrorIsr(void* params) {
	volatile unsigned int pendingIrqs;
	volatile unsigned int evtqueNum = 0; /* Event Queue Num */
	volatile unsigned int isIPRH = 0;
	volatile unsigned int isIPR = 0;
	volatile unsigned int Cnt = 0u;
	volatile unsigned int index;

	pendingIrqs = 0u;
	index = 1u;

	isIPR = EDMA3GetIntrStatus(EDMA_INST_BASE);
	isIPRH = EDMA3IntrStatusHighGet(EDMA_INST_BASE);

	if ((isIPR | isIPRH) || (EDMA3QdmaGetErrIntrStatus(EDMA_INST_BASE) != 0)
			|| (EDMA3GetCCErrStatus(EDMA_INST_BASE) != 0)) {
		/* Loop for EDMA3CC_ERR_HANDLER_RETRY_COUNT number of time,
		 * breaks when no pending interrupt is found
		 */
		while ((Cnt < EDMA3CC_ERR_HANDLER_RETRY_COUNT) && (index != 0u)) {
			index = 0u;

			if (isIPR) {
				pendingIrqs = EDMA3GetErrIntrStatus(EDMA_INST_BASE);
			} else {
				pendingIrqs = EDMA3ErrIntrHighStatusGet(EDMA_INST_BASE);
			}

			while (pendingIrqs) {
				/*Process all the pending interrupts*/
				if (TRUE == (pendingIrqs & 1u)) {
					/* Write to EMCR to clear the corresponding EMR bits.
					 */
					/*Clear any SER*/

					if (isIPR) {
						EDMA3ClrMissEvt(EDMA_INST_BASE, index);
					} else {
						EDMA3ClrMissEvt(EDMA_INST_BASE, index + 32);
					}
				}
				++index;
				pendingIrqs >>= 1u;
			}
			index = 0u;
			pendingIrqs = EDMA3QdmaGetErrIntrStatus(EDMA_INST_BASE);
			while (pendingIrqs) {
				/*Process all the pending interrupts*/
				if (TRUE == (pendingIrqs & 1u)) {
					/* Here write to QEMCR to clear the corresponding QEMR bits*/
					/*Clear any QSER*/
					EDMA3QdmaClrMissEvt(EDMA_INST_BASE, index);
				}
				++index;
				pendingIrqs >>= 1u;
			}
			index = 0u;

			pendingIrqs = EDMA3GetCCErrStatus(EDMA_INST_BASE);
			if (pendingIrqs != 0u) {
				/* Process all the pending CC error interrupts. */
				/* Queue threshold error for different event queues.*/
				for (evtqueNum = 0u; evtqueNum < SOC_EDMA3_NUM_EVQUE;
						evtqueNum++) {
					if ((pendingIrqs & (1u << evtqueNum)) != 0u) {
						/* Clear the error interrupt. */
						EDMA3ClrCCErr(EDMA_INST_BASE, (1u << evtqueNum));
					}
				}

				/* Transfer completion code error. */
				if ((pendingIrqs & (1 << EDMA3CC_CCERR_TCCERR_SHIFT)) != 0u) {
					EDMA3ClrCCErr(EDMA_INST_BASE,
							(0x01u << EDMA3CC_CCERR_TCCERR_SHIFT));
				}
				++index;
			}
			Cnt++;
		}
	}
}

static void HSMMCSDIsr(void* params) {
	volatile unsigned int status = 0;

	status = HSMMCSDIntrStatusGet(ctrlInfo.memBase, 0xFFFFFFFF);

	HSMMCSDIntrStatusClear(ctrlInfo.memBase, status);

	if (status & HS_MMCSD_STAT_CMDCOMP) {
		cmdCompFlag = 1;
	}

	if (status & HS_MMCSD_STAT_ERR) {
		errFlag = status & 0xFFFF0000;

		if (status & HS_MMCSD_STAT_CMDTIMEOUT) {
			cmdTimeout = 1;
		}

		if (status & HS_MMCSD_STAT_DATATIMEOUT) {
			dataTimeout = 1;
		}
	}

	if (status & HS_MMCSD_STAT_TRNFCOMP) {
		xferCompFlag = 1;
	}
}
