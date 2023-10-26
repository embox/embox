/**
 * @file arch.c
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 26.12.2017
 */
#include <hal/arch.h>
#include <hal/mmu.h>
#include <hal/reg.h>
#include <kernel/panic.h>

/* Watchdog registers */
#define WCR 0x20bc000
#define WSR 0x20bc000

void arch_init(void) {
}

void arch_idle(void) {
	__asm__ __volatile__("WFI");
}

void _NORETURN arch_shutdown(arch_shutdown_mode_t mode) {
	while(1) {}
}

void __extenddftf2(void) {
}

void __trunctfdf2(void) {
}
