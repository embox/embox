/**
 * @file
 *
 * @date Aug 28, 2023
 * @author Anton Bondarev
 */

#include <stdint.h>

#include <hal/clock.h>

uint32_t HAL_GetTick(void) {
	return clock_sys_ticks();
}
