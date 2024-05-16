/**
 * @file arch.c
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 26.12.2017
 */
#include <hal/cpu_idle.h>
#include <hal/mmu.h>
#include <hal/platform.h>
#include <hal/reg.h>
#include <kernel/panic.h>

/* Watchdog registers */
#define WCR 0x20bc000
#define WSR 0x20bc000

void _NORETURN platform_shutdown(shutdown_mode_t mode) {
	switch (mode) {
	case SHUTDOWN_MODE_REBOOT:
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

	while (1) {
		arch_cpu_idle();
	}
}
