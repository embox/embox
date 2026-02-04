/**
 * @file
 *
 * @date 05 aug 2015
 * @author: Anton Bondarev
 */

#include <stddef.h>
#include <stdint.h>
#include <sys/mman.h>

#include <drivers/common/memory.h>
#include <hal/clock.h>
#include <hal/reg.h>
#include <hal/system.h>
#include <kernel/irq.h>
#include <kernel/time/clock_source.h>
#include <kernel/time/time.h>

#define TIMER0_BASE OPTION_GET(NUMBER, base_addr)
#define TIMER0_IRQ  5

#define CYCLE_PER_SEC  SYS_CLOCK
#define CYCLE_PER_TICK (CYCLE_PER_SEC / JIFFIES_PERIOD)

/* Timer0 registers */
#define TMR_LOAD   (TIMER0_BASE + 0x000)
#define TMR_VAL    (TIMER0_BASE + 0x004)
#define TMR_CTRL   (TIMER0_BASE + 0x008)
#define TMR_CLR    (TIMER0_BASE + 0x00c)
#define TMR_BGLOAD (TIMER0_BASE + 0x024)

/* Timer control register */
#define TCTRL_DISABLE  0x00
#define TCTRL_ENABLE   0x80
#define TCTRL_PERIODIC 0x40
#define TCTRL_INTEN    0x20
#define TCTRL_SCALE256 0x08
#define TCTRL_SCALE16  0x04
#define TCTRL_32BIT    0x02
#define TCTRL_ONESHOT  0x01

static int integratorcp_clock_setup(struct clock_source *cs) {
	/* Setup counter value */
	REG32_STORE(TMR_CTRL, TCTRL_DISABLE);
	REG32_STORE(TMR_LOAD, CYCLE_PER_TICK);
	REG32_ORIN(TMR_CTRL, (TCTRL_ENABLE | TCTRL_PERIODIC));

	/* Enable timer interrupt */
	REG32_ORIN(TMR_CTRL, TCTRL_INTEN);

	return 0;
}

static struct time_event_device integratorcp_event_device = {
    .set_periodic = integratorcp_clock_setup,
    .name = "integratorcp_clk",
    .irq_nr = TIMER0_IRQ,
};

static cycle_t integratorcp_get_cycles(struct clock_source *cs) {
	return (CYCLE_PER_TICK - 1) - (REG32_LOAD(TMR_VAL) & 0xFFFF);
}

static uint64_t integratorcp_get_time(struct clock_source *cs) {
	clock_t jiffies;
	cycle_t cycles;

	do {
		jiffies = cs->event_device->jiffies;
		cycles = integratorcp_get_cycles(cs);
	} while (jiffies != cs->event_device->jiffies);

	cycles += jiffies * CYCLE_PER_TICK;

#if NSEC_PER_SEC >= CYCLE_PER_SEC
	return cycles * (NSEC_PER_SEC / CYCLE_PER_SEC);
#else
	return cycles / (CYCLE_PER_SEC / NSEC_PER_SEC);
#endif
}

static struct time_counter_device integratorcp_counter_device = {
    .get_cycles = integratorcp_get_cycles,
    .get_time = integratorcp_get_time,
    .cycle_hz = CYCLE_PER_SEC,
};

static irq_return_t clock_handler(unsigned int irq_nr, void *dev_id) {
	clock_tick_handler(dev_id);
	REG32_STORE(TMR_CLR, 0x01); /* Clear timer interrupt */
	return IRQ_HANDLED;
}

static int integratorcp_cs_init(struct clock_source *cs) {
	return irq_attach(TIMER0_IRQ, clock_handler, 0, cs, "integratorcp_clk");
}

CLOCK_SOURCE_DEF(integratorcp, integratorcp_cs_init, NULL,
    &integratorcp_event_device, &integratorcp_counter_device);

PERIPH_MEMORY_DEFINE(integratorcp_clock, TIMER0_BASE, 0x30);
