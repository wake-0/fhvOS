typedef struct
{
	unsigned int pageBaseAddress;
	unsigned int accessPermission : 2;
	unsigned int cachedBuffered : 2;
	unsigned int descriptorType : 2;
} secondLevelDescriptor_t;