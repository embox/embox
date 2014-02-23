/**
 * @file
 * @brief Embox OSL for ACPICA.
 *
 * @date 17.12.13
 * @author Roman Kurbatov
 */

#include <stdlib.h>

#include <drivers/acpi/acpi.h>
#include <kernel/thread.h>
#include <kernel/thread/sync/semaphore.h>

ACPI_STATUS AcpiOsInitialize(void) {
	return AE_OK;
}

ACPI_STATUS AcpiOsTerminate(void) {
	return AE_OK;
}

ACPI_PHYSICAL_ADDRESS AcpiOsGetRootPointer(void) {
	ACPI_PHYSICAL_ADDRESS address = 0;
	AcpiFindRootPointer(&address);
	return address;
}

ACPI_STATUS AcpiOsPredefinedOverride(
		const ACPI_PREDEFINED_NAMES *InitVal,
		ACPI_STRING *NewVal) {
    if (!InitVal || !NewVal) {
        return AE_BAD_PARAMETER;
    }

    *NewVal = NULL;

    return AE_OK;
}

ACPI_STATUS AcpiOsTableOverride(
		ACPI_TABLE_HEADER *ExistingTable,
		ACPI_TABLE_HEADER **NewTable) {
    if (!ExistingTable || !NewTable) {
        return AE_BAD_PARAMETER;
    }

    *NewTable = NULL;

    return AE_OK;
}


ACPI_STATUS AcpiOsPhysicalTableOverride(
		ACPI_TABLE_HEADER       *ExistingTable,
		ACPI_PHYSICAL_ADDRESS   *NewAddress,
		UINT32                  *NewTableLength) {
	if (!ExistingTable || !NewAddress || !NewTableLength) {
		return AE_BAD_PARAMETER;
	}

	*NewAddress = 0;
	*NewTableLength = 0;

	return AE_OK;
}

/*
 * We assume that there is no virtual memory in Embox.
 */

void *AcpiOsMapMemory(ACPI_PHYSICAL_ADDRESS Where, ACPI_SIZE) {
	return ACPI_PHYSADDR_TO_PTR(Where);
}

void AcpiOsUnmapMemory(void *, ACPI_SIZE) {
}

ACPI_STATUS AcpiOsGetPhysicalAddress(
		void *LogicalAddress,
		ACPI_PHYSICAL_ADDRESS *PhysicalAddress) {
	if (!LogicalAddress || !PhysicalAddress) {
		return AE_BAD_PARAMETER;
	}

	*PhysicalAddress = ACPI_PTR_TO_PHYSADDR(LogicalAddress);

	return AE_OK;
}

void *AcpiOsAllocate(ACPI_SIZE Size) {
	return malloc(Size);
}

void AcpiOsFree(void *Memory) {
	free(Memory);
}

BOOLEAN AcpiOsReadable(void *, ACPI_SIZE) {
	return TRUE;
}

BOOLEAN AcpiOsWritable(void *, ACPI_SIZE) {
	return TRUE;
}

ACPI_THREAD_ID AcpiOsGetThreadId(void) {
	return thread_self()->id;
}

ACPI_STATUS AcpiOsExecute(
		ACPI_EXECUTE_TYPE,
		ACPI_OSD_EXEC_CALLBACK Function,
		void *Context) {
	if (!Function) {
		return AE_BAD_PARAMETER;
	}

	thread_create(THREAD_FLAG_DETACHED, Function, Context);

	return AE_OK;
}

void AcpiOsSleep(UINT64 Milliseconds) {
	ksleep(Milliseconds);
}

void AcpiOsStall(UINT32 Microseconds) {
	// TODO
}

void AcpiOsWaitEventsComplete(void) {
	// TODO
}

ACPI_STATUS AcpiOsCreateSemaphore(
		UINT32,
		UINT32 InitialUnits,
		ACPI_SEMAPHORE *OutHandle) {
	if (!OutHandle) {
		return AE_BAD_PARAMETER;
	}

	*OutHandle = malloc(sizeof(struct sem));

	if (!*OutHandle) {
		return AE_NO_MEMORY;
	}

	semaphore_init(*OutHandle, InitialUnits);
}

ACPI_STATUS AcpiOsDeleteSemaphore(ACPI_SEMAPHORE Handle) {
	if (!Handle) {
		return AE_BAD_PARAMETER;
	}

	free(Handle);

	return AE_OK;
}

ACPI_STATUS AcpiOsWaitSemaphore(
		ACPI_SEMAPHORE,
		UINT32,
		UINT16) {
	// TODO
	return AE_OK;
}

ACPI_STATUS AcpiOsSignalSemaphore(ACPI_SEMAPHORE, UINT32) {
	// TODO
	return AE_OK;
}

ACPI_STATUS AcpiOsCreateLock(ACPI_SPINLOCK *) {
	// TODO
	return AE_OK;
}

void AcpiOsDeleteLock (ACPI_SPINLOCK) {
	// TODO
}

ACPI_CPU_FLAGS AcpiOsAcquireLock(ACPI_SPINLOCK) {
	// TODO
	return AE_OK;
}

void AcpiOsReleaseLock(ACPI_SPINLOCK, ACPI_CPU_FLAGS Flags) {
	// TODO
}

ACPI_STATUS AcpiOsInstallInterruptHandler(UINT32, ACPI_OSD_HANDLER, void *) {
	// TODO
	return AE_OK;
}

ACPI_STATUS AcpiOsRemoveInterruptHandler(UINT32, ACPI_OSD_HANDLER) {
	// TODO
	return AE_OK;
}

ACPI_STATUS AcpiOsReadPort (ACPI_IO_ADDRESS, UINT32, UINT32) {
	// TODO
	return AE_OK;
}

ACPI_STATUS AcpiOsWritePort (ACPI_IO_ADDRESS, UINT32, UINT32) {
	// TODO
	return AE_OK;
}

ACPI_STATUS AcpiOsReadMemory(ACPI_PHYSICAL_ADDRESS, UINT64, UINT32) {
	// TODO
	return AE_OK;
}

ACPI_STATUS AcpiOsWriteMemory(ACPI_PHYSICAL_ADDRESS, UINT64, UINT32) {
	// TODO
	return AE_OK;
}

ACPI_STATUS
AcpiOsReadPciConfiguration (ACPI_PCI_ID *, UINT32, UINT64, UINT32) {
	// TODO
	return AE_OK;
}

ACPI_STATUS AcpiOsWritePciConfiguration(ACPI_PCI_ID *, UINT32, UINT64, UINT32) {
	// TODO
	return AE_OK;
}

void ACPI_INTERNAL_VAR_XFACE AcpiOsPrintf (const char *Format, ...) {
	// TODO
}

void AcpiOsVprintf(const char *, va_list) {
	// TODO
}

void AcpiOsRedirectOutput(void *) {
	// TODO
}

UINT64 AcpiOsGetTimer(void) {
	// TODO
	return 0;
}

ACPI_STATUS AcpiOsSignal(UINT32, void *) {
	// TODO
	return AE_OK;
}

ACPI_STATUS AcpiOsGetLine(char *, UINT32, UINT32 *) {
	// TODO
	return AE_OK;
}
