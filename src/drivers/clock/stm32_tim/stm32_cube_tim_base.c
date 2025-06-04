/**
 * @file
 *
 * @date   12.11.2020
 * @author Alexander Kalmuk
 */

#include <assert.h>
#include <hal/clock.h>
#include <hal/reg.h>
#include <kernel/irq.h>
#include <kernel/time/clock_source.h>
#include "stm32_cube_tim_base.h"

int stm32_cube_tim_base_set_oneshot(struct clock_source *cs) {
	TIM_HandleTypeDef *htim = cs->driver_priv_data;

	HAL_TIM_Base_Stop_IT(htim);

	return 0;
}

int stm32_cube_tim_base_set_periodic(struct clock_source *cs) {
	/* Same as oneshot mode. */
	return stm32_cube_tim_base_set_oneshot(cs);
}

int stm32_cube_tim_base_set_next_event(struct clock_source *cs,
		uint32_t next_event) {
	TIM_HandleTypeDef *htim = cs->driver_priv_data;

	htim->Instance->CR1 |= (1 << TIM_CR1_URS_Pos);
	htim->Instance->ARR = next_event - 1;
	htim->Instance->EGR = TIM_EGR_UG;

	HAL_TIM_Base_Start_IT(htim);

	return 0;
}

cycle_t stm32_cube_tim_get_cycles(struct clock_source *cs) {
	TIM_HandleTypeDef *htim = cs->driver_priv_data;

	if (!(htim->Instance->CR1 & (1 << TIM_CR1_CEN_Pos))) {
		return htim->Instance->ARR;
	}

	return htim->Instance->ARR - htim->Instance->CNT;
}

irq_return_t stm32_cube_time_base_irq_handler(unsigned int irq_nr, void *data) {
	struct clock_source *cs = data;
	TIM_HandleTypeDef *htim = cs->driver_priv_data;

	HAL_TIM_IRQHandler(htim);

	if (cs->event_device->flags & CLOCK_EVENT_ONESHOT_MODE) {
		HAL_TIM_Base_Stop_IT(htim);
	}

	clock_tick_handler(data);

	return IRQ_HANDLED;
}
