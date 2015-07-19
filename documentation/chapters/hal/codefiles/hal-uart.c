extern int UARTHalEnable(uartPins_t uartPins);
extern int UARTHalDisable(uartPins_t uartPins);
extern int UARTHalSoftwareReset(uartPins_t uartPins);
extern int UARTHalFifoSettings(uartPins_t uartPins);
extern int UARTHalSettings(uartPins_t uartPins, configuration_t* config);
extern int UARTHalFifoWrite(uartPins_t uartPins, uint8_t* msg);
extern int UARTHalFifoRead(uartPins_t uartPins, uint8_t* msg);
extern boolean_t UARTHalIsFifoFull(uartPins_t uartPins);
extern boolean_t UARTHalIsCharAvailable(uartPins_t uartPins);