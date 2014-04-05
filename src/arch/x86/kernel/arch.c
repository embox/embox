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

extern void cpu_triple_reset(void);

void arch_init(void) {
	//gdt_init();
	//idt_init();
}

void arch_idle(void) {
	__asm__ __volatile__("hlt");
}

void arch_reset_kbd(void) {
	while (in8(0x64) & 0x2) {
	}
	out8(0x60, 0x64);

	while (in8(0x64) & 0x2) {
	}
	out8(0x4, 0x60);

	while (in8(0x64) & 0x2) {
	}
	out8(0xfe, 0x64);
}



void __attribute__ ((noreturn)) arch_shutdown(arch_shutdown_mode_t mode) {
	ACPI_STATUS status;

	switch (mode) {
	case ARCH_SHUTDOWN_MODE_HALT:
		status = AcpiEnterSleepStatePrep(ACPI_STATE_S5);
		if (ACPI_FAILURE(status)) {
			printk("ERROR: Unable to prepare to enter the soft off system state\n");
			break;
		}

		status = AcpiEnterSleepState(ACPI_STATE_S5);
		if (ACPI_FAILURE(status)) {
			printk("ERROR: Unable to enter the soft off system state\n");
		}

		break;

	case ARCH_SHUTDOWN_MODE_REBOOT:

		/*
		 * There are several ways to reboot a computer. Unfortunately, none of
		 * them will work on all machines. We try to use three methods:
		 * 1. using ACPI;
		 * 2. via the keyboard controller;
		 * 3. generating a triple fault.
		 * You can read more at
		 * http://mjg59.dreamwidth.org/3561.html
		 */

		AcpiReset();
		arch_reset_kbd();
		cpu_triple_reset();

		break;

	case ARCH_SHUTDOWN_MODE_ABORT:
		break;
	}

	ipl_disable();

	while (1) {}
}
