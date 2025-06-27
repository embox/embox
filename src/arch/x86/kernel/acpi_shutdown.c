/**
 * @file
 *
 * @date 12.03.2015
 *  @author: Anton Bondarev
 */

#include <acpi.h>
#include <compiler.h>

#include <asm/io.h>
#include <asm/traps.h>
#include <hal/cpu_idle.h>
#include <hal/ipl.h>
#include <hal/platform.h>
#include <kernel/printk.h>

extern void cpu_triple_reset(void);

static void acpi_shutdown(void) {
	ACPI_STATUS status;

	status = AcpiEnterSleepStatePrep(ACPI_STATE_S5);
	if (ACPI_FAILURE(status)) {
		printk("ERROR: Unable to prepare to enter the soft off system state\n");
		return;
	}

	status = AcpiEnterSleepState(ACPI_STATE_S5);
	if (ACPI_FAILURE(status)) {
		printk("ERROR: Unable to enter the soft off system state\n");
	}
}

static void acpi_reset(void) {
	AcpiReset();
}

static void arch_reset_kbd(void) {
	while (in8(0x64) & 0x2) {}
	out8(0x60, 0x64);

	while (in8(0x64) & 0x2) {}
	out8(0x4, 0x60);

	while (in8(0x64) & 0x2) {}
	out8(0xfe, 0x64);
}

void _NORETURN platform_shutdown(shutdown_mode_t mode) {
	switch (mode) {
	case SHUTDOWN_MODE_HALT:
		acpi_shutdown();
		break;

	case SHUTDOWN_MODE_REBOOT:

		/*
		 * There are several ways to reboot a computer. Unfortunately, none of
		 * them will work on all machines. We try to use three methods:
		 * 1. using ACPI;
		 * 2. via the keyboard controller;
		 * 3. generating a triple fault.
		 * You can read more at
		 * http://mjg59.dreamwidth.org/3561.html
		 */
		acpi_reset();
		arch_reset_kbd();
		cpu_triple_reset();

		break;

	case SHUTDOWN_MODE_ABORT:
		break;
	}

	ipl_disable();

	while (1) {
		arch_cpu_idle();
	}
}
