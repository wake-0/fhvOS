/*
 * hw_uart.h
 *
 *  Created on: 19.03.2015
 *      Author: Kevin
 */

#ifndef HAL_AM335X_HW_UART_H_
#define HAL_AM335X_HW_UART_H_

#define UART_DEFAULT_CLK	(48000000)

/*
 * UART base addresses
 */
#define UART0_BASE_ADR 		(0x44E09000)
#define UART1_BASE_ADR 		(0x48022000)
#define UART2_BASE_ADR 		(0x48024000)
#define UART3_BASE_ADR 		(0x481A6000)
#define UART4_BASE_ADR 		(0x481A8000)
#define UART5_BASE_ADR 		(0x481AA000)

/*
 * Offsets
 */
#define UART_SYSC_OFF 			(0x054)	// System Configuration Register
#define UART_SYSS_OFF 			(0x058)	// System Status Register
#define UART_LCR_OFF			(0x00C)	// Line Control Register
#define UART_LCR_EFR_OFF		(0x008)	// Enhanced Feature Register
#define UART_MCR_OFF			(0x010)	// Modem Control Register
#define UART_FCR_OFF			(0x008)	// FIFO Control Register
#define UART_TLR_OFF			(0x01C)	// Trigger Level Register
#define UART_SCR_OFF			(0x040)	// Supplementary Control Register
#define UART_SSR_OFF			(0x044)	// Supplementary Status Register
#define UART_MDR1_OFF			(0x020)	// Mode Definition Register 1
#define UART_IER_OFF			(0x004)	// Interrupt Enable Register
#define UART_DLL_OFF			(0x000)	// Divisor Latches Low Register
#define UART_DLH_OFF			(0x004)	// Divisor Latches High Register
#define UART_THR_OFF			(0x000) // Transmit Holding Register
#define UART_RHR_OFF			(0x000) // Receive Holding Register

/*
 * Bits for the UART FIFO settings
 */
#define UART_LCR_MODE_A 		(0x080)
#define UART_LCR_MODE_B			(0x0BF)
#define UART_LCR_ENHANCED_EN	(1 << 4)

#define UART_MCR_TCR_TLR		(1 << 6)

#define UART_SCR_TX_TRIG_GRANU  (1 << 6)
#define UART_SCR_RX_TRIG_GRANU  (1 << 7)
#define UART_SCR_DMA_MODE_CTL	(1 << 0)
#define UART_SCR_DMA_MODE_0		(1 << 1)
#define UART_SCR_DMA_MODE_1		(1 << 2)

#define UART_SSR_TXFIFOFULL		(1 << 0)

/*
 * Bits for the UART settings
 */
#define UART_MODE_DISABLE_UART			(0x07)
#define UART_LCR_MODE_OPERATIONAL		(0x00)
#define UART_IER_RHR_IT					(0x01)
#define UART_PARITY_EN					(1 << 3)
#define UART_PARITY_TYPE_2				(1 << 5)
#define UART_PARITY_TYPE_1				(1 << 4)
#define UART_16X_MODE					(0x00)
#define UART_DISABLE_MDOE				(0x07)

#endif /* HAL_AM335X_HW_UART_H_ */
