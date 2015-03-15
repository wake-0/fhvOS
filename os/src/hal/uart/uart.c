/*
 * uart.c
 *
 *  Created on: 15.03.2015
 *      Author: Kevin
 */
#include "uart.h"

/*
 * Makros for SET and CHECK HWREG
 */
#define HWREG(address) 					(*((volatile unsigned int *)(address)))
#define HWREG_SET(address,bit) 			(HWREG(address) |= bit)
#define HWREG_UNSET(address,bit) 		(HWREG(address) &= ~bit)
#define HWREG_CHECK(address, checkbit)	((HWREG(address) & checkbit) != checkbit)
#define HWREG_CLEAR(address)			(HWREG(address) &= 0xFFFF)
#define HWREG_WRITE(address, value)		(HWREG(address) = value)

/*
 * Typedefs for better understanding
 */
typedef unsigned int address_t;

/*
 * UART base addresses
 */
#define UART0_BASE_ADR 		0x44E09000
#define UART1_BASE_ADR 		0x48022000
#define UART2_BASE_ADR 		0x48024000
#define UART3_BASE_ADR 		0x481A6000
#define UART4_BASE_ADR 		0x481A8000
#define UART5_BASE_ADR 		0x481AA000

/*
 * Bits for the UART reset
 */
#define UART_SYSC_SOFT_RESET	0x2u
#define UART_SYSS_SOFT_RESET	0x1u

/*
 * Offsets
 */
#define UART_SYSC_OFF 			0x54	// System Configuration Register
#define UART_SYSS_OFF 			0x58	// System Status Register
#define UART_LCR_OFF			0x0C	// Line Control Register
#define UART_LCR_EFR_OFF		0x08	// Enhanced Feature Register
#define UART_MCR_OFF			0x10	// Modem Control Register
#define UART_FCR_OFF			0x08	// FIFO Control Register
#define UART_TLR_OFF			0x1C	// Trigger Level Register
#define UART_SCR_OFF			0x40	// Supplementary Control Register

/*
 * Bits for the UART FIFO settings
 */
#define UART_LCR_MODE_B			0xBF
#define UART_LCR_ENHANCED_EN	(1 << 4)
#define UART_LCR_MODE_A 		0x80
#define UART_MCR_TCR_TLR		(1 << 6)
#define UART_SCR_RX_TRIG_GRANU  (1 << 7)
#define UART_SCR_TX_TRIG_GRANU  (1 << 6)
#define UART_SCR_DMA_MODE_0		(1 << 1)
#define UART_SCR_DMA_MODE_1		(1 << 2)
#define UART_SCR_DMA_MODE_CTL	0x01

/*
 * Forward declaration
 */
static unsigned int getBaseAddressOfUART(uart_t);

void UARTSoftwareReset(uart_t uart)
{
	unsigned int baseAddress = getBaseAddressOfUART(uart);

	// 1. Initiate a software reset
	HWREG_SET(baseAddress + UART_SYSC_OFF, UART_SYSC_SOFT_RESET);

	// 2. Wait for the end of the reset operation
	while(HWREG_CHECK(baseAddress + UART_SYSS_OFF, UART_SYSS_SOFT_RESET));
}

//TODO: add some logic to decide write/read, ...
void UARTFIFOSettings(uart_t uart)
{
	unsigned int baseAddress = getBaseAddressOfUART(uart);

	// 1. Switch to register configuration mode B to access the UARTi.UART_EFR register
	// Save the current UARTi.UART_LCR register value.
	unsigned int lcrValue = HWREG(baseAddress + UART_LCR_OFF);
	// Set the UARTi.UART_LCR register value to 0x00BF.
	HWREG_SET(baseAddress + UART_LCR_OFF, UART_LCR_MODE_B);


	// 2. Enable register submode TCR_TLR to access the UARTi.UART_TLR register
	// Save the UARTi.UART_EFR[4] ENHANCED_EN value.
	unsigned int efrValue = HWREG(baseAddress + UART_LCR_EFR_OFF) & UART_LCR_ENHANCED_EN;
	// Set the UARTi.UART_EFR[4] ENHANCED_EN bit to 1.
	HWREG_SET(baseAddress + UART_LCR_EFR_OFF, UART_LCR_ENHANCED_EN);


	// 3. Switch to register configuration mode A to access the UARTi.UART_MCR register
	// Set the UARTi.UART_LCR register value to 0x0080.
	HWREG_SET(baseAddress + UART_LCR_OFF, UART_LCR_MODE_A);


	// 4. Enable register submode TCR_TLR to access the UARTi.UART_TLR register
	// Save the UARTi.UART_MCR[6] TCR_TLR value.
	unsigned int tcrTlrValue = HWREG(baseAddress + UART_MCR_OFF) & UART_MCR_TCR_TLR;
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
	HWREG_WRITE(baseAddress + UART_LCR_OFF, UART_MCR_TCR_TLR);
}

void UARTSettings(uart_t uart) {

}

/*
 * Helper methods
 */
static unsigned int getBaseAddressOfUART(uart_t uart)
{
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



