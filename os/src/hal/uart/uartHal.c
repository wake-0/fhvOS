/*
 * uartHal.c
 *
 *  Created on: 15.03.2015
 *      Author: Kevin
 */
#include "uartHal.h"
#include "../am335x/hw_types.h"
#include "../am335x/hw_uart.h"

/*
 * Typedefs for better understanding
 */
typedef uint32_t address_t;

/*
 * General defines
 */
#define UART_BASE_ADR_NOT_FOUND 0x00

/*
 * Bits for the UART reset
 */
#define UART_SYSC_SOFT_RESET	(1 << 1)
#define UART_SYSS_SOFT_RESET	(1 << 0)

/*
 * Forward declarations
 */
static address_t getBaseAddressOfUART(uartPins_t uartPins);
static uint32_t getBaudRateOfUART(baudrate_t baudRate);

static void switchToConfModeA(address_t baseAddress);
static void switchToConfModeB(address_t baseAddress);
static void switchToConfModeOp(address_t baseAddress);

/*
 * Implementations of the functions from the h file
 */
int UARTHalSoftwareReset(uartPins_t uartPins) {
	address_t baseAddress = getBaseAddressOfUART(uartPins);
	if (baseAddress == UART_BASE_ADR_NOT_FOUND) {
		return UART_HAL_ERROR;
	}

	// 1. Initiate a software reset
	// Set the UARTi.UART_SYSC[1] SOFTRESET bit to 1.
	HWREG_SET(baseAddress + UART_SYSC_OFF, UART_SYSC_SOFT_RESET);

	// 2. Wait for the end of the reset operation
	// Poll the UARTi.UART_SYSS[0] RESETDONE bit until it equals 1.
	while (!HWREG_CHECK(baseAddress + UART_SYSS_OFF, UART_SYSS_SOFT_RESET)) {
	}

	return UART_HAL_OK;
}

//TODO: add some logic to decide write/read, ...
int UARTHalFifoSettings(uartPins_t uartPins) {
	address_t baseAddress = getBaseAddressOfUART(uartPins);
	if (baseAddress == UART_BASE_ADR_NOT_FOUND) {
		return UART_HAL_ERROR;
	}

	// 1. Switch to register configuration mode B to access the UARTi.UART_EFR register
	// Save the current UARTi.UART_LCR register value.
	uint8_t lcrValue = HWREG(baseAddress + UART_LCR_OFF);
	// Set the UARTi.UART_LCR register value to 0x00BF.
	switchToConfModeB(baseAddress);

	// 2. Enable register submode TCR_TLR to access the UARTi.UART_TLR register
	// Save the UARTi.UART_EFR[4] ENHANCED_EN value.
	boolean_t efrValue = (HWREG(baseAddress + UART_LCR_EFR_OFF)
			& UART_LCR_ENHANCED_EN) != 0;
	// Set the UARTi.UART_EFR[4] ENHANCED_EN bit to 1.
	HWREG_SET(baseAddress + UART_LCR_EFR_OFF, UART_LCR_ENHANCED_EN);

	// 3. Switch to register configuration mode A to access the UARTi.UART_MCR register
	// Set the UARTi.UART_LCR register value to 0x0080.
	switchToConfModeA(baseAddress);

	// 4. Enable register submode TCR_TLR to access the UARTi.UART_TLR register
	// Save the UARTi.UART_MCR[6] TCR_TLR value.
	boolean_t tcrTlrValue = (HWREG(baseAddress + UART_MCR_OFF)
			& UART_MCR_TCR_TLR) != 0;
	// Set the UARTi.UART_MCR[6] TCR_TLR bit to 1.
	HWREG_SET(baseAddress + UART_MCR_OFF, UART_MCR_TCR_TLR);

	// 5. Enable the FIFO; load the new FIFO triggers and the new DMA mode
	// Set the UARTi.UART_FCR[7:6] RX_FIFO_TRIG
	// Set the UARTi.UART_FCR[5:4] TX_FIFO_TRIG
	// Set the UARTi.UART_FCR[3] DMA_MODE
	// Set the UARTi.UART_FCR[0] FIFO_ENABLE (0: Disable the FIFO; 1: Enable the FIFO)
	HWREG_UNSET(baseAddress + UART_FCR_OFF, 0b11111001);

	// 6. Switch to register configuration mode B to access the UARTi.UART_EFR register
	// Set the UARTi.UART_LCR register value to 0x00BF.
	switchToConfModeB(baseAddress);

	// 7. Load the new FIFO triggers
	// Set the UARTi.UART_TLR[7:4] RX_FIFO_TRIG_DMA
	// Set the UARTi.UART_TLR[3:0] TX_FIFO_TRIG_DMA
	HWREG_UNSET(baseAddress + UART_TLR_OFF, 0xFF);

	// 8. Load the new FIFO triggers and the new DMA mode
	// Set the UARTi.UART_SCR[7] RX_TRIG_GRANU1
	// Set the UARTi.UART_SCR[6] TX_TRIG_GRANU1
	// Set the UARTi.UART_SCR[2:1] DMA_MODE_2
	// Set the UARTi.UART_SCR[0] DMA_MODE_CTL
	HWREG_UNSET(baseAddress + UART_SCR_OFF, 0b11000111);

	// 9. Restore the UARTi.UART_EFR[4] ENHANCED_EN value saved
	if (efrValue) {
		HWREG_SET(baseAddress + UART_LCR_EFR_OFF, UART_LCR_ENHANCED_EN);
	} else {
		HWREG_UNSET(baseAddress + UART_LCR_EFR_OFF, UART_LCR_ENHANCED_EN);
	}

	// 10. Switch to register configuration mode A to access the UARTi.UART_MCR register
	// Set the UARTi.UART_LCR register value to 0x0080.
	switchToConfModeA(baseAddress);

	// 11. Restore the UARTi.UART_MCR[6] TCR_TLR value saved
	if (tcrTlrValue) {
		HWREG_SET(baseAddress + UART_MCR_OFF, UART_MCR_TCR_TLR);
	} else {
		HWREG_UNSET(baseAddress + UART_MCR_OFF, UART_MCR_TCR_TLR);
	}

	// 12. Restore the UARTi.UART_LCR value saved
	HWREG_WRITE(baseAddress + UART_LCR_OFF, lcrValue);

	return UART_HAL_OK;
}

int UARTHalSettings(uartPins_t uartPins, configuration_t* config) {
	address_t baseAddress = getBaseAddressOfUART(uartPins);
	if (baseAddress == UART_BASE_ADR_NOT_FOUND) {
		return UART_HAL_ERROR;
	}

	// 1. Disable UART to access the UARTi.UART_DLL and UARTi.UART_DLH registers
	// Set the UARTi.UART_MDR1[2:0] MODE_SELECT bit field to 0x7.
	HWREG_SET(baseAddress + UART_MDR1_OFF, UART_MODE_DISABLE_UART);

	// 2. Switch to register configuration mode B to access the UARTi.UART_EFR register
	// Set the UARTi.UART_LCR register value to 0x00BF.
	switchToConfModeB(baseAddress);

	// 3. Enable access to the UARTi.UART_IER[7:4] bit field
	// Save the UARTi.UART_EFR[4] ENHANCED_EN value.
	boolean_t efrValue = (HWREG(baseAddress + UART_LCR_EFR_OFF)
			& UART_LCR_ENHANCED_EN) != 0;
	// Set the UARTi.UART_EFR[4] ENHANCED_EN bit to 1.
	HWREG_SET(baseAddress + UART_LCR_EFR_OFF, UART_LCR_ENHANCED_EN);

	// 4. Switch to register operational mode to access the UARTi.UART_IER register
	// Set the UARTi.UART_LCR register value to 0x0000.
	switchToConfModeOp(baseAddress);

	// 5. Clear the UARTi.UART_IER register (set the UARTi.UART_IER[4] SLEEP_MODE bit to 0 to change
	// the UARTi.UART_DLL and UARTi.UART_DLH registers).
	//Set the UARTi.UART_IER register value to 0x0000.
	HWREG_WRITE(baseAddress + UART_IER_OFF, 0x00);

	// 6. Switch to register configuration mode B to access the UARTi.UART_DLL and UARTi.UART_DLH registers
	// Set the UARTi.UART_LCR register value to 0x00BF.
	switchToConfModeB(baseAddress);

	// 7. Load the new divisor value
	// Set the UARTi.UART_DLL[7:0] CLOCK_LSB and UARTi.UART_DLH[5:0] CLOCK_MSB bit fields to the desired values.
	uint16_t divisorValue = UART_DEFAULT_CLK
			/ (16 * getBaudRateOfUART(config->baudRate));
	HWREG_WRITE(baseAddress + UART_DLL_OFF, divisorValue);
	HWREG_WRITE(baseAddress + UART_DLH_OFF, (divisorValue >> 8));

	// 8. Switch to register operational mode to access the UARTi.UART_IER register
	// Set the UARTi.UART_LCR register value to 0x0000.
	switchToConfModeOp(baseAddress);

	// 9. Load the new interrupt configuration (0: Disable the interrupt; 1: Enable the interrupt)
	// Set the UARTi.UART_IER[7] CTS_IT
	// Set the UARTi.UART_IER[6] RTS_IT
	// Set the UARTi.UART_IER[5] XOFF_IT
	// Set the UARTi.UART_IER[4] SLEEP_MODE
	// Set the UARTi.UART_IER[3] MODEM_STS_IT
	// Set the UARTi.UART_IER[2] LINE_STS_IT
	// Set the UARTi.UART_IER[1] THR_IT
	// Set the UARTi.UART_IER[0] RHR_IT
	HWREG_UNSET(baseAddress + UART_IER_OFF, 0xFF);

	// 10. Switch to register configuration mode B to access the UARTi.UART_EFR register
	// Set the UARTi.UART_LCR register value to 0x00BF.
	switchToConfModeB(baseAddress);

	// 11. Restore the UARTi.UART_EFR[4] ENHANCED_EN value saved
	if (efrValue) {
		HWREG_SET(baseAddress + UART_LCR_EFR_OFF, UART_LCR_ENHANCED_EN);
	} else {
		HWREG_UNSET(baseAddress + UART_LCR_EFR_OFF, UART_LCR_ENHANCED_EN);
	}

	// 12. Load the new protocol formatting (parity, stop-bit, character length) and switch to register operational mode:
	// Set the UARTi.UART_LCR[7] DIV_EN bit to 0.
	// Set the UARTi.UART_LCR[6] BREAK_EN bit to 0.
	// Set the UARTi.UART_LCR[5] PARITY_TYPE_2
	// Set the UARTi.UART_LCR[4] PARITY_TYPE_1
	// Set the UARTi.UART_LCR[3] PARITY_EN
	// Set the UARTi.UART_LCR[2] NB_STOP
	// Set the UARTi.UART_LCR[1:0] CHAR_LENGTH
	HWREG_UNSET(baseAddress + UART_LCR_OFF, 0xFF);

	switch (config->stopBit) {
	case UART_STOPBIT_2:
		HWREG_SET(baseAddress + UART_LCR_OFF, (1 << 2));
		break;
	case UART_STOPBIT_1:
	default:
		HWREG_UNSET(baseAddress + UART_LCR_OFF, (1 << 2));
		break;
	}

	// set parity type
	switch (config->parity) {
	case UART_PARITY_NONE:
		break;

	case UART_PARITY_0:
		HWREG_SET(baseAddress + UART_LCR_OFF, (1 << 3)); // Parity enable
		HWREG_SET(baseAddress + UART_LCR_OFF, (1 << 4)); // PARITY_TYPE_1 (Even)
		HWREG_SET(baseAddress + UART_LCR_OFF, (1 << 5)); // PARITY_TYPE_2
		break;

	case UART_PARITY_1:
		HWREG_SET(baseAddress + UART_LCR_OFF, (1 << 3)); // Parity enable
		HWREG_SET(baseAddress + UART_LCR_OFF, (1 << 5)); // PARITY_TYPE_2
		break;
	}

	// set char length
	HWREG_WRITE(baseAddress + UART_LCR_OFF,
			HWREG(baseAddress + UART_LCR_OFF) | config->charLength);

	// 13. Load the new UART mode
	// Set the UARTi.UART_MDR1[2:0] MODE_SELECT bit field to the desired value.
	HWREG_UNSET(baseAddress + UART_MDR1_OFF, UART_MODE_DISABLE_UART);

	return UART_HAL_OK;
}

int UARTHalFifoWrite(uartPins_t uartPins, uint8_t* msg) {
	address_t baseAddress = getBaseAddressOfUART(uartPins);
	HWREG(baseAddress + UART_THR_OFF) = *(msg);

	return UART_HAL_OK;
}

/*
 * Helper methods
 */
void switchToConfModeA(address_t baseAddress) {
	// Set the UARTi.UART_LCR register value to 0x0080.
	HWREG_WRITE(baseAddress + UART_LCR_OFF, UART_LCR_MODE_A);
}

void switchToConfModeB(address_t baseAddress) {
	// Set the UARTi.UART_LCR register value to 0x00BF.
	HWREG_WRITE(baseAddress + UART_LCR_OFF, UART_LCR_MODE_B);
}

void switchToConfModeOp(address_t baseAddress) {
	// Set the UARTi.UART_LCR register value to 0x0000.
	HWREG_WRITE(baseAddress + UART_LCR_OFF, UART_LCR_MODE_OPERATIONAL);
}

static address_t getBaseAddressOfUART(uartPins_t uartPins) {
	if (uartPins.rxd == BOARD_UART0_RXD && uartPins.txd == BOARD_UART0_TXD) {
		return UART0_BASE_ADR;
	}
	if (uartPins.rxd == BOARD_UART1_RXD && uartPins.txd == BOARD_UART1_TXD) {
		return UART1_BASE_ADR;
	}
	if (uartPins.rxd == BOARD_UART2_RXD && uartPins.txd == BOARD_UART2_TXD) {
		return UART2_BASE_ADR;
	}
	if (uartPins.rxd == BOARD_UART4_RXD && uartPins.txd == BOARD_UART4_TXD) {
		return UART4_BASE_ADR;
	}
	if (uartPins.rxd == BOARD_UART5_RXD && uartPins.txd == BOARD_UART5_TXD) {
		return UART5_BASE_ADR;
	}

	return UART_BASE_ADR_NOT_FOUND;
}

static uint32_t getBaudRateOfUART(baudrate_t baudRate) {
	uint32_t baud_rate;
	switch (baudRate) {
	case UART_BAUDRATE_115200:
		baud_rate = 115200;
		break;
	case UART_BAUDRATE_57600:
		baud_rate = 57600;
		break;
	case UART_BAUDRATE_38400:
		baud_rate = 38400;
		break;
	case UART_BAUDRATE_28800:
		baud_rate = 28800;
		break;
	case UART_BAUDRATE_19200:
		baud_rate = 19200;
		break;
	case UART_BAUDRATE_14400:
		baud_rate = 14400;
		break;
	case UART_BAUDRATE_9600:
		baud_rate = 9600;
		break;
	case UART_BAUDRATE_4800:
		baud_rate = 4800;
		break;
	case UART_BAUDRATE_2400:
		baud_rate = 2400;
		break;
	case UART_BAUDRATE_1200:
		baud_rate = 1200;
		break;
	default:
		baud_rate = 1;
	}

	return baud_rate;
}
