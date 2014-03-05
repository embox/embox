/**
 * @file
 *
 * @date 10.11.10
 * @author Nikolay Korotky
 *         Roman Kurbatov
 *         - ACPI power off
 */

#include <asm/io.h>
#include <asm/traps.h>
#include <hal/arch.h>
#include <hal/ipl.h>
#include <kernel/printk.h>
#include <acpica/acpi.h>

void arch_init(void) {
	//gdt_init();
	//idt_init();
}

void arch_idle(void) {
	__asm__ __volatile__("hlt");
}

#if 1
//http://mjg59.dreamwidth.org/3561.html
//http://stackoverflow.com/questions/3145569/how-to-power-down-the-computer-from-a-freestanding-environment
void arch_reset_kbd(void) {
	while (in8(0x64) & 0x2);
	out8(0x60, 0x64);
	while (in8(0x64) & 0x2);
	out8(0x4, 0x60);
	while (in8(0x64) & 0x2);
	out8(0xfe, 0x64);
}
#endif



void __attribute__ ((noreturn)) arch_shutdown(arch_shutdown_mode_t mode) {
	ACPI_STATUS status;

	status = AcpiEnterSleepStatePrep(ACPI_STATE_S5);
	if (ACPI_FAILURE(status)) {
		printk("ERROR: Unable to prepare to enter the soft off system state");
		goto error;
	}

	status = AcpiEnterSleepState(ACPI_STATE_S5);
	if (ACPI_FAILURE(status)) {
		printk("ERROR: Unable to enter the soft off system state");
	}

	error:

	while (1) {}
}

