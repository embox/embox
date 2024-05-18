/**
 * @file
 *
 * @date May 13, 2024
 * @author Zeng ZiXian
 */
#include <stdint.h>

#include <hal/clock.h>

uint32_t HAL_GetTick(void) {
	return clock_sys_ticks();
}

