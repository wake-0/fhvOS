/*
 * uart.c
 *
 *  Created on: 15.03.2015
 *      Author: Kevin
 */
#include "uart.h"
#include "../am335x/hw_uart.h"

/*
 * Typedefs for better understanding
 */
typedef unsigned int address_t;

/*
 * UART default settings
 */
#define UART_DEFAULT_CLK	(48000000)

/*
 * Bits for the UART reset
 */
#define UART_SYSC_SOFT_RESET	(0x2u)
#define UART_SYSS_SOFT_RESET	(0x1u)

/*
 * Forward declarations
 */
static unsigned int getBaseAddressOfUART(uart_t);
static unsigned int getBaudRateOfUART(baudrate_t);
static unsigned int getStopBitOfUART(stopbit_t);
static unsigned int getCharLengthOfUART(charlength_t);
static unsigned int getParityOfUART(parity_t);

void UARTSoftwareReset(uart_t uart) {
	unsigned int baseAddress = getBaseAddressOfUART(uart);

	// 1. Initiate a software reset
	HWREG_SET(baseAddress + UART_SYSC_OFF, UART_SYSC_SOFT_RESET);

	// 2. Wait for the end of the reset operation
	while (HWREG_CHECK(baseAddress + UART_SYSS_OFF, UART_SYSS_SOFT_RESET))
		;
}

//TODO: add some logic to decide write/read, ...
void UARTFIFOSettings(uart_t uart) {
	unsigned int baseAddress = getBaseAddressOfUART(uart);

	// 1. Switch to register configuration mode B to access the UARTi.UART_EFR register
	// Save the current UARTi.UART_LCR register value.
	unsigned int lcrValue = HWREG(baseAddress + UART_LCR_OFF);
	// Set the UARTi.UART_LCR register value to 0x00BF.
	HWREG_SET(baseAddress + UART_LCR_OFF, UART_LCR_MODE_B);

	// 2. Enable register submode TCR_TLR to access the UARTi.UART_TLR register
	// Save the UARTi.UART_EFR[4] ENHANCED_EN value.
	unsigned int efrValue = HWREG(
			baseAddress + UART_LCR_EFR_OFF) & UART_LCR_ENHANCED_EN;
	// Set the UARTi.UART_EFR[4] ENHANCED_EN bit to 1.
	HWREG_SET(baseAddress + UART_LCR_EFR_OFF, UART_LCR_ENHANCED_EN);

	// 3. Switch to register configuration mode A to access the UARTi.UART_MCR register
	// Set the UARTi.UART_LCR register value to 0x0080.
	HWREG_WRITE(baseAddress + UART_LCR_OFF, UART_LCR_MODE_A);

	// 4. Enable register submode TCR_TLR to access the UARTi.UART_TLR register
	// Save the UARTi.UART_MCR[6] TCR_TLR value.
	unsigned int tcrTlrValue = HWREG(
			baseAddress + UART_MCR_OFF) & UART_MCR_TCR_TLR;
	// Set the UARTi.UART_MCR[6] TCR_TLR bit to 1.
	HWREG_SET(baseAddress + UART_MCR_OFF, UART_MCR_TCR_TLR);

	// 5. Enable the FIFO; load the new FIFO triggers and the new DMA mode
	// Set the UARTi.UART_FCR[7:6] RX_FIFO_TRIG
	// Set the UARTi.UART_FCR[5:4] TX_FIFO_TRIG
	// Set the UARTi.UART_FCR[3] DMA_MODE
	// Set the UARTi.UART_FCR[0] FIFO_ENABLE (0: Disable the FIFO; 1: Enable the FIFO)
	// 16 char Trigger (RX/TX), DMA-Mode 1, FIFO-Enable
	unsigned int fifoMask = 0x59;
	HWREG_SET(baseAddress + UART_FCR_OFF, fifoMask);

	// 6. Switch to register configuration mode B to access the UARTi.UART_EFR register
	// Set the UARTi.UART_LCR register value to 0x00BF.
	HWREG_SET(baseAddress + UART_LCR_OFF, UART_LCR_MODE_B);

	// 7. Load the new FIFO triggers
	// Set the UARTi.UART_TLR[7:4] RX_FIFO_TRIG_DMA
	// Set the UARTi.UART_TLR[3:0] TX_FIFO_TRIG_DMA
	// Set both 0
	unsigned int triggerMask = 0xFF;
	HWREG_UNSET(baseAddress + UART_TLR_OFF, triggerMask);

	// 8. Load the new FIFO triggers and the new DMA mode
	// Set the UARTi.UART_SCR[7] RX_TRIG_GRANU1
	// Set the UARTi.UART_SCR[6] TX_TRIG_GRANU1
	// Set the UARTi.UART_SCR[2:1] DMA_MODE_2
	// Set the UARTi.UART_SCR[0] DMA_MODE_CTL
	// Granularity enabled for RX and TX, DMA Mode both directions
	HWREG_SET(baseAddress + UART_SCR_OFF, UART_SCR_RX_TRIG_GRANU);
	HWREG_SET(baseAddress + UART_SCR_OFF, UART_SCR_TX_TRIG_GRANU);
	HWREG_SET(baseAddress + UART_SCR_OFF, UART_SCR_DMA_MODE_0);
	HWREG_UNSET(baseAddress + UART_SCR_OFF, UART_SCR_DMA_MODE_1);
	HWREG_SET(baseAddress + UART_SCR_OFF, UART_SCR_DMA_MODE_CTL);

	// 9. Restore the UARTi.UART_EFR[4] ENHANCED_EN value saved
	HWREG_WRITE(baseAddress + UART_LCR_EFR_OFF, efrValue);

	// 10. Switch to register configuration mode A to access the UARTi.UART_MCR register
	// Set the UARTi.UART_LCR register value to 0x0080.
	HWREG_WRITE(baseAddress + UART_LCR_OFF, UART_LCR_MODE_A);

	// 11. Restore the UARTi.UART_MCR[6] TCR_TLR value saved
	HWREG_WRITE(baseAddress + UART_MCR_OFF, tcrTlrValue);

	// 12. Restore the UARTi.UART_LCR value saved
	HWREG_WRITE(baseAddress + UART_LCR_OFF, lcrValue);
}

void UARTSettings(uart_t uart, configuration_t config) {
	unsigned int baseAddress = getBaseAddressOfUART(uart);

	// 1. Disable UART to access the UARTi.UART_DLL and UARTi.UART_DLH registers
	// Set the UARTi.UART_MDR1[2:0] MODE_SELECT bit field to 0x7.
	HWREG_SET(baseAddress + UART_MDR1_OFF, UART_MODE_UART);

	// 2. Switch to register configuration mode B to access the UARTi.UART_EFR register
	// Set the UARTi.UART_LCR register value to 0x00BF.
	HWREG_SET(baseAddress + UART_LCR_OFF, UART_LCR_MODE_B);

	// 3. Enable access to the UARTi.UART_IER[7:4] bit field
	// Save the UARTi.UART_EFR[4] ENHANCED_EN value.
	unsigned int efrValue = HWREG(
			baseAddress + UART_LCR_EFR_OFF) & UART_LCR_ENHANCED_EN;
	// Set the UARTi.UART_EFR[4] ENHANCED_EN bit to 1.
	HWREG_SET(baseAddress + UART_LCR_EFR_OFF, UART_LCR_ENHANCED_EN);

	// 4. Switch to register operational mode to access the UARTi.UART_IER register
	// Set the UARTi.UART_LCR register value to 0x0000.
	HWREG_WRITE(baseAddress + UART_LCR_OFF, UART_LCR_MODE_OPERATIONAL);

	// 5. Clear the UARTi.UART_IER register (set the UARTi.UART_IER[4] SLEEP_MODE bit to 0 to change
	// the UARTi.UART_DLL and UARTi.UART_DLH registers).
	//Set the UARTi.UART_IER register value to 0x0000.
	HWREG_CLEAR(baseAddress + UART_IER_OFF);

	// 6. Switch to register configuration mode B to access the UARTi.UART_DLL and UARTi.UART_DLH registers
	// Set the UARTi.UART_LCR register value to 0x00BF.
	HWREG_WRITE(baseAddress + UART_LCR_OFF, UART_LCR_MODE_B);

	// 7. Load the new divisor value
	// Set the UARTi.UART_DLL[7:0] CLOCK_LSB and UARTi.UART_DLH[5:0] CLOCK_MSB bit fields to the desired values.
	unsigned int divisorValue = UART_DEFAULT_CLK / getBaudRateOfUART(config.baudeRate);
	HWREG_WRITE(baseAddress + UART_DLL_OFF, divisorValue);
	HWREG_WRITE(baseAddress + UART_DLH_OFF, (divisorValue >> 8));

	// 8. Switch to register operational mode to access the UARTi.UART_IER register
	// Set the UARTi.UART_LCR register value to 0x0000.
	HWREG_WRITE(baseAddress + UART_LCR_OFF, UART_LCR_MODE_OPERATIONAL);

	// 9. Load the new interrupt configuration (0: Disable the interrupt; 1: Enable the interrupt)
	// Set the UARTi.UART_IER[7] CTS_IT
	// Set the UARTi.UART_IER[6] RTS_IT
	// Set the UARTi.UART_IER[5] XOFF_IT
	// Set the UARTi.UART_IER[4] SLEEP_MODE
	// Set the UARTi.UART_IER[3] MODEM_STS_IT
	// Set the UARTi.UART_IER[2] LINE_STS_IT
	// Set the UARTi.UART_IER[1] THR_IT
	// Set the UARTi.UART_IER[0] RHR_IT
	// Enable only the RHR interrupt
	HWREG_WRITE(baseAddress + UART_IER_OFF, UART_IER_RHR_IT);

	// 10. Switch to register configuration mode B to access the UARTi.UART_EFR register
	// Set the UARTi.UART_LCR register value to 0x00BF.
	HWREG_WRITE(baseAddress + UART_LCR_OFF, UART_LCR_MODE_B);

	// 11. Restore the UARTi.UART_EFR[4] ENHANCED_EN value saved
	HWREG_WRITE(baseAddress + UART_LCR_EFR_OFF, efrValue);

	// 12. Load the new protocol formatting (parity, stop-bit, character length) and switch to register operational mode:
	// Set the UARTi.UART_LCR[7] DIV_EN bit to 0.
	// Set the UARTi.UART_LCR[6] BREAK_EN bit to 0.
	// Set the UARTi.UART_LCR[5] PARITY_TYPE_2
	// Set the UARTi.UART_LCR[4] PARITY_TYPE_1
	// Set the UARTi.UART_LCR[3] PARITY_EN
	// Set the UARTi.UART_LCR[2] NB_STOP
	// Set the UARTi.UART_LCR[1:0] CHAR_LENGTH
	HWREG_UNSET(baseAddress + UART_LCR_OFF, UART_CONFIG);
	HWREG_SET(baseAddress + UART_LCR_OFF, ((getParityOfUART(config.parity) << 5) &  UART_PARITY_TYPE_2));
	HWREG_SET(baseAddress + UART_LCR_OFF, ((getParityOfUART(config.parity) << 4) &  UART_PARITY_TYPE_1));
	if (config.parity != UART_PARITY_NONE) { HWREG_SET(baseAddress + UART_LCR_OFF, UART_PARITY_EN); }
	HWREG_SET(baseAddress + UART_LCR_OFF, (getStopBitOfUART(config.stopBit) << 2));
	HWREG_SET(baseAddress + UART_LCR_OFF, getCharLengthOfUART(config.charLength));

	// 13. Load the new UART mode
	// Set the UARTi.UART_MDR1[2:0] MODE_SELECT bit field to the desired value.
	// UART 16× mode.
	HWREG_SET(baseAddress + UART_MDR1_OFF, UART_16X_MODE);
}

/*
 * Helper methods
 */
static unsigned int getBaseAddressOfUART(uart_t uart) {
	unsigned int baseAddress;
	switch (uart) {
	case UART0:
		baseAddress = UART0_BASE_ADR;
		break;
	case UART1:
		baseAddress = UART1_BASE_ADR;
		break;
	case UART2:
		baseAddress = UART2_BASE_ADR;
		break;
	case UART3:
		baseAddress = UART3_BASE_ADR;
		break;
	case UART4:
		baseAddress = UART4_BASE_ADR;
		break;
	default:
		baseAddress = UART5_BASE_ADR;
	}

	return baseAddress;
}

static unsigned int getBaudRateOfUART(baudrate_t baudRate) {
	unsigned int baude_rate;
	switch (baudRate) {
	case UART_BAUDRATE_115200:
		baude_rate = 115200;
		break;
	case UART_BAUDRATE_57600:
		baude_rate = 57600;
		break;
	case UART_BAUDRATE_38400:
		baude_rate = 38400;
		break;
	case UART_BAUDRATE_28800:
		baude_rate = 28800;
		break;
	case UART_BAUDRATE_19200:
		baude_rate = 19200;
		break;
	case UART_BAUDRATE_14400:
		baude_rate = 14400;
		break;
	case UART_BAUDRATE_9600:
		baude_rate = 9600;
		break;
	case UART_BAUDRATE_4800:
		baude_rate = 4800;
		break;
	case UART_BAUDRATE_2400:
		baude_rate = 2400;
		break;
	case UART_BAUDRATE_1200:
		baude_rate = 1200;
		break;
	default:
		baude_rate = 1;
	}

	return baude_rate;
}

static unsigned int getStopBitOfUART(stopbit_t stopBit) {
	unsigned int stop_bit;
	switch (stopBit) {
	case UART_STOPBIT_ENABLED:
		stop_bit = 1;
		break;
	default:
		stop_bit = 0;
	}

	return stop_bit;
}

static unsigned int getCharLengthOfUART(charlength_t charLength) {
	unsigned int char_length;
	switch (charLength) {
	case UART_CHARLENGTH_5:
		char_length = 0x00;
		break;
	case UART_CHARLENGTH_6:
		char_length = 0x01;
		break;
	case UART_CHARLENGTH_7:
		char_length = 0x02;
		break;
	default:
		char_length = 0x03;
	}
	return char_length;
}

static unsigned int getParityOfUART(parity_t parity) {
	unsigned int parity_uart;
	switch (parity) {
	case UART_PARITY_0:
		parity_uart = 0x03;
		break;
	case UART_PARITY_1:
		parity_uart = 0x02;
		break;
	default:
		parity_uart = 0x00;
	}

	return parity_uart;
}
