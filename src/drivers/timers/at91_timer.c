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
#include <AT91SAM7_PIT.h>
#include <AT91SAM7S256.h>

#define CLOCK_IRQ   1
static int ticks = 0;
static int delay = 0;
irq_return_t clock_handler(int irq_num, void *dev_id) {
	if (REG_LOAD(AT91C_PITC_PISR)) {
		//REG_ORIN(PIT_MR, PIT_INTERRUPT_ENABLE | PIT_ENABLE); /*p 82, before last paragraph */
		ticks ++;
		if (ticks >= delay) {
			ticks = 0;
		}
		clock_tick_handler(irq_num, dev_id);
	}
        return IRQ_HANDLED;
}

void clock_init(void) {
	irq_attach((irq_nr_t) CLOCK_IRQ, (irq_handler_t) &clock_handler, 0, NULL, "at91 PIT");
}


void clock_setup(useconds_t useconds) {
	delay = useconds;
	REG_STORE(AT91C_PITC_PIMR, AT91C_PITC_PITEN | AT91C_PITC_PITIEN | (REG_LOAD(AT91C_CKGR_MCFR) & 0xffff) /1000);
	REG_LOAD(AT91C_PITC_PIVR);
}

