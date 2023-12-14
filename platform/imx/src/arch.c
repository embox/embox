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
}

void _NORETURN arch_shutdown(arch_shutdown_mode_t mode) {

	switch (mode) {
	case ARCH_SHUTDOWN_MODE_REBOOT:
		mmu_off(); /* Access possibly unmapped registers */
		REG16_STORE(WCR, 0);
		REG16_STORE(WSR, 0x5555);
		REG16_STORE(WSR, 0xAAAA);
		REG16_STORE(WCR, 0x1);

		/* Shouldn't be reached */
		panic("Reboot failed!\n");
		break;
	default:
		break;
		/* NIY */
	}

	while(1) {}
}
