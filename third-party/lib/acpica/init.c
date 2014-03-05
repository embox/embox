/**
 * @file
 * @brief Initialization of the ACPICA module.
 *
 * @date 05.03.2014
 * @author Roman Kurbatov
 */

#include <embox/unit.h>
#include <kernel/printk.h>
#include <acpica/acpi.h>

EMBOX_UNIT_INIT(init);

static int init(void) {
	ACPI_STATUS status;

	status = AcpiInitializeSubsystem();
	if (ACPI_FAILURE(status)) {
		printk("ERROR: Unable to initialize the ACPICA subsystem");
		goto error;
	}

	status = AcpiInitializeTables(NULL, 16, FALSE);
	if (ACPI_FAILURE(status)) {
		printk("ERROR: Unable to initialize the ACPICA table manager");
		goto error;
	}

	status = AcpiLoadTables();
	if (ACPI_FAILURE(status)) {
		printk("ERROR: Unable to load ACPI tables");
		goto error;
	}

	status = AcpiEnableSubsystem(ACPI_FULL_INITIALIZATION);
	if (ACPI_FAILURE(status)) {
		printk("ERROR: Unable to complete the ACPICA subsystem initialization");
		goto error;
	}

	status = AcpiInitializeObjects(ACPI_FULL_INITIALIZATION);
	if (ACPI_FAILURE(status)) {
		printk("ERROR: Unable to initialize objects within the ACPI namespace");
		goto error;
	}

	return 0;

	error:
	AcpiTerminate();
	return -1;
}
