/**
 * @file
 *
 * @date 12.03.2015
 *  @author: Anton Bondarev
 */

#include <kernel/printk.h>

#include <acpi.h>


void acpi_shutdown(void) {
	ACPI_STATUS status;

	status = AcpiEnterSleepStatePrep(ACPI_STATE_S5);
	if (ACPI_FAILURE(status)) {
		printk("ERROR: Unable to prepare to enter the soft off system state\n");
		break;
	}

	status = AcpiEnterSleepState(ACPI_STATE_S5);
	if (ACPI_FAILURE(status)) {
		printk("ERROR: Unable to enter the soft off system state\n");
	}
}

void acpi_reset(void) {
	AcpiReset();
}
