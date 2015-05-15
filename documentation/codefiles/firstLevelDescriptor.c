typedef struct
{
	unsigned int sectionBaseAddress;
	unsigned int accessPermission : 2;
	unsigned int domain : 4;
	unsigned int cachedBuffered : 2;
	unsigned int descriptorType : 2;

} firstLevelDescriptor_t;