typedef struct region
{
    unsigned int startAddress;
    unsigned int endAddress;
    unsigned int pageSize;
    unsigned int accessPermission;
    unsigned int cacheBufferAttributes;
    unsigned int reservedPages;
    pageStatusPointer_t pageStatus;
} memoryRegion_t;