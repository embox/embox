/**
 * @file
 *
 * @date 05.05.2020
 * @author Alexander Kalmuk
 */
#include <hal/arch.h>
#include <hal/reg.h>

#define GPREG_BASE            0x50003300
#define GPREG_SET_FREEZE_REG (GPREG_BASE + 0x0)

void arch_init(void) {
	/* Disable watchdog. It was enabled by bootloader. */
	REG16_STORE(GPREG_SET_FREEZE_REG, (1 << 3));
}

void arch_idle(void) {
}

void _NORETURN arch_shutdown(arch_shutdown_mode_t mode) {
	while (1) {
	}
}
