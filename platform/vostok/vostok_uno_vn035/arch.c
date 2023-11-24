#include <assert.h>
#include <stdint.h>

#include <hal/arch.h>
#include <hal/clock.h>
#include <hal/ipl.h>
#include <hal/reg.h>

#include <system_K1921VK035.h>
#include <K1921VK035.h>

#include <framework/mod/options.h>
#include <module/embox/arch/system.h>

void arch_init(void) {

  // static_assert(OPTION_MODULE_GET(embox__arch__system, NUMBER, core_freq) ==
  //               100000000);
  SystemInit();
  SystemCoreClockUpdate();
}

void arch_idle(void) {

}

void arch_shutdown(arch_shutdown_mode_t mode) {
	switch (mode) {
	case ARCH_SHUTDOWN_MODE_HALT:
	case ARCH_SHUTDOWN_MODE_REBOOT:
	case ARCH_SHUTDOWN_MODE_ABORT:
	default:
          NVIC_SystemReset();
          break;
        }

	/* NOTREACHED */
	while(1) {

	}
}

uint32_t HAL_GetTick(void) {
	return clock_sys_ticks();
}
