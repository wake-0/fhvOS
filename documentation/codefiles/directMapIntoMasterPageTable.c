static void mmuMapDirectRegionToKernelMasterPageTable(memoryRegionPointer_t memoryRegion, pageTablePointer_t table)
{
	unsigned int physicalAddress;
	firstLevelDescriptor_t pageTableEntry;

	for(physicalAddress = memoryRegion->startAddress; physicalAddress < memoryRegion->endAddress; physicalAddress += 0x100000)
	{
		pageTableEntry.sectionBaseAddress 	= physicalAddress & UPPER_12_BITS_MASK;
		pageTableEntry.descriptorType 		= DESCRIPTOR_TYPE_SECTION;
		pageTableEntry.cachedBuffered 		= WRITE_BACK;
		pageTableEntry.accessPermission 	= AP_FULL_ACCESS;
		pageTableEntry.domain 				= DOMAIN_MANAGER_ACCESS;

		uint32_t tableOffset = mmuGetTableIndex(physicalAddress, INDEX_OF_L1_PAGE_TABLE, TTBR1);

		// see Format of first-level Descriptor on p. B3-1335 in ARM Architecture Reference Manual ARMv7 edition
		uint32_t *firstLevelDescriptorAddress = table + (tableOffset << 2)/sizeof(uint32_t);
		*firstLevelDescriptorAddress = mmuCreateL1PageTableEntry(pageTableEntry);
	}
}