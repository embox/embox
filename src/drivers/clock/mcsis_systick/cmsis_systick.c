/**
 * @file
 * @brief Core clocking device in Cortex ARM-M
 *
 * @date 25.03.2014
 * @author Anton Kozlov
 */


#include <errno.h>
#include <hal/clock.h>
#include <hal/system.h>
#include <arm/exception.h>
#include <kernel/irq.h>
#include <kernel/time/clock_source.h>
#include <hal/reg.h>

#include <embox/unit.h>

#define CLOCK_DIVIDER 1

#define SYSTICK_IRQ 15

#include <module/embox/arch/arm/cmsis.h>

static irq_return_t cmsis_systick_clock_handler(unsigned int irq_nr, void *data) {
	struct clock_source *cs = data;

	clock_tick_handler(data);

	if (cs->event_device->flags & CLOCK_EVENT_ONESHOT_MODE) {
		/* Systick do not support one-shot mode, so we do
		 * it by shutting Systick down. */
		REG_STORE(SysTick->CTRL, 0);
		REG_STORE(SysTick->VAL, 0);
	}

	return IRQ_HANDLED;
}

static int cmsis_systick_set_periodic(struct clock_source *cs) {
	int reload = SYS_CLOCK / (CLOCK_DIVIDER * 1000);

	return 0 == SysTick_Config(reload) ? 0 : -EINVAL;
}

static struct time_event_device cmsis_systick_event = {
	.set_periodic = cmsis_systick_set_periodic,
	.irq_nr = SYSTICK_IRQ,
};

#if 0
static cycle_t this_get_cycles(struct clock_source *cs) {
	return 0;
}

static struct time_counter_device this_counter = {
	.get_cycles = this_get_cycles,
	.cycles_hz = SYS_CLOCK / CLOCK_DIVIDER,
};
#endif

CLOCK_SOURCE_DEF(cmsis_systick, NULL, NULL,
	&cmsis_systick_event, NULL);

STATIC_EXC_ATTACH(SYSTICK_IRQ, cmsis_systick_clock_handler,  &CLOCK_SOURCE_NAME(cmsis_systick));
