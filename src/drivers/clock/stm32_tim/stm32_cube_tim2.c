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
#include <embox/unit.h>
#include "stm32_cube_tim_base.h"

EMBOX_UNIT_INIT(stm32_cube_tim_init);

/* Desired clock hz, we will set Presacaler to reach this value. */
#define TIM_CLK_HZ   10000

#define TIM_HZ   OPTION_GET(NUMBER, hz)
#define TIM_IRQ  OPTION_GET(NUMBER, irq)
static_assert(TIM_IRQ == TIM2_IRQn);

static TIM_HandleTypeDef TimHandle;

static struct time_event_device stm32_cube_tim_event = {
	.set_oneshot = stm32_cube_tim_base_set_oneshot,
	.set_periodic = stm32_cube_tim_base_set_periodic,
	.set_next_event = stm32_cube_tim_base_set_next_event,
	.event_hz = TIM_HZ,
	.irq_nr = TIM_IRQ,
};

static struct time_counter_device stm32_cube_tim_counter = {
	.read = stm32_cube_tim_base_read,
	.cycle_hz = TIM_CLK_HZ,
	.mask = 0xffffffff,
};

static struct clock_source stm32_cube_tim_clock_source = {
	.name = "TIM2",
	.event_device = &stm32_cube_tim_event,
	.counter_device = &stm32_cube_tim_counter,
	.driver_priv_data = &TimHandle,
};

STATIC_IRQ_ATTACH(TIM_IRQ, stm32_cube_time_base_irq_handler,
	&stm32_cube_tim_clock_source)

static int stm32_cube_tim_init(void) {
	uint32_t uwPrescalerValue;

	/* Compute the prescaler value to have TIMx counter clock equal to TIM_CLK_HZ */
	uwPrescalerValue = (uint32_t)((SystemCoreClock / 2) / TIM_CLK_HZ) - 1;

	TimHandle.Instance = TIM2;

	TimHandle.Init.Prescaler         = uwPrescalerValue;
	TimHandle.Init.ClockDivision     = 0;
	TimHandle.Init.CounterMode       = TIM_COUNTERMODE_DOWN;
	TimHandle.Init.RepetitionCounter = 0;
	TimHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

	if (HAL_TIM_Base_Init(&TimHandle) != HAL_OK) {
		return -1;
	}

	clock_source_register(&stm32_cube_tim_clock_source);

	return irq_attach(TIM_IRQ, stm32_cube_time_base_irq_handler, 0,
			&stm32_cube_tim_clock_source, "tim2_cs");
}
