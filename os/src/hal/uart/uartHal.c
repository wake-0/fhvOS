/*
 * uartHal.c
 *
 *  Created on: 15.03.2015
 *      Author: Kevin
 */
#include "uartHal.h"
#include "../am335x/hw_types.h"
#include "../am335x/hw_uart.h"
#include "../platform/platform.h"

/*
 * Typedefs for better understanding
 */
typedef uint32_t address_t;

/*
 * UART default settings
 */
#define UART_DEFAULT_CLK	(48000000)

/*
 * Bits for the UART reset
 */
#define UART_SYSC_SOFT_RESET	(1 << 1)
#define UART_SYSS_SOFT_RESET	(1 << 0)

/*
 * Forward declarations
 */
static address_t getBaseAddressOfUART(uart_t);
static uint32_t getBaudRateOfUART(baudrate_t);
static uint8_t getStopBitOfUART(stopbit_t);
static uint8_t getCharLengthOfUART(charlength_t);
static uint8_t getParityOfUART(parity_t);

/*
 * Implementations of the functions from the h file
 */
void UARTHALSoftwareReset(uart_t uart) {
	address_t baseAddress = getBaseAddressOfUART(uart);

	// 1. Initiate a software reset
	// Set the UARTi.UART_SYSC[1] SOFTRESET bit to 1.
	HWREG_SET(baseAddress + UART_SYSC_OFF, UART_SYSC_SOFT_RESET);

	// 2. Wait for the end of the reset operation
	// Poll the UARTi.UART_SYSS[0] RESETDONE bit until it equals 1.
	while (!HWREG_CHECK(baseAddress + UART_SYSS_OFF, UART_SYSS_SOFT_RESET));
}

//TODO: add some logic to decide write/read, ...
void UARTHALFIFOSettings(uart_t uart) {
	address_t baseAddress = getBaseAddressOfUART(uart);

	// 1. Switch to register configuration mode B to access the UARTi.UART_EFR register
	// Save the current UARTi.UART_LCR register value.
	uint16_t lcrValue = HWREG(baseAddress + UART_LCR_OFF);
	// Set the UARTi.UART_LCR register value to 0x00BF.
	HWREG_WRITE(baseAddress + UART_LCR_OFF, UART_LCR_MODE_B);

	// 2. Enable register submode TCR_TLR to access the UARTi.UART_TLR register
	// Save the UARTi.UART_EFR[4] ENHANCED_EN value.
	uint8_t efrValue = HWREG(baseAddress + UART_LCR_EFR_OFF) & UART_LCR_ENHANCED_EN;
	// Set the UARTi.UART_EFR[4] ENHANCED_EN bit to 1.
	HWREG_SET(baseAddress + UART_LCR_EFR_OFF, UART_LCR_ENHANCED_EN);

	// 3. Switch to register configuration mode A to access the UARTi.UART_MCR register
	// Set the UARTi.UART_LCR register value to 0x0080.
	HWREG_WRITE(baseAddress + UART_LCR_OFF, UART_LCR_MODE_A);

	// 4. Enable register submode TCR_TLR to access the UARTi.UART_TLR register
	// Save the UARTi.UART_MCR[6] TCR_TLR value.
	uint8_t tcrTlrValue = HWREG(baseAddress + UART_MCR_OFF) & UART_MCR_TCR_TLR;
	// Set the UARTi.UART_MCR[6] TCR_TLR bit to 1.
	HWREG_SET(baseAddress + UART_MCR_OFF, UART_MCR_TCR_TLR);

	// 5. Enable the FIFO; load the new FIFO triggers and the new DMA mode
	// Set the UARTi.UART_FCR[7:6] RX_FIFO_TRIG
	// Set the UARTi.UART_FCR[5:4] TX_FIFO_TRIG
	// Set the UARTi.UART_FCR[3] DMA_MODE
	// Set the UARTi.UART_FCR[0] FIFO_ENABLE (0: Disable the FIFO; 1: Enable the FIFO)
	// 8 char Trigger (RX/TX), DMA-Mode 1, FIFO-Enable
	HWREG_UNSET(baseAddress + UART_FCR_OFF, 0xF9);
	// RX_FIFO_TRIG: 8 chars  --> [7:6] = 0:0
	// TX_FIFO_TROG: 8 spaces --> [5:4] = 0:0
	// DMA_MODE:	 1 (UART_NDMA_REQ[0] in TX, UART_NDMA_REQ[1] in RX).
	// FIFO_ENABLE:  1 Enables the transmit and receive FIFOs. The transmit and receive holding registers are 1-byte
	HWREG_SET(baseAddress + UART_FCR_OFF, (1 << 0));

	// 6. Switch to register configuration mode B to access the UARTi.UART_EFR register
	// Set the UARTi.UART_LCR register value to 0x00BF.
	HWREG_WRITE(baseAddress + UART_LCR_OFF, UART_LCR_MODE_B);

	// 7. Load the new FIFO triggers
	// Set the UARTi.UART_TLR[7:4] RX_FIFO_TRIG_DMA
	// Set the UARTi.UART_TLR[3:0] TX_FIFO_TRIG_DMA
	// Reset RX_FIFO_TRIG_DMA and TX_FIFO_TRIG_DMA to 0 --> defined by FCR[5:4]
	HWREG_UNSET(baseAddress + UART_TLR_OFF, 0xFF);

	// 8. Load the new FIFO triggers and the new DMA mode
	// Set the UARTi.UART_SCR[7] RX_TRIG_GRANU1
	// Set the UARTi.UART_SCR[6] TX_TRIG_GRANU1
	// Set the UARTi.UART_SCR[2:1] DMA_MODE_2
	// Set the UARTi.UART_SCR[0] DMA_MODE_CTL
	HWREG_UNSET(baseAddress + UART_SCR_OFF, 0xC7);
	// DMA_MODE_CTL: 1 The DMAMODE is set with SCR[2:1].
	// DMA_MODE_2: 0x03 DMA mode 3 (UARTnDMAREQ[0] in TX)
	HWREG_SET(baseAddress + UART_SCR_OFF, (1 << 0) | (1 << 1) | (1 << 2));

	// 9. Restore the UARTi.UART_EFR[4] ENHANCED_EN value saved
	if(efrValue & UART_LCR_ENHANCED_EN) {
		HWREG_SET(baseAddress + UART_LCR_EFR_OFF, efrValue);
	} else {
		HWREG_UNSET(baseAddress + UART_LCR_EFR_OFF, efrValue);
	}

	// 10. Switch to register configuration mode A to access the UARTi.UART_MCR register
	// Set the UARTi.UART_LCR register value to 0x0080.
	HWREG_WRITE(baseAddress + UART_LCR_OFF, UART_LCR_MODE_A);

	// 11. Restore the UARTi.UART_MCR[6] TCR_TLR value saved
	if (tcrTlrValue & UART_MCR_TCR_TLR) {
		HWREG_SET(baseAddress + UART_MCR_OFF, tcrTlrValue);
	} else  {
		HWREG_UNSET(baseAddress + UART_MCR_OFF, tcrTlrValue);
	}

	// 12. Restore the UARTi.UART_LCR value saved
	HWREG_WRITE(baseAddress + UART_LCR_OFF, lcrValue);
}

void UARTHALSettings(uart_t uart, configuration_t config) {
	address_t baseAddress = getBaseAddressOfUART(uart);

	// 1. Disable UART to access the UARTi.UART_DLL and UARTi.UART_DLH registers
	// Set the UARTi.UART_MDR1[2:0] MODE_SELECT bit field to 0x7.
	HWREG_SET(baseAddress + UART_MDR1_OFF, UART_MODE_DISABLE_UART);

	// 2. Switch to register configuration mode B to access the UARTi.UART_EFR register
	// Set the UARTi.UART_LCR register value to 0x00BF.
	HWREG_WRITE(baseAddress + UART_LCR_OFF, UART_LCR_MODE_B);

	// 3. Enable access to the UARTi.UART_IER[7:4] bit field
	// Save the UARTi.UART_EFR[4] ENHANCED_EN value.
	uint8_t efrValue = HWREG(baseAddress + UART_LCR_EFR_OFF) & UART_LCR_ENHANCED_EN;
	// Set the UARTi.UART_EFR[4] ENHANCED_EN bit to 1.
	HWREG_SET(baseAddress + UART_LCR_EFR_OFF, UART_LCR_ENHANCED_EN);

	// 4. Switch to register operational mode to access the UARTi.UART_IER register
	// Set the UARTi.UART_LCR register value to 0x0000.
	HWREG_WRITE(baseAddress + UART_LCR_OFF, UART_LCR_MODE_OPERATIONAL);

	// 5. Clear the UARTi.UART_IER register (set the UARTi.UART_IER[4] SLEEP_MODE bit to 0 to change
	// the UARTi.UART_DLL and UARTi.UART_DLH registers).
	//Set the UARTi.UART_IER register value to 0x0000.
	HWREG_UNSET(baseAddress + UART_IER_OFF, 0xFFFF);

	// 6. Switch to register configuration mode B to access the UARTi.UART_DLL and UARTi.UART_DLH registers
	// Set the UARTi.UART_LCR register value to 0x00BF.
	HWREG_WRITE(baseAddress + UART_LCR_OFF, UART_LCR_MODE_B);

	// 7. Load the new divisor value
	// Set the UARTi.UART_DLL[7:0] CLOCK_LSB and UARTi.UART_DLH[5:0] CLOCK_MSB bit fields to the desired values.
	uint16_t divisorValue = UART_DEFAULT_CLK / (16 * getBaudRateOfUART(config.baudeRate));
	//HWREG_WRITE(baseAddress + UART_DLL_OFF, divisorValue);
	//HWREG_WRITE(baseAddress + UART_DLH_OFF, (divisorValue >> 8));
	// TODO: fix baudrate Test with 9600
	HWREG_UNSET(baseAddress + UART_DLL_OFF, 0xFF);
	HWREG_SET(baseAddress + UART_DLL_OFF, 0x38);
	HWREG_UNSET(baseAddress + UART_DLH_OFF, 0xFF);
	HWREG_SET(baseAddress + UART_DLH_OFF, 0x01);

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
	HWREG_SET(baseAddress + UART_IER_OFF, 0xFF);

	// 10. Switch to register configuration mode B to access the UARTi.UART_EFR register
	// Set the UARTi.UART_LCR register value to 0x00BF.
	HWREG_WRITE(baseAddress + UART_LCR_OFF, UART_LCR_MODE_B);

	// 11. Restore the UARTi.UART_EFR[4] ENHANCED_EN value saved
	if(efrValue & UART_LCR_ENHANCED_EN) {
		HWREG_SET(baseAddress + UART_LCR_EFR_OFF, efrValue);
	} else {
		HWREG_UNSET(baseAddress + UART_LCR_EFR_OFF, efrValue);
	}

	// 12. Load the new protocol formatting (parity, stop-bit, character length) and switch to register operational mode:
	// Set the UARTi.UART_LCR[7] DIV_EN bit to 0.
	// Set the UARTi.UART_LCR[6] BREAK_EN bit to 0.
	// Set the UARTi.UART_LCR[5] PARITY_TYPE_2
	// Set the UARTi.UART_LCR[4] PARITY_TYPE_1
	// Set the UARTi.UART_LCR[3] PARITY_EN
	// Set the UARTi.UART_LCR[2] NB_STOP
	// Set the UARTi.UART_LCR[1:0] CHAR_LENGTH
	HWREG_CLEAR(baseAddress + UART_LCR_OFF);
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

void UARTHALFIFOWrite(uart_t uart, char* msg)
{
	address_t baseAddress = getBaseAddressOfUART(uart);
	HWREG(baseAddress + UART_THR) = *(msg);
}

/*
 * Helper methods
 */
static address_t getBaseAddressOfUART(uart_t uart) {
	address_t baseAddress;
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

static uint32_t getBaudRateOfUART(baudrate_t baudRate) {
	uint32_t baude_rate;
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

static uint8_t getStopBitOfUART(stopbit_t stopBit) {
	uint8_t stop_bit;
	switch (stopBit) {
	case UART_STOPBIT_1:
		stop_bit = 1;
		break;
	default:
		stop_bit = 0;
	}

	return stop_bit;
}

static uint8_t getCharLengthOfUART(charlength_t charLength) {
	uint8_t char_length;
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

static uint8_t getParityOfUART(parity_t parity) {
	uint8_t parity_uart;
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
