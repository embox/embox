/**
 * @file
 *
 * @date   12.11.2020
 * @author Alexander Kalmuk
 */

#include <assert.h>
#include <kernel/irq.h>
#include <kernel/time/clock_source.h>
#include <hal/clock.h>
#include <hal/system.h>
#include <framework/mod/options.h>
#include "stm32_cube_tim_base.h"

/* Desired clock hz, we will set Presacaler to reach this value. */
#define TIM_CLK_HZ   10000

#define TIM_IRQ  OPTION_GET(NUMBER, irq)
static_assert(TIM_IRQ == TIM3_IRQn, "");

static TIM_HandleTypeDef TimHandle;

static struct time_event_device stm32_cube_tim_event = {
	.set_oneshot = stm32_cube_tim_base_set_oneshot,
	.set_periodic = stm32_cube_tim_base_set_periodic,
	.set_next_event = stm32_cube_tim_base_set_next_event,
	.irq_nr = TIM_IRQ,
};

static struct time_counter_device stm32_cube_tim_counter = {
	.get_cycles = stm32_cube_tim_get_cycles,
	.cycle_hz = TIM_CLK_HZ,
	.mask = 0xffff,
};

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim) {
	__HAL_RCC_TIM3_CLK_ENABLE();
}

static int stm32_cube_tim3_init(struct clock_source *cs) {
	uint32_t uwPrescalerValue;

	/* Compute the prescaler value to have TIMx counter clock equal to TIM_CLK_HZ */
	uwPrescalerValue = (uint32_t)((SystemCoreClock / 2) / TIM_CLK_HZ) - 1;

	TimHandle.Instance = TIM3;

	TimHandle.Init.Prescaler         = uwPrescalerValue;
	TimHandle.Init.ClockDivision     = 0;
	TimHandle.Init.CounterMode       = TIM_COUNTERMODE_DOWN;
	TimHandle.Init.RepetitionCounter = 0;
	TimHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

	if (HAL_TIM_Base_Init(&TimHandle) != HAL_OK) {
		return -1;
	}

	return irq_attach(TIM_IRQ, stm32_cube_time_base_irq_handler, 0,
			cs, "stm32_cube_tim3");
}

CLOCK_SOURCE_DEF(stm32_cube_tim3, stm32_cube_tim3_init, &TimHandle,
	&stm32_cube_tim_event, &stm32_cube_tim_counter);

STATIC_IRQ_ATTACH(TIM_IRQ, stm32_cube_time_base_irq_handler,
	&CLOCK_SOURCE_NAME(stm32_cube_tim3));
