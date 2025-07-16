/**
 * @file epit.c
 * @brief Enhanced.Periodic.Interrupt.Timer driver
 * @author Puranjay Mohan <puranjay12@gmail.com>
 * @version 0.1
 * @date 2020-03-05
 */

#include <drivers/common/memory.h>
#include <hal/clock.h>
#include <hal/reg.h>
#include <kernel/irq.h>
#include <kernel/printk.h>
#include <kernel/time/clock_source.h>
#include <kernel/time/time_device.h>

#include <framework/mod/options.h>
#include <embox/unit.h>

#define EPIT_BASE		OPTION_GET(NUMBER, base_addr)
#define EPIT_IRQ		OPTION_GET(NUMBER, irq_nr)
#define EPIT_TARGET_HZ	OPTION_GET(NUMBER, freq)
#define EPIT_FREQ		66000000

#define EPIT_CR		(EPIT_BASE + 0x00) /* Control Register */
#define EPIT_SR		(EPIT_BASE + 0x04) /* Status Register */
#define EPIT_LR		(EPIT_BASE + 0x08) /* Load Register */
#define EPIT_CMPR		(EPIT_BASE + 0x0C) /* Compare Register */
#define EPIT_CNR		(EPIT_BASE + 0x10) /* Counter Register */

#define EPIT_PRESCALER	66 /* prescaler value of 66 */
#define EPIT_CR_PRESCLR	(EPIT_PRESCALER << 4) /* Prescaler value of 66 */
#define EPIT_CR_EN		(1 << 0) /* Enable Bit */
#define EPIT_CR_ENMOD		(1 << 1) /* Enable Mode Bit */
#define EPIT_CR_OCIEN		(1 << 2) /* Output compare Interrupt Enable bit*/
#define EPIT_CR_RLD		(1 << 3) /* Counter Reload Control bit */
#define EPIT_SR_CLEAR		(1 << 0) /* Clear the status register*/
#define EPIT_CR_CLKSRC	(1 << 24) /* Use peripheral clock as input, 66MHz */
#define EPIT_CR_IOVW		(1 << 17) /* Load Immidiate Override*/
#define EPIT_LR_VALUE		((EPIT_FREQ)/(EPIT_PRESCALER * EPIT_TARGET_HZ))

static irq_return_t clock_handler(unsigned int irq_nr, void *data) {
	clock_tick_handler(data);
	REG32_STORE(EPIT_SR, EPIT_SR_CLEAR);
	return IRQ_HANDLED;
}

static int epit_init(struct clock_source *cs) {
	return irq_attach(EPIT_IRQ,
			clock_handler,
			0,
			cs,
			"EPIT");
}

static int epit_set_periodic(struct clock_source *cs) {
	/* changin clock source to peripheral clock */
	REG32_STORE(EPIT_CR, REG32_LOAD(EPIT_CR) & (~EPIT_CR_OCIEN)); /* Turn off the interrupt */
	REG32_STORE(EPIT_CR, REG32_LOAD(EPIT_CR) & (~EPIT_CR_EN)); /* disable the timer*/
	REG32_STORE(EPIT_CR, REG32_LOAD(EPIT_CR) | EPIT_CR_CLKSRC); /* Select the clock source */
	REG32_STORE(EPIT_SR, EPIT_SR_CLEAR); /* Clear the status register */
	REG32_STORE(EPIT_CR, REG32_LOAD(EPIT_CR) | EPIT_CR_ENMOD); /* Set enable mode = 1 */
	REG32_STORE(EPIT_CR, REG32_LOAD(EPIT_CR) | EPIT_CR_EN); /* enable the timer */
	REG32_STORE(EPIT_CR, REG32_LOAD(EPIT_CR) | EPIT_CR_OCIEN); /* Enable Interrupts */

	/* setting the load and compare registers */
	REG32_STORE(EPIT_CR, REG32_LOAD(EPIT_CR) | EPIT_CR_RLD | EPIT_CR_IOVW | EPIT_CR_PRESCLR); /* Set reload and overrite */
	REG32_STORE(EPIT_LR, EPIT_LR_VALUE); /* Load regiter set to (clck_src)/(presclr*target_freq) */
	REG32_STORE(EPIT_CMPR, 0x0); /* Compare register set to 0 */
	return 0;
}

static cycle_t epit_get_cycles(struct clock_source *cs) {
	return REG32_LOAD(EPIT_CNR);
}

static struct time_event_device epit_event = {
    .set_periodic = epit_set_periodic,
    .irq_nr = EPIT_IRQ,
};

static struct time_counter_device epit_counter = {
    .get_cycles = epit_get_cycles,
    .cycle_hz = EPIT_TARGET_HZ,
};

STATIC_IRQ_ATTACH(EPIT_IRQ, clock_handler, &imx6_clock_source);

PERIPH_MEMORY_DEFINE(epit_mem, EPIT_BASE, 0x14);

CLOCK_SOURCE_DEF(epit, epit_init, NULL,
	&epit_event, &epit_counter);
