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
#define make_reg(offset) ((unsigned long *) (BASE_ADDR + offset))
#define BASE_ADDR 0xfffffd30 /*Periodical interval timer base. PIT supposed to OS scheduling */
#define PIT_MR	 make_reg(0x0)
#define PIT_SR	 make_reg(0x04)
#define PIT_PIVR make_reg(0x08)
#define CLOCK_IRQ   1
#define PIT_ENABLE 0x1000000
#define PIT_INTERRUPT_ENABLE 0x2000000
#define MCK 36000   //XXX assume running on Slow Clock

static int ticks = 0; /* will cotain msecs since last IRQ. HW allows not simply incrementing, but counting real msecs */
static int delay = 0;
irq_return_t clock_handler(int irq_num, void *dev_id) {
	if (REG_LOAD(PIT_SR)) {
		ticks ++;
		REG_ORIN(PIT_MR, PIT_INTERRUPT_ENABLE | PIT_ENABLE); /*p 82, before last paragraph */
		if (ticks >= delay) {
			ticks = 0;
#if 0
			//TODO what does mean si_handler
			si_handler();
#endif
		}
		clock_tick_handler(irq_num, dev_id);
	}
        return IRQ_HANDLED;
}

void clock_init(void) {
	//REG_STORE(PIT_MR, PIT_ENABLE);
	//REG_LOAD(PIT_PIVR);
	irq_attach((irq_nr_t) CLOCK_IRQ, (irq_handler_t) &clock_handler, 0, NULL, "at91 PIT");
}


void clock_setup(useconds_t useconds) {
	delay = useconds;
	REG_STORE(PIT_MR, PIT_ENABLE | PIT_INTERRUPT_ENABLE | MCK/16 /1000);
	REG_LOAD(PIT_PIVR);
}

