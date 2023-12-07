#include <assert.h>
#include <stdint.h>

#include <hal/arch.h>
#include <hal/clock.h>
#include <hal/ipl.h>
#include <hal/reg.h>

#include <framework/mod/options.h>
#include <module/embox/arch/system.h>

void arch_init(void) {

}

void arch_idle(void) {
	__asm__ __volatile__("WFI");
}

void arch_shutdown(arch_shutdown_mode_t mode) {
	switch (mode) {
	case ARCH_SHUTDOWN_MODE_HALT:
	case ARCH_SHUTDOWN_MODE_REBOOT:
	case ARCH_SHUTDOWN_MODE_ABORT:
	default:
          break;
        }

	/* NOTREACHED */
	while(1) {

	}
}

