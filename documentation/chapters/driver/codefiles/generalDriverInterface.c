typedef struct {
    int (*init)(uint16_t pin);
    int (*open)(uint16_t pin);
    int (*close)(uint16_t pin);
    int (*read)(uint16_t pin, char* buf, uint16_t length);
    int (*write)(uint16_t pin, char* buf, uint16_t length);
    int (*ioctl)(uint16_t pin, uint16_t cmd, uint8_t mode, char* buf, uint16_t length);
} driver_t;
