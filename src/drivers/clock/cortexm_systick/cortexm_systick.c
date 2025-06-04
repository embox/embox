/**
 * @file
 * @brief Core clocking device in Cortex-M
 *
 * @date 23.03.12
 * @author Anton Kozlov
 */

#include <assert.h>
#include <hal/clock.h>
#include <hal/reg.h>
#include <hal/system.h>
#include <arm/exception.h>
#include <kernel/irq.h>
#include <kernel/time/clock_source.h>

#include <framework/mod/options.h>
#include <embox/unit.h>

#define SYSTICK_BASE 0xe000e010

#define SYSTICK_CTRL    (SYSTICK_BASE + 0x0)
# define SYSTICK_ENABLE    (1 << 0)
# define SYSTICK_TICKINT   (1 << 1)
# define SYSTICK_CLOCKINIT (1 << 2)
#define SYSTICK_RELOAD  (SYSTICK_BASE + 0x4)
# define SYSTICK_RELOAD_MSK  0xfffffful
#define SYSTICK_VAL     (SYSTICK_BASE + 0x8)
#define SYSTICK_CALIB   (SYSTICK_BASE + 0xc)

static irq_return_t cortexm_systick_irq_handler(unsigned int irq_nr, void *data) {
	struct clock_source *cs = data;

	clock_tick_handler(data);

	if (cs->event_device->flags & CLOCK_EVENT_ONESHOT_MODE) {
		/* Systick do not support one-shot mode, so we do
		 * it by shutting Systick down. */
		REG_STORE(SYSTICK_CTRL, 0);
		REG_STORE(SYSTICK_VAL, 0);
	}

	return IRQ_HANDLED;
}

static int cortexm_systick_set_oneshot(struct clock_source *cs) {
	return 0;
}

static int cortexm_systick_set_periodic(struct clock_source *cs) {
	return 0;
}

static int cortexm_systick_set_next_event(struct clock_source *cs,
		uint32_t next_event) {
	REG_STORE(SYSTICK_CTRL, 0);
	REG_STORE(SYSTICK_RELOAD, next_event - 1);
	REG_STORE(SYSTICK_VAL, 0);
	REG_STORE(SYSTICK_CTRL, SYSTICK_ENABLE | SYSTICK_TICKINT |
			SYSTICK_CLOCKINIT);

	return 0;
}

static int cortexm_systick_init(struct clock_source *cs) {
	/* Disable clock. */
	REG_STORE(SYSTICK_CTRL, 0);

	return 0;
}

static cycle_t cortexm_systick_get_cycles(struct clock_source *cs) {
	return REG_LOAD(SYSTICK_RELOAD) - REG_LOAD(SYSTICK_VAL);
}

static struct time_event_device cortexm_systick_event = {
	.set_oneshot = cortexm_systick_set_oneshot,
	.set_periodic = cortexm_systick_set_periodic,
	.set_next_event = cortexm_systick_set_next_event,
	.irq_nr = SYSTICK_IRQ,
};

static struct time_counter_device cortexm_systick_counter = {
	.get_cycles = cortexm_systick_get_cycles,
	.cycle_hz = SYS_CLOCK,
	.mask = SYSTICK_RELOAD_MSK,
};

CLOCK_SOURCE_DEF(cortexm_systick, cortexm_systick_init, NULL,
	&cortexm_systick_event, &cortexm_systick_counter);

STATIC_EXC_ATTACH(SYSTICK_IRQ, cortexm_systick_irq_handler, &CLOCK_SOURCE_NAME(cortexm_systick));
