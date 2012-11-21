/**
 * @file
 *
 * @date 10.11.10
 * @author Nikolay Korotky
 */

#include <hal/arch.h>
#include <asm/traps.h>
#include <hal/ipl.h>

void arch_init(void) {
	gdt_init();
	idt_init();
}

void arch_idle(void) {
	__asm__ __volatile__("hlt");
}
#if 0
void arch_reset(void) {
	while (in8(0x64) & 0x2);
	out8(0x60, 0x64);
	while (in8(0x64) & 0x2);
	out8(0x4, 0x60);
	while (in8(0x64) & 0x2);
	out8(0xfe, 0x64);
}
#endif
void __attribute__ ((noreturn)) arch_shutdown(arch_shutdown_mode_t mode) {
	while (1) {}
}
