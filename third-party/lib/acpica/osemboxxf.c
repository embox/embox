/**
 * @file
 * @brief Embox OSL for ACPICA.
 *
 * @date 17.12.13
 * @author Roman Kurbatov
 */

/*
 * Notes:
 *
 * 1. Now the only purpose of using ACPICA is to turn the system off. So many
 * functions not required for it are not implemented here.
 *
 * 2. We assume that there is no virtual memory in Embox.
 *
 */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include <asm/io.h>
#include <acpi.h>
#include <kernel/printk.h>
#include <kernel/thread.h>
#include <kernel/time/ktime.h>
#include <sys/mman.h>

#ifdef DEBUG
#define PRINTD(msg) printk("ACPICA OSL DEBUG: " msg "\n")
#else
#define PRINTD(msg)
#endif

ACPI_STATUS AcpiOsInitialize(void) {
	PRINTD("AcpiOsInitialize() called");

	return AE_OK;
}

ACPI_STATUS AcpiOsTerminate(void) {
	PRINTD("AcpiOsTerminate() called");

	return AE_OK;
}

ACPI_PHYSICAL_ADDRESS AcpiOsGetRootPointer(void) {
	ACPI_PHYSICAL_ADDRESS address = 0;

	PRINTD("AcpiOsGetRootPointer() called");

	AcpiFindRootPointer(&address);
	return address;
}

ACPI_STATUS AcpiOsPredefinedOverride(
		const ACPI_PREDEFINED_NAMES *InitVal,
		ACPI_STRING *NewVal) {
    if (!InitVal || !NewVal) {
        return AE_BAD_PARAMETER;
    }

	PRINTD("AcpiOsPredefinedOverride() called");

    *NewVal = NULL;

    return AE_OK;
}

ACPI_STATUS AcpiOsTableOverride(
		ACPI_TABLE_HEADER *ExistingTable,
		ACPI_TABLE_HEADER **NewTable) {
    if (!ExistingTable || !NewTable) {
        return AE_BAD_PARAMETER;
    }

	PRINTD("AcpiOsTableOverride() called");

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

	PRINTD("AcpiOsPhysicalTableOverride() called");

	*NewAddress = 0;
	*NewTableLength = 0;

	return AE_OK;
}

void *AcpiOsMapMemory(ACPI_PHYSICAL_ADDRESS Where, ACPI_SIZE Length) {
	PRINTD("AcpiOsMapMemory() called");
	if (MAP_FAILED == mmap_device_memory((void *) Where,
                Length,
                PROT_READ | PROT_WRITE | PROT_NOCACHE,
                MAP_FIXED,
                (uintptr_t)Where)) {
		return NULL;
	}
	return ACPI_TO_POINTER(Where);
}

void AcpiOsUnmapMemory(void *LogicalAddress, ACPI_SIZE Size) {
	PRINTD("AcpiOsUnmapMemory() called");
}

ACPI_STATUS AcpiOsGetPhysicalAddress(
		void *LogicalAddress,
		ACPI_PHYSICAL_ADDRESS *PhysicalAddress) {
	PRINTD("AcpiOsGetPhysicalAddress() called");

	if (!LogicalAddress || !PhysicalAddress) {
		return AE_BAD_PARAMETER;
	}

	*PhysicalAddress = ACPI_PTR_TO_PHYSADDR(LogicalAddress);

	return AE_OK;
}

void *AcpiOsAllocate(ACPI_SIZE Size) {
	PRINTD("AcpiOsAllocate() called");

	return malloc(Size);
}

void AcpiOsFree(void *Memory) {
	PRINTD("AcpiOsFree() called");

	free(Memory);
}

BOOLEAN AcpiOsReadable(void *Pointer, ACPI_SIZE Length) {
	PRINTD("AcpiOsReadable() called");

	return TRUE;
}

BOOLEAN AcpiOsWritable(void *Pointer, ACPI_SIZE Length) {
	PRINTD("AcpiOsWritable() called");

	return TRUE;
}

ACPI_THREAD_ID AcpiOsGetThreadId(void) {
	PRINTD("AcpiOsGetThreadId() called");

	return thread_self()->id;
}

ACPI_STATUS AcpiOsExecute(
		ACPI_EXECUTE_TYPE Type,
		ACPI_OSD_EXEC_CALLBACK Function,
		void *Context) {
	PRINTD("Not implemented AcpiOsExecute() called");

	/* Not implemented. */

	return AE_OK;
}

void AcpiOsSleep(UINT64 Milliseconds) {
	PRINTD("AcpiOsSleep() called");

	ksleep(Milliseconds);
}

/******************************************************************************
 *
 * FUNCTION:    AcpiOsEnterSleep
 *
 * PARAMETERS:  SleepState          - Which sleep state to enter
 *              RegaValue           - Register A value
 *              RegbValue           - Register B value
 *
 * RETURN:      Status
 *
 * DESCRIPTION: A hook before writing sleep registers to enter the sleep
 *              state. Return AE_CTRL_TERMINATE to skip further sleep register
 *              writes.
 *
 *****************************************************************************/

ACPI_STATUS
AcpiOsEnterSleep (
    UINT8                   SleepState,
    UINT32                  RegaValue,
    UINT32                  RegbValue)
{

    return (AE_OK);
}

void AcpiOsStall(UINT32 Microseconds) {
	PRINTD("Not implemented AcpiOsStall() called");

	/* Not implemented. */
}

void AcpiOsWaitEventsComplete(void) {
	PRINTD("Not implemented AcpiOsWaitEventsComplete() called");

	/* Not implemented. */
}

ACPI_STATUS AcpiOsCreateSemaphore(
		UINT32 MaxUnits,
		UINT32 InitialUnits,
		ACPI_SEMAPHORE *OutHandle) {
	PRINTD("Not implemented AcpiOsCreateSemaphore() called");

	/* Not implemented. */

	return AE_OK;
}

ACPI_STATUS AcpiOsDeleteSemaphore(ACPI_SEMAPHORE Handle) {
	PRINTD("Not implemented AcpiOsDeleteSemaphore() called");

	/* Not implemented. */

	return AE_OK;
}

ACPI_STATUS AcpiOsWaitSemaphore(
		ACPI_SEMAPHORE Handle,
		UINT32 Units,
		UINT16 Timeout) {
	PRINTD("Not implemented AcpiOsWaitSemaphore() called");

	/* Not implemented. */

	return AE_OK;
}

ACPI_STATUS AcpiOsSignalSemaphore(ACPI_SEMAPHORE Handle, UINT32 Units) {
	PRINTD("Not implemented AcpiOsSignalSemaphore() called");

	/* Not implemented. */

	return AE_OK;
}

ACPI_STATUS AcpiOsCreateLock(ACPI_SPINLOCK *OutHandle) {
	PRINTD("Not implemented AcpiOsCreateLock() called");

	/* Not implemented. */

	return AE_OK;
}

void AcpiOsDeleteLock(ACPI_SPINLOCK Handle) {
	PRINTD("Not implemented AcpiOsDeleteLock() called");

	/* Not implemented. */
}

ACPI_CPU_FLAGS AcpiOsAcquireLock(ACPI_SPINLOCK Handle) {
	PRINTD("Not implemented AcpiOsAcquireLock() called");

	/* Not implemented. */

	return AE_OK;
}

void AcpiOsReleaseLock(ACPI_SPINLOCK Handle, ACPI_CPU_FLAGS Flags) {
	PRINTD("Not implemented AcpiOsReleaseLock() called");

	/* Not implemented. */
}

ACPI_STATUS AcpiOsInstallInterruptHandler(
		UINT32 InterruptNumber,
		ACPI_OSD_HANDLER ServiceRoutine,
		void *Context) {
	PRINTD("Not implemented AcpiOsInstallInterruptHandler() called");

	/* Not implemented. */

	return AE_OK;
}

ACPI_STATUS AcpiOsRemoveInterruptHandler(
		UINT32 InterruptNumber,
		ACPI_OSD_HANDLER ServiceRoutine) {
	PRINTD("Not implemented AcpiOsRemoveInterruptHandler() called");

	/* Not implemented. */

	return AE_OK;
}

ACPI_STATUS AcpiOsReadPort(
		ACPI_IO_ADDRESS Address,
		UINT32 *Value,
		UINT32 Width) {
	PRINTD("AcpiOsReadPort() called");

	if (!Value) {
		return AE_BAD_PARAMETER;
	}

	switch (Width) {
		case 8: {
			*Value = in8(Address);
			break;
		}
		case 16: {
			*Value = in16(Address);
			break;
		}
		case 32: {
			*Value = in32(Address);
			break;
		}
		default: {
			return AE_BAD_PARAMETER;
		}
	}

	return AE_OK;
}

ACPI_STATUS AcpiOsWritePort(
		ACPI_IO_ADDRESS Address,
		UINT32 Value,
		UINT32 Width) {
	PRINTD("AcpiOsWritePort() called");

	switch (Width) {
		case 8: {
			out8((uint8_t) Value, Address);
			break;
		}
		case 16: {
			out16((uint16_t) Value, Address);
			break;
		}
		case 32: {
			out32(Value, Address);
			break;
		}
		default: {
			return AE_BAD_PARAMETER;
		}
	}

	return AE_OK;
}

ACPI_STATUS AcpiOsReadMemory(
		ACPI_PHYSICAL_ADDRESS Address,
		UINT64 *Value,
		UINT32 Width) {
	PRINTD("Not implemented AcpiOsReadMemory() called");

	/* Not implemented. */

	return AE_OK;
}

ACPI_STATUS AcpiOsWriteMemory(
		ACPI_PHYSICAL_ADDRESS Address,
		UINT64 Value,
		UINT32 Width) {
	PRINTD("Not implemented AcpiOsWriteMemory() called");

	/* Not implemented. */

	return AE_OK;
}

ACPI_STATUS AcpiOsReadPciConfiguration(
		ACPI_PCI_ID *PciId,
		UINT32 Reg,
		UINT64 *Value,
		UINT32 Width) {
	PRINTD("Not implemented AcpiOsReadPciConfiguration() called");

	/* Not implemented. */

	return AE_OK;
}

ACPI_STATUS AcpiOsWritePciConfiguration(
		ACPI_PCI_ID *PciId,
		UINT32 Reg,
		UINT64 Value,
		UINT32 Width) {
	PRINTD("Not implemented AcpiOsWritePciConfiguration() called");

	/* Not implemented. */

	return AE_OK;
}

void ACPI_INTERNAL_VAR_XFACE AcpiOsPrintf(const char *Format, ...) {
	va_list args;

	PRINTD("AcpiOsPrintf() called");

	va_start(args, Format);
	AcpiOsVprintf(Format, args);
	va_end(args);
}

void AcpiOsVprintf(const char *Format, va_list Args) {
	PRINTD("AcpiOsVprintf() called");

	vprintf(Format, Args);
}

void AcpiOsRedirectOutput(void *Destination) {
	PRINTD("Not implemented AcpiOsRedirectOutput() called");

	/* Not implemented. */
}

UINT64 AcpiOsGetTimer(void) {
	PRINTD("Not implemented AcpiOsGetTimer() called");

	/* Not implemented. */

	return 0;
}

ACPI_STATUS AcpiOsSignal(UINT32 Function, void *Info) {
	PRINTD("Not implemented AcpiOsSignal() called");

	/* Not implemented. */

	return AE_OK;
}
