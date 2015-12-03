/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    30.10.2014
 */

#include <assert.h>
#include <stdint.h>

#include <hal/arch.h>

#include <system_stm32f3xx.h>
#include <stm32f3xx_hal_cortex.h>

#include <framework/mod/options.h>
#include <module/embox/arch/system.h>



void arch_init(void) {
	static_assert(OPTION_MODULE_GET(embox__arch__system, NUMBER, core_freq) == 144000000);
	SystemInit();
}

void arch_idle(void) {

}

void arch_shutdown(arch_shutdown_mode_t mode) {
	switch (mode) {
	case ARCH_SHUTDOWN_MODE_HALT:
	case ARCH_SHUTDOWN_MODE_REBOOT:
	case ARCH_SHUTDOWN_MODE_ABORT:
	default:
		//HAL_NVIC_SystemReset();
		break;
	}

	/* NOTREACHED */
	while(1) {

	}
}
