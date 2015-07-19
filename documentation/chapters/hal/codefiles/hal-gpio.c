extern void GPIOEnable(uint16_t pin);
extern void GPIODisable(uint16_t pin);
extern void GPIOReset(uint16_t pin);
extern void GPIOSetMux(uint16_t pin, mux_mode_t mux);
extern void GPIOSetPinDirection(uint16_t pin, pin_direction_t dir);
extern void GPIOSetPinValue(uint16_t pin, pin_value_t value);
extern pin_value_t 	GPIOGetPinValue(uint16_t pin);