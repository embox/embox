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

static irq_return_t clock_handler(unsigned int irq_num, void *dev_id) {
	if (REG_LOAD(AT91C_PITC_PISR)) {
		REG_LOAD(AT91C_PITC_PIVR);
		clock_tick_handler(dev_id);
	}
	return IRQ_HANDLED;
}

static int at91_pitc_init(struct clock_source *cs) {
	REG_STORE(AT91C_PMC_PCER, AT91C_ID_SYS);
	return irq_attach(AT91C_ID_SYS, clock_handler, 0, cs, "at91 PIT");
}

static int at91_pitc_set_periodic(struct clock_source *cs);

static struct time_event_device at91_pitc_event = {
	.set_periodic = at91_pitc_set_periodic,
	.irq_nr = AT91C_ID_SYS
};


static cycle_t at91_pitc_get_cycles(struct clock_source *cs) {
	return 0; //REG_LOAD(AT91C_PITC_PIVR);
}

static struct time_counter_device at91_pitc_counter = {
	.get_cycles = at91_pitc_get_cycles,
	.cycle_hz = AT91_PIT_COUNTER_RES,
};

static int at91_pitc_set_periodic(struct clock_source *cs) {
	REG_LOAD(AT91C_PITC_PIVR);
	REG_STORE(AT91C_PITC_PIMR, AT91C_PITC_PITEN | AT91C_PITC_PITIEN |
	    (at91_pitc_counter.cycle_hz / AT91_PIT_EVENT_RES));
	return 0;
}

CLOCK_SOURCE_DEF(at91_pitc, at91_pitc_init, NULL,
	&at91_pitc_event, &at91_pitc_counter);
