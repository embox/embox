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
#include <compiler.h>

extern void cpu_triple_reset(void);
extern void acpi_shutdown(void);
extern void acpi_reset(void);

static void arch_reset_kbd(void) {
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

void arch_init(void) {
}

void arch_idle(void) {
	__asm__ __volatile__("hlt");
}

void _NORETURN arch_shutdown(arch_shutdown_mode_t mode) {

	switch (mode) {
	case ARCH_SHUTDOWN_MODE_HALT:
		acpi_shutdown();
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
		acpi_reset();
		arch_reset_kbd();
		cpu_triple_reset();

		break;

	case ARCH_SHUTDOWN_MODE_ABORT:
		break;
	}

	ipl_disable();

	while (1) {}
}
