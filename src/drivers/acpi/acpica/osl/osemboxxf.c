/**
 * @file
 * @brief Embox OSL for ACPICA.
 *
 * @date 17.12.13
 * @author Roman Kurbatov
 */

#include <drivers/acpi/acpi.h>

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
		ACPI_STRING                 *NewVal) {
    if (!InitVal || !NewVal) {
        return AE_BAD_PARAMETER;
    }

    *NewVal = NULL;

    return AE_OK;
}

ACPI_STATUS AcpiOsTableOverride(
		ACPI_TABLE_HEADER       *ExistingTable,
		ACPI_TABLE_HEADER       **NewTable) {
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
