/*
 * uartDriver.c
 *
 *  Created on: 20.03.2015
 *      Author: Kevin
 */
#include "driver_uart.h"

#include "../beaglebone/hw_uart.h"

static configuration_t createDmxConfig(void);
static configuration_t createSlowConfig(void);
static configuration_t createDefaultConfig(void);
static void init(configuration_t* config, uartPins_t* pins);

int UARTDriverInit(uint16_t id) {
	if (id > BOARD_UART_COUNT - 1) { return DRIVER_ERROR; }

	//Setup the uart
	uartPins_t pins = GetUARTPins(id);
	UARTHalEnable(pins);

	configuration_t config;
	// TODO: move this special stuff to Ioctl
	if (id == 1) {
		config = createDmxConfig();
	} else {
		config = createDefaultConfig();
	}

	init(&config, &pins);
	return DRIVER_OK;
}

int UARTDriverOpen(uint16_t id) {
	return DRIVER_OK;
}

int UARTDriverClose(uint16_t id) {
	return DRIVER_OK;
}

int UARTDriverWrite(uint16_t id, char* buf, uint16_t len) {
	if (id > BOARD_UART_COUNT - 1) { return DRIVER_ERROR; }
	uartPins_t pins = GetUARTPins(id);

	// Write chunks
	int i;
	uint8_t msg;
	for (i = 0; i < len; i++) {
		msg = (uint8_t)*(buf+i);
		UARTHalFifoWrite(pins, &msg);
		while (UARTHalIsFifoFull(pins) == true)
		{
			// Busy waiting until fifo is not full
		}
	}

	return DRIVER_OK;
}

int UARTDriverRead(uint16_t id, char* buf, uint16_t len) {
	if (id > BOARD_UART_COUNT - 1) { return DRIVER_ERROR; }
	uartPins_t pins = GetUARTPins(id);

	// Read chunks
	int i;
	for (i = 0; i < len; i++) {
		while (UARTHalIsCharAvailable(pins) == FALSE)
		{
			// Wait until a char is available
		}
		UARTHalFifoRead(pins, (uint8_t*)(buf+i));
	}

	return DRIVER_OK;
}

int UARTDriverIoctl(uint16_t id, uint16_t cmd, uint8_t mode, char* buf, uint16_t len) {
	if (id > BOARD_UART_COUNT - 1) { return DRIVER_ERROR; }

	uartPins_t pins = GetUARTPins(id);
	configuration_t config;

	if (cmd == 1) {
		config = createDmxConfig();
	} else if (cmd == 2) {
		config = createSlowConfig();
	}

	// call init functions
	init(&config, &pins);

	return DRIVER_OK;
}

static void init(configuration_t* config, uartPins_t* pins) {
	UARTHalSoftwareReset(*pins);
	UARTHalFifoSettings(*pins);
	UARTHalSettings(*pins, config);
}

// TODO: this stuff should be handled by the input parameters from the Ioctl
static configuration_t createDmxConfig(void) {
	configuration_t config;
	config.baudRate = UART_BAUDRATE_250000;
	config.parity =  UART_PARITY_NONE;
	config.charLength = UART_CHARLENGTH_8;
	config.stopBit = UART_STOPBIT_2;
	return config;
}

static configuration_t createSlowConfig(void) {
	configuration_t config;
	config.baudRate = UART_BAUDRATE_1;
	config.parity =  UART_PARITY_NONE;
	config.charLength = UART_CHARLENGTH_8;
	config.stopBit = UART_STOPBIT_1;
	return config;
}

static configuration_t createDefaultConfig(void) {
	configuration_t config;
	config.baudRate = UART_BAUDRATE_115200;
	config.parity =  UART_PARITY_NONE;
	config.charLength = UART_CHARLENGTH_8;
	config.stopBit = UART_STOPBIT_1;
	return config;
}
