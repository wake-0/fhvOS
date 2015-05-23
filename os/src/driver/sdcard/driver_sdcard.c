/*
 * driver_sdcard.c
 *
 *  Created on: 20.05.2015
 *      Author: Kevin
 */

#include "driver_sdcard.h"
#include "mmcsd_proto.h"

#include "../../hal/edma/hal_edma.h"
#include "../../hal/edma/hs_mmcsd.h"
#include "../../hal/am335x/soc_AM335x.h"
#include "../../hal/am335x/hw_edma3cc.h"

struct _mmcsdCtrlInfo;

volatile unsigned int callbackOccured = 0;
volatile unsigned int xferCompFlag = 0;
volatile unsigned int dataTimeout = 0;
volatile unsigned int cmdCompFlag = 0;
volatile unsigned int cmdTimeout = 0;
volatile unsigned int errFlag = 0;

#define HSMMCSD_CARD_DETECT_PINNUM 	6
#define HSMMCSD_IN_FREQ 			96000000 /* 96MHz */
#define HSMMCSD_INIT_FREQ 			400000 /* 400kHz */

#define EDMA_INST_BASE 				(SOC_EDMA30CC_0_REGS)
#define MMCSD_INST_BASE 			(SOC_MMCHS_0_REGS)
#define HS_MMCSD_SUPPORT_VOLT_1P8       (MMCHS_CAPA_VS18)
#define HS_MMCSD_SUPPORT_VOLT_3P0       (MMCHS_CAPA_VS30)
#define EDMA3_TRIG_MODE_EVENT 		(2u)
#define EDMA3_TRIG_MODE_MANUAL 		(0u)
#define EDMA3_TRIG_MODE_QDMA 		(1u)
#define MMCSD_RX_EDMA_CHAN (EDMA3_CHA_MMCSD0_RX)

static void EDMAInit(void);
static void HSMMCSDControllerSetup(void);
static unsigned int HSMMCSDControllerInit(mmcsdCtrlInfo *ctrl);
static unsigned int HSMMCSDCardPresent(mmcsdCtrlInfo *ctrl);
static void HSMMCSDIntEnable(mmcsdCtrlInfo *ctrl);
static void HSMMCSDXferSetup(mmcsdCtrlInfo *ctrl, unsigned char rwFlag, void *ptr, unsigned int blkSize, unsigned int nBlks);
static int HSMMCSDBusFreqConfig(mmcsdCtrlInfo *ctrl, unsigned int busFreq);
static void HSMMCSDBusWidthConfig(mmcsdCtrlInfo *ctrl, unsigned int busWidth);
static unsigned int HSMMCSDCmdSend(mmcsdCtrlInfo *ctrl, mmcsdCmd *c);
static unsigned int HSMMCSDCmdStatusGet(mmcsdCtrlInfo *ctrl);
static unsigned int HSMMCSDXferStatusGet(mmcsdCtrlInfo *ctrl);
static void HSMMCSDRxDmaConfig(void *ptr, unsigned int blkSize, unsigned int nblks);
static unsigned int MMCSDCtrlInit(mmcsdCtrlInfo *ctrl);
static void MMCSDIntEnable(mmcsdCtrlInfo *ctrl);
static void HSMMCSDFsMount(unsigned int driveNum, void *ptr);

/* EDMA callback function array */
static void (*cb_Fxn[EDMA3_NUM_TCC])(unsigned int tcc, unsigned int status);
static void callback(unsigned int tccNum, unsigned int status);

/* SD Controller info structure */
mmcsdCtrlInfo ctrlInfo;
/* SD card info structure */
mmcsdCardInfo sdCard;

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
	volatile unsigned int i = 0;
	volatile unsigned int initFlg = 1;

	while (1) {
		if ((HSMMCSDCardPresent(&ctrlInfo)) == 1) {
			if (initFlg) {
				HSMMCSDFsMount(0, &sdCard);
				initFlg = 0;
			}
			return 1;
		} else {
			i = (i + 1) & 0xFFF;

			if (i % 20 == 1) {
				// TODO: check what should be done, when no SD card is insert
				// printf("FS: Please insert the card \n\r");
			}

			if (initFlg != 1) {
				/* Reinitialize all the state variables */
				callbackOccured = 0;
				xferCompFlag = 0;
				dataTimeout = 0;
				cmdCompFlag = 0;
				cmdTimeout = 0;

				/* Initialize the MMCSD controller */
				MMCSDCtrlInit(&ctrlInfo);

				MMCSDIntEnable(&ctrlInfo);
			}

			initFlg = 1;
		}
	}
}

int SDCardClose(uint16_t id) {
	return DRIVER_OK;
}

int SDCardWrite(uint16_t id, char* buf, uint16_t len) {
	return DRIVER_OK;
}

int SDCardRead(uint16_t id, char* buf, uint16_t len) {
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

static unsigned int HSMMCSDCardPresent(mmcsdCtrlInfo *ctrl) {
	return HSMMCSDIsCardInserted(ctrl->memBase);
}

static void HSMMCSDIntEnable(mmcsdCtrlInfo *ctrl) {
	HSMMCSDIntrEnable(ctrl->memBase, ctrl->intrMask);
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

static unsigned int HSMMCSDControllerInit(mmcsdCtrlInfo *ctrl) {
	int status = 0;

	/*Refer to the MMC Host and Bus configuration steps in TRM */
	/* controller Reset */
	status = HSMMCSDSoftReset(ctrl->memBase);

	const char * txt1 = "HS MMC/SD Reset failed\n\r";
	const char * txt2 = "HS MMC/SD Power on failed\n\r";
	const char * txt3 = "HS MMC/SD Bus Frequency set failed\n\r";

	if (status != 0) {
		// TODO: fix write
		//UartWrite(SOC_UART_0_REGS, txt1, strlen(txt1));
	}

	/* Lines Reset */
	HSMMCSDLinesReset(ctrl->memBase, HS_MMCSD_ALL_RESET);

	/* Set supported voltage list */
	HSMMCSDSupportedVoltSet(ctrl->memBase, HS_MMCSD_SUPPORT_VOLT_1P8 |
	HS_MMCSD_SUPPORT_VOLT_3P0);

	HSMMCSDSystemConfig(ctrl->memBase, HS_MMCSD_AUTOIDLE_ENABLE);

	/* Set the bus width */
	HSMMCSDBusWidthSet(ctrl->memBase, HS_MMCSD_BUS_WIDTH_1BIT);

	/* Set the bus voltage */
	HSMMCSDBusVoltSet(ctrl->memBase, HS_MMCSD_BUS_VOLT_3P0);

	/* Bus power on */
	status = HSMMCSDBusPower(ctrl->memBase, HS_MMCSD_BUS_POWER_ON);

	if (status != 0) {
		// TODO: fix write
		//UartWrite(SOC_UART_0_REGS, txt2, strlen(txt2));
	}

	/* Set the initialization frequency */
	status = HSMMCSDBusFreqSet(ctrl->memBase, ctrl->ipClk, ctrl->opClk, 0);
	if (status != 0) {
		// TODO: fix write
		//UartWrite(SOC_UART_0_REGS, txt3, strlen(txt3));
	}

	HSMMCSDInitStreamSend(ctrl->memBase);

	status = (status == 0) ? 1 : 0;

	return status;
}

static int HSMMCSDBusFreqConfig(mmcsdCtrlInfo *ctrl, unsigned int busFreq) {
	return HSMMCSDBusFreqSet(ctrl->memBase, ctrl->ipClk, busFreq, 0);
}

static void HSMMCSDBusWidthConfig(mmcsdCtrlInfo *ctrl, unsigned int busWidth) {
	if (busWidth == SD_BUS_WIDTH_1BIT) {
		HSMMCSDBusWidthSet(ctrl->memBase, HS_MMCSD_BUS_WIDTH_1BIT);
	} else {
		HSMMCSDBusWidthSet(ctrl->memBase, HS_MMCSD_BUS_WIDTH_4BIT);
	}
}

static unsigned int HSMMCSDCmdSend(mmcsdCtrlInfo *ctrl, mmcsdCmd *c) {
	unsigned int cmdType = HS_MMCSD_CMD_TYPE_NORMAL;
	unsigned int dataPresent;
	unsigned int status = 0;
	unsigned int rspType;
	unsigned int cmdDir;
	unsigned int nblks;
	unsigned int cmd;

	if (c->flags & SD_CMDRSP_STOP) {
		cmdType = HS_MMCSD_CMD_TYPE_SUSPEND;
	} else if (c->flags & SD_CMDRSP_FS) {
		cmdType = HS_MMCSD_CMD_TYPE_FUNCSEL;
	} else if (c->flags & SD_CMDRSP_ABORT) {
		cmdType = HS_MMCSD_CMD_TYPE_ABORT;
	}

	cmdDir = (c->flags & SD_CMDRSP_READ) ?
	HS_MMCSD_CMD_DIR_READ :
											HS_MMCSD_CMD_DIR_WRITE;

	dataPresent = (c->flags & SD_CMDRSP_DATA) ? 1 : 0;
	nblks = (dataPresent == 1) ? c->nblks : 0;

	if (c->flags & SD_CMDRSP_NONE) {
		rspType = HS_MMCSD_NO_RESPONSE;
	} else if (c->flags & SD_CMDRSP_136BITS) {
		rspType = HS_MMCSD_136BITS_RESPONSE;
	} else if (c->flags & SD_CMDRSP_BUSY) {
		rspType = HS_MMCSD_48BITS_BUSY_RESPONSE;
	} else {
		rspType = HS_MMCSD_48BITS_RESPONSE;
	}

	cmd = HS_MMCSD_CMD(c->idx, cmdType, rspType, cmdDir);

	if (dataPresent) {
		HSMMCSDIntrStatusClear(ctrl->memBase, HS_MMCSD_STAT_TRNFCOMP);

		HSMMCSDDataTimeoutSet(ctrl->memBase, HS_MMCSD_DATA_TIMEOUT(27));
	}

	HSMMCSDCommandSend(ctrl->memBase, cmd, c->arg, (void*) dataPresent, nblks,
			ctrl->dmaEnable);

	if (ctrl->cmdStatusGet) {
		status = ctrl->cmdStatusGet(ctrl);
	}

	if (status == 1) {
		HSMMCSDResponseGet(ctrl->memBase, c->rsp);
	}

	return status;
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

static unsigned int MMCSDCtrlInit(mmcsdCtrlInfo *ctrl) {
	return ctrl->ctrlInit(ctrl);
}

static void MMCSDIntEnable(mmcsdCtrlInfo *ctrl) {
	ctrl->intrEnable(ctrl);

	return;
}

static void HSMMCSDFsMount(unsigned int driveNum, void *ptr)
{
	/*
    f_mount(driveNum, &g_sFatFs);
    fat_devices[driveNum].dev = ptr;
    fat_devices[driveNum].fs = &g_sFatFs;
    fat_devices[driveNum].initDone = 0;
    */
}
