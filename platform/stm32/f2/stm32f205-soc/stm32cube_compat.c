#include <stdint.h>

#include <hal/clock.h>

uint32_t HAL_GetTick(void) {
	return clock_sys_ticks();
}
