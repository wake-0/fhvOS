[...]
int LEDWrite (uint16_t id, char* buf, uint16_t len)
{
	if (id > (BOARD_LED_COUNT - 1)) return DRIVER_ERROR;

	if (len != 1) return DRIVER_ERROR;

	switch(buf[0])
	{
		case '1':
			GPIOSetPinValue(BOARD_LED(id), PIN_VALUE_HIGH);
			break;
		case '0':
			GPIOSetPinValue(BOARD_LED(id), PIN_VALUE_LOW);
			break;
		default:
			return DRIVER_ERROR;
	}
	return DRIVER_OK;
}
[...]