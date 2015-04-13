/**
 * @file
 *
 * @date 12.03.2015
 *  @author: Anton Bondarev
 */
#include <kernel/printk.h>

void acpi_shutdown(void) {
	printk("No shutdown implemented please enable "
			"'embox.arch.x86.kernel.acpi_shutdown'\n");
}

void acpi_reset(void) {
}
