/**
 * @file
 * @brief Core clocking device in Cortex-M
 *
 * @date 23.03.12
 * @author Anton Kozlov
 */

#include <hal/clock.h>
#include <hal/reg.h>
#include <hal/system.h>
#include <arm/exception.h>
#include <kernel/irq.h>
#include <kernel/time/clock_source.h>

#include <framework/mod/options.h>
#include <embox/unit.h>

#define SYSTICK_HZ  OPTION_GET(NUMBER,systick_hz)

#define SYSTICK_BASE 0xe000e010

#define SYSTICK_CTRL    (SYSTICK_BASE + 0x0)
# define SYSTICK_ENABLE    (1 << 0)
# define SYSTICK_TICKINT   (1 << 1)
# define SYSTICK_CLOCKINIT (1 << 2)
#define SYSTICK_RELOAD  (SYSTICK_BASE + 0x4)
#define SYSTICK_VAL     (SYSTICK_BASE + 0x8)
#define SYSTICK_CALIB   (SYSTICK_BASE + 0xc)

#define RELOAD_VALUE (SYS_CLOCK / SYSTICK_HZ)

static struct clock_source cortexm_systick_clock_source;

static irq_return_t cortexm_systick_irq_handler(unsigned int irq_nr, void *data) {
	clock_tick_handler(data);
	return IRQ_HANDLED;
}

static int cortexm_systick_init(void) {
	return clock_source_register(&cortexm_systick_clock_source);
}

static int cortexm_systick_config(struct time_dev_conf * conf) {
	REG_STORE(SYSTICK_CTRL, 0);
	REG_STORE(SYSTICK_RELOAD, RELOAD_VALUE - 1);
	REG_STORE(SYSTICK_VAL, 0);
	REG_STORE(SYSTICK_CTRL, SYSTICK_ENABLE | SYSTICK_TICKINT |
			SYSTICK_CLOCKINIT);

	return 0;
}

static cycle_t cortexm_systick_read(void) {
	return RELOAD_VALUE - REG_LOAD(SYSTICK_VAL);
}

static struct time_event_device cortexm_systick_event = {
	.config = cortexm_systick_config,
	.event_hz = SYSTICK_HZ,
	.irq_nr = SYSTICK_IRQ,
};

static struct time_counter_device cortexm_systick_counter = {
	.read = cortexm_systick_read,
	.cycle_hz = SYS_CLOCK,
};

static struct clock_source cortexm_systick_clock_source = {
	.name = "system_tick",
	.event_device = &cortexm_systick_event,
	.counter_device = &cortexm_systick_counter,
	.read = clock_source_read,
};

EMBOX_UNIT_INIT(cortexm_systick_init);

STATIC_EXC_ATTACH(SYSTICK_IRQ, cortexm_systick_irq_handler, &cortexm_systick_clock_source);
