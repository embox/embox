/**
 * @file
 * @brief System timer
 *
 * @date 14.07.10
 * @author Anton Kozlov
 */

#include <hal/clock.h>
#include <hal/reg.h>
#include <kernel/irq.h>
#include <kernel/clock_source.h>
#include <types.h>
#include <hal/interrupt.h>
#include <drivers/at91sam7s256.h>

static LIST_HEAD(timers_list);
static struct clock_source at91_pit_clock_source;

irq_return_t clock_handler(int irq_num, void *dev_id) {
	if (REG_LOAD(AT91C_PITC_PISR)) {
		REG_LOAD(AT91C_PITC_PIVR);
		clock_tick_handler(irq_num, dev_id);
	}
	return IRQ_HANDLED;
}

void clock_init(void) {
	REG_STORE(AT91C_PMC_PCER, AT91C_ID_SYS);
	irq_attach((irq_nr_t) AT91C_ID_SYS,
		(irq_handler_t) &clock_handler, 0, NULL, "at91 PIT");
	at91_pit_clock_source.flags = 1;
	at91_pit_clock_source.precision = 1000;
	at91_pit_clock_source.timers_list = &timers_list;
	clock_source_register(&at91_pit_clock_source);
}

void clock_setup(useconds_t useconds) {
	REG_LOAD(AT91C_PITC_PIVR);
	REG_STORE(AT91C_PITC_PIMR, AT91C_PITC_PITEN | AT91C_PITC_PITIEN |
	    (useconds * AT91C_PIT_USECOND));
}
