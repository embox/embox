/**
 * @file
 * @brief System timer
 *
 * @date 14.07.10
 * @author Anton Kozlov
 */

#include <embox/unit.h>
#include <drivers/at91sam7s256.h>
#include <drivers/irqctrl.h>
#include <hal/clock.h>
#include <hal/reg.h>
#include <hal/system.h>
#include <kernel/irq.h>
#include <kernel/time/clock_source.h>

#define AT91_PIT_COUNTER_RES (SYS_CLOCK / 16)
#define AT91_PIT_EVENT_RES 1000

static struct clock_source at91_pitc_clock_source;

static irq_return_t clock_handler(unsigned int irq_num, void *dev_id) {
	if (REG_LOAD(AT91C_PITC_PISR)) {
		REG_LOAD(AT91C_PITC_PIVR);
		clock_tick_handler(irq_num, dev_id);
	}
	return IRQ_HANDLED;
}

static int at91_pitc_init(void) {
	clock_source_register(&at91_pitc_clock_source);
	REG_STORE(AT91C_PMC_PCER, AT91C_ID_SYS);
	return irq_attach(AT91C_ID_SYS, clock_handler, 0, &at91_pitc_clock_source, "at91 PIT");
}

static int at91_pitc_config(struct time_dev_conf * conf);

static struct time_event_device at91_pitc_event = {
	.config = at91_pitc_config,
	.event_hz = AT91_PIT_EVENT_RES,
	.irq_nr = AT91C_ID_SYS
};


static cycle_t at91_pitc_read(void) {
	return 0; //REG_LOAD(AT91C_PITC_PIVR);
}

static struct time_counter_device at91_pitc_counter = {
	.read = at91_pitc_read,
	.cycle_hz = AT91_PIT_COUNTER_RES,
};

static struct clock_source at91_pitc_clock_source = {
	.name = "at91_pitc",
	.event_device = &at91_pitc_event,
	.counter_device = &at91_pitc_counter,
	.read = clock_source_read,
};

static int at91_pitc_config(struct time_dev_conf * conf) {
	REG_LOAD(AT91C_PITC_PIVR);
	REG_STORE(AT91C_PITC_PIMR, AT91C_PITC_PITEN | AT91C_PITC_PITIEN |
	    (at91_pitc_counter.cycle_hz / at91_pitc_event.event_hz));
	return 0;
}

EMBOX_UNIT_INIT(at91_pitc_init);
