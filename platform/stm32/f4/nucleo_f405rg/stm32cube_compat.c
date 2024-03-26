/**
 * @file
 *
 * @date March 19, 2024
 * @author Efim Perevalov
 */
#include <stdint.h>

#include <hal/clock.h>

uint32_t HAL_GetTick(void) {
	return clock_sys_ticks();
}

