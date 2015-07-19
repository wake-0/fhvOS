static driver_t* drivers[MAX_DRIVER];

void DriverManagerInit()
{
	// LED Driver
	driver_t* led = malloc(sizeof(driver_t));
	led->init = &LEDInit;
	led->open = &LEDOpen;
	led->close = &LEDClose;
	led->read = &LEDRead;
	led->write = &LEDWrite;
	led->ioctl = &LEDIoctl;
	drivers[DRIVER_ID_LED] = led;
}

driver_t* DriverManagerGetDriver(driver_id_t driver_id)
{
	return drivers[driver_id];
}

void DriverManagerDestruct()
{
	int i;
	for (i = 0; i < MAX_DRIVER; i++) {
		if (drivers[i] != NULL) {
			free(drivers[i]);
			drivers[i] = NULL;
		}
	}
}
