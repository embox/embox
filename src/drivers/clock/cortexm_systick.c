/**
 * @file
 * @brief Core clocking device in Cortex-M3
 *
 * @date 23.03.12
 * @author Anton Kozlov
 */

#include <hal/clock.h>
#include <hal/reg.h>
#include <hal/system.h>
#include <kernel/irq.h>
#include <kernel/time/clock_source.h>

#include <embox/unit.h>

#define CLOCK_DIVIDER 1

#define SYSTICK_IRQ 15

#define SYSTICK_BASE 0xe000e010

#define SYSTICK_CTRL SYSTICK_BASE

#define SYSTICK_CALIB ((int) SYSTICK_BASE + 0x0c)

#define SYSTICK_RELOAD (SYSTICK_BASE + 0x04)
#define SYSTICK_VAL (SYSTICK_BASE + 0x08)

#define SYSTICK_ENABLE 0x01
#define SYSTICK_TICKINT (0x01 << 1)
#define SYSTICK_CLOCKINIT (0x01 << 2)

#define SCB_SHP_BASE ((unsigned int *) 0xe000ed18)
#define SCB_SHP_PERIF_N 8

#define RELOAD_VALUE (SYS_CLOCK / (CLOCK_DIVIDER * 1000))

static struct clock_source this_clock_source;
static irq_return_t clock_handler(unsigned int irq_nr, void *data) {
	clock_tick_handler(irq_nr, data);
	return IRQ_HANDLED;
}

static int this_init(void) {
	clock_source_register(&this_clock_source);
	return irq_attach(SYSTICK_IRQ, clock_handler, 0, &this_clock_source, "stm32 systick timer");
}

static int this_config(struct time_dev_conf * conf) {
	REG_STORE(SYSTICK_CTRL, 0);

	REG_STORE(SYSTICK_RELOAD, RELOAD_VALUE - 1);

	REG_STORE(SYSTICK_VAL, 0);

	REG_STORE(SYSTICK_CTRL, SYSTICK_ENABLE | SYSTICK_TICKINT |
			SYSTICK_CLOCKINIT);

	return 0;
}

cycle_t this_read(void) {
	return RELOAD_VALUE - REG_LOAD(SYSTICK_VAL);
}

static struct time_event_device this_event = {
	.config = this_config,
	.event_hz = 1000,
	.irq_nr = SYSTICK_IRQ,
};


static struct time_counter_device this_counter = {
	.read = this_read,
	.cycle_hz = SYS_CLOCK / CLOCK_DIVIDER,
};

static struct clock_source this_clock_source = {
	.name = "system_tick",
	.event_device = &this_event,
	.counter_device = &this_counter,
	.read = clock_source_read,
};

EMBOX_UNIT_INIT(this_init);

STATIC_IRQ_ATTACH(SYSTICK_IRQ, clock_handler, &this_clock_source);
