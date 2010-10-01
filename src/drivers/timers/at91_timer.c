/**
 * @file
 * @brief System timer
 *
 * @date 14.07.2010
 * @author Anton Kozlov
 */

#include <hal/clock.h>
#include <hal/reg.h>
#include <kernel/irq.h>
#include <types.h>
#include <hal/interrupt.h>
#include <drivers/at91sam7s256.h>

#define USECOND (CONFIG_SYS_CLOCK / (16 * 1000000))

static int ticks = 0;
static int delay = 0;
irq_return_t clock_handler(int irq_num, void *dev_id) {
	if (REG_LOAD(AT91C_PITC_PISR)) {
		//REG_ORIN(PIT_MR, PIT_INTERRUPT_ENABLE | PIT_ENABLE); /*p 82, before last paragraph */
#if 0
		ticks ++;
		if (ticks >= delay) {
			ticks = 0;
		}
#endif
		clock_tick_handler(irq_num, dev_id);
	}
        return IRQ_HANDLED;
}

void clock_init(void) {
	irq_attach((irq_nr_t) AT91C_PIT_IRQ, (irq_handler_t) &clock_handler, 0, NULL, "at91 PIT");
}


void clock_setup(useconds_t useconds) {
	delay = useconds;
	REG_STORE(AT91C_PITC_PIMR, AT91C_PITC_PITEN | AT91C_PITC_PITIEN | \
	    (useconds * USECOND));
	REG_LOAD(AT91C_PITC_PIVR);
}

