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

	HAL_TIM_Base_Stop(htim);

	htim->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	TIM_Base_SetConfig(htim->Instance, &htim->Init);

	return 0;
}

int stm32_cube_tim_base_set_periodic(struct clock_source *cs) {
	TIM_HandleTypeDef *htim = cs->driver_priv_data;

	HAL_TIM_Base_Stop(htim);

	htim->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
	TIM_Base_SetConfig(htim->Instance, &htim->Init);

	return 0;
}

int stm32_cube_tim_base_set_next_event(struct clock_source *cs,
		uint32_t next_event) {
	TIM_HandleTypeDef *htim = cs->driver_priv_data;

	htim->Init.Period = next_event - 1;
	TIM_Base_SetConfig(htim->Instance, &htim->Init);

	HAL_TIM_Base_Start(htim);

	return 0;
}

cycle_t stm32_cube_tim_base_read(struct clock_source *cs) {
	TIM_HandleTypeDef *htim = cs->driver_priv_data;

	return htim->Instance->ARR - htim->Instance->CNT;
}

irq_return_t stm32_cube_time_base_irq_handler(unsigned int irq_nr, void *data) {
	clock_tick_handler(data);

	return IRQ_HANDLED;
}

int stm32_cube_tim_base_config(struct time_dev_conf *conf) {
	return 0;
}
