/**
 * @file
 * @brief Initialization of the ACPICA module.
 *
 * @date 05.03.2014
 * @author Roman Kurbatov
 */

#include <embox/unit.h>
#include <kernel/printk.h>
#include <acpi.h>

EMBOX_UNIT_INIT(init);

static int init(void) {
	ACPI_STATUS status;

	status = AcpiInitializeSubsystem();
	if (ACPI_FAILURE(status)) {
		printk("ERROR: Unable to initialize the ACPICA subsystem\n");
		goto error;
	}

	status = AcpiInitializeTables(NULL, 16, FALSE);
	if (ACPI_FAILURE(status)) {
		printk("ERROR: Unable to initialize the ACPICA table manager\n");
		goto error;
	}

	status = AcpiLoadTables();
	if (ACPI_FAILURE(status)) {
		printk("ERROR: Unable to load ACPI tables\n");
		goto error;
	}

	status = AcpiEnableSubsystem(ACPI_FULL_INITIALIZATION);
	if (ACPI_FAILURE(status)) {
		printk("ERROR: Unable to complete the ACPICA subsystem initialization\n");
		goto error;
	}

	status = AcpiInitializeObjects(ACPI_FULL_INITIALIZATION);
	if (ACPI_FAILURE(status)) {
		printk("ERROR: Unable to initialize objects within the ACPI namespace\n");
		goto error;
	}

	return 0;

	error:
	AcpiTerminate();
	return -1;
}
