typedef struct {
    int (*init)(uint16_t payload);
    int (*open)(uint16_t payload);
    int (*close)(uint16_t payload);
    int (*read)(uint16_t payload, char* buf, uint16_t length);
    int (*write)(uint16_t payload, char* buf, uint16_t length);
    int (*ioctl)(uint16_t payload, uint16_t cmd, uint8_t mode, char* buf, uint16_t length);
} driver_t;
