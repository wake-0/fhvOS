#define DRIVER_ID_LED		123

extern void DriverManagerInit(void);
extern driver_t* DriverManagerGetDriver(driver_id_t driver_id);
extern void DriverManagerDestruct(void);