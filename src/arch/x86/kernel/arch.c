/**
 * @file
 *
 * @date 10.11.10
 * @author Nikolay Korotky
 */

#include <hal/arch.h>
#include <asm/traps.h>
#include <hal/ipl.h>
#include <asm/io.h>

void arch_init(void) {
	gdt_init();
	idt_init();
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

	//outw(0xB004, 0x0 | 0x2000);
	//asm("cli;hlt");

	while (1) {}
}

