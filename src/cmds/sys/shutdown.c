/**
 * @file
 *
 * @date Oct 29, 2012
 * @author: Anton Bondarev
 * @author: Roman Kurbatov
 *          - Turn off the system with ACPICA
 */

#include <drivers/acpi/acpi.h>
#include <embox/cmd.h>
#include <hal/arch.h>

EMBOX_CMD(exec);

static int exec(int argc, char **argv) {
	//arch_shutdown(0);
	AcpiInitializeSubsystem();
	AcpiInitializeTables(NULL, 16, FALSE);
	AcpiLoadTables();
	AcpiEnableSubsystem(ACPI_FULL_INITIALIZATION);
	AcpiInitializeObjects(ACPI_FULL_INITIALIZATION);
	AcpiEnterSleepStatePrep(ACPI_STATE_S5);
	AcpiEnterSleepState(ACPI_STATE_S5);
	return 0;
}
