/*
 * driver_sdcard.c
 *
 *  Created on: 20.05.2015
 *      Author: Kevin
 */

#include "driver_sdcard.h"

#include "../../hal/edma/hal_edma.h"

static void EDMAInit(void);

#define EDMA_INST_BASE 				(SOC_EDMA30CC_0_REGS)
#define EDMA3_NUM_TCC 				(SOC_EDMA3_NUM_DMACH)

/* EDMA callback function array */
static void (*cb_Fxn[EDMA3_NUM_TCC]) (unsigned int tcc, unsigned int status);
static void callback(unsigned int tccNum, unsigned int status);

int SDCardInit(uint16_t id) {
	/*
	 - Initialize Clocks
	 - Software reset of the controller
	 - Set module's hardware capabilities
	 - Set module's Idle and Wake-Up modes
	 */

	// Configure clocks
	EDMAModuleClkConfig();

	EDMAInit();

	return DRIVER_OK;
}

static void EDMAInit(void) {
	// Initialization of EDMA3
	EDMA3Init(EDMA_INST_BASE, EVT_QUEUE_NUM);
	// Configuring the AINTC to receive EDMA3 interrupts.
	EDMA3AINTCConfigure();

	// Request DMA Channel and TCC for MMCSD Receive
	EDMA3RequestChannel(EDMA_INST_BASE, EDMA3_CHANNEL_TYPE_DMA, MMCSD_RX_EDMA_CHAN, MMCSD_RX_EDMA_CHAN, EVT_QUEUE_NUM);

	// Registering Callback Function for RX
	cb_Fxn[MMCSD_RX_EDMA_CHAN] = &callback;
}

int SDCardOpen(uint16_t id) {
	return DRIVER_OK;
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

/*
** This function is used as a callback from EDMA3 Completion Handler.
*/
static void callback(unsigned int tccNum, unsigned int status)
{
    //callbackOccured = 1;
    //EDMA3DisableTransfer(EDMA_INST_BASE, tccNum, EDMA3_TRIG_MODE_EVENT);
}

