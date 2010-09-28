/**
 * @file arch.c
 * @brief ARM
 *
 * @date 21.06.2010
 * @author Anton Kozlov
 */

#include <hal/reg.h>

void arch_init(void) {
	//XXX to disable watchdog
	//XXX to setup clock
}

void arch_idle(void) {
}

void arch_shutdown(void) {
	while(1);
}
