int MMUInit()
{
	MemoryManagerInit();

	MMUDisable();

	// reserve direct mapped regions so no accidently reserving of pages can occur
	MemoryManagerReserveAllDirectMappedRegions();

	// master page table for kernel region must be created statically and before MMU is enabled
	mmuCreateMasterPageTable(KERNEL_START_ADDRESS, KERNEL_END_ADDRESS);
	mmuSetKernelMasterPageTable(kernelMasterPageTable);
	mmuSetProcessPageTable(kernelMasterPageTable);

	// MMU Settings
	mmuSetTranslationTableSelectionBoundary(BOUNDARY_AT_QUARTER_OF_MEMORY);
	mmuSetDomainToFullAccess();

	MMUEnable();

	return MMU_OK;
}