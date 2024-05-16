/**
 * @file
 *
 * @date Aug 28, 2023
 * @author Anton Bondarev
 */

#include <stdint.h>

#include <bsp/stm32cube_hal.h>
#include <hal/clock.h>

uint32_t HAL_GetTick(void) {
	return clock_sys_ticks();
}

HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority) {
	return HAL_OK;
}
