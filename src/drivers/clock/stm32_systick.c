/**
 * @file
 * @brief Core clocking device in Cortex-M3
 *
 * @date 23.03.12
 * @author Anton Kozlov
 */

#include <hal/clock.h>
#include <hal/reg.h>
#include <kernel/irq.h>
#include <kernel/time/clock_source.h>
#include <types.h>
#include <hal/interrupt.h>

#include <embox/unit.h>

#include <module/embox/arch/system.h>

#define CORE_FREQ OPTION_MODULE_GET(embox__arch__system,NUMBER,core_freq)
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

static irq_return_t clock_handler(irq_nr_t irq_nr, void *data) {
	clock_tick_handler(irq_nr, data);
	return IRQ_HANDLED;
}

static int this_init(void) {
	return irq_attach(SYSTICK_IRQ, clock_handler, 0, NULL, "stm32 systick timer");
}

static int this_config(struct time_dev_conf * conf) {
	int reload = CORE_FREQ / (CLOCK_DIVIDER * 1000);

	REG_STORE(SYSTICK_CTRL, 0);

	REG_STORE(SYSTICK_RELOAD, reload - 1);

	REG_STORE(SYSTICK_VAL, 0);

	REG_STORE(SCB_SHP_BASE + 2, 0xf0 << (3 * SCB_SHP_PERIF_N));

	REG_STORE(SYSTICK_CTRL, SYSTICK_ENABLE | SYSTICK_TICKINT |
			SYSTICK_CLOCKINIT);

	return 0;
}

static cycle_t this_read(void) {
	return 0;
}

static struct time_event_device this_event = {
	.config = this_config ,
	.resolution = 1000,
	.irq_nr = SYSTICK_IRQ,
};


static struct time_counter_device this_counter = {
	.read = this_read,
	.resolution = CORE_FREQ / CLOCK_DIVIDER,
};

static struct clock_source this_clock_source = {
	.name = "system_tick",
	.event_device = &this_event,
	.counter_device = &this_counter,
	.read = clock_source_read,
};

CLOCK_SOURCE(&this_clock_source);
EMBOX_UNIT_INIT(this_init);
