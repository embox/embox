/**
 * @file
 *
 * @date Mar 17, 2023
 * @author Anton Bondarev
 */

#include <util/log.h>

#include <stdint.h>
#include <sys/mman.h>
#include <stdbool.h>

#include <drivers/common/memory.h>

#include <hal/clock.h>
#include <hal/reg.h>
#include <hal/system.h>

#include <kernel/irq.h>
#include <kernel/time/clock_source.h>

#include <embox/unit.h>

#include <framework/mod/options.h>
#include <embox/unit.h>

#define BASE_ADDR     OPTION_GET(NUMBER, base_addr)
#define IRQ_NUM       OPTION_GET(NUMBER, irq_num)

#define BIT(nr)                     (1UL << (nr))

#define TIMER_IRQ_EN_REG          0x00
#define TIMER_IRQ_EN(val)         BIT(val)
#define TIMER_IRQ_ST_REG          0x04
#define TIMER_CTL_REG(val)        (0x10 * val + 0x10)
#define TIMER_CTL_ENABLE          BIT(0)
#define TIMER_CTL_RELOAD          BIT(1)
#define TIMER_CTL_CLK_SRC(val)    (((val) & 0x3) << 2)
#define TIMER_CTL_CLK_SRC_OSC24M  (1)
#define TIMER_CTL_CLK_PRES(val)   (((val) & 0x7) << 4)
#define TIMER_CTL_ONESHOT         BIT(7)
#define TIMER_INTVAL_REG(val)     (0x10 * (val) + 0x14)
#define TIMER_CNTVAL_REG(val)     (0x10 * (val) + 0x18)

#define TIMER_SYNC_TICKS          3

static inline void allwinner_sun4i_clkevt_sync(void  *base) {
	uint32_t old = REG32_LOAD(base + TIMER_CNTVAL_REG(1));

	while ((old - REG32_LOAD(base + TIMER_CNTVAL_REG(1))) < TIMER_SYNC_TICKS) {
	//	cpu_relax();
	}
}

static inline void allwinner_sun4i_clkevt_time_stop(void *base, uint8_t timer) {
	uint32_t val = REG32_LOAD(base + TIMER_CTL_REG(timer));
	REG32_STORE(base + TIMER_CTL_REG(timer), val & ~TIMER_CTL_ENABLE);
	allwinner_sun4i_clkevt_sync(base);
}

static inline void allwinner_sun4i_clkevt_time_setup(void *base, uint8_t timer, uint32_t delay) {
	REG32_STORE(base + TIMER_INTVAL_REG(timer), delay);
}

static inline void allwinner_sun4i_clkevt_time_start(void *base, uint8_t timer, int periodic) {
	uint32_t val = REG32_LOAD(base + TIMER_CTL_REG(timer));

	if (periodic) {
		val &= ~TIMER_CTL_ONESHOT;
	} else{
		val |= TIMER_CTL_ONESHOT;
	}

	REG32_STORE((base + TIMER_CTL_REG(timer)), val | TIMER_CTL_ENABLE | TIMER_CTL_RELOAD);
}


static inline void allwinner_sun4i_timer_clear_interrupt(void *base) {
	REG32_STORE(base + TIMER_IRQ_ST_REG, TIMER_IRQ_EN(0));
}

static inline void allwinner_sun4i_show_regs(void) {
	log_debug("\n**********************");
	log_debug("0x00 TIMER_IRQ_EN_REG: 0x%8x", REG32_LOAD(BASE_ADDR + TIMER_IRQ_EN_REG));
	log_debug("0x04 TIMER_IRQ_ST_REG: 0x%8x", REG32_LOAD(BASE_ADDR + TIMER_IRQ_ST_REG));

	log_debug("0x10 TIMER_CTL_REG(0): 0x%8x", REG32_LOAD(BASE_ADDR + TIMER_CTL_REG(0)));
	log_debug("0x14 TIMER_INTVAL_REG(0): 0x%8x", REG32_LOAD(BASE_ADDR + TIMER_INTVAL_REG(0)));
	log_debug("0x18 TIMER_CNTVAL_REG(0): 0x%8x", REG32_LOAD(BASE_ADDR + TIMER_CNTVAL_REG(0)));

	log_debug("0x20 TIMER_CTL_REG(1): 0x%8x", REG32_LOAD(BASE_ADDR + TIMER_CTL_REG(1)));
	log_debug("0x24 TIMER_INTVAL_REG(1): 0x%8x", REG32_LOAD(BASE_ADDR + TIMER_INTVAL_REG(1)));
	log_debug("0x28 TIMER_CNTVAL_REG(1): 0x%8x", REG32_LOAD(BASE_ADDR + TIMER_CNTVAL_REG(1)));
}

static irq_return_t allwinner_sun4i_timer_handler(unsigned int irq_nr, void *dev_id) {
	allwinner_sun4i_timer_clear_interrupt((void *)(uintptr_t)BASE_ADDR);

	clock_tick_handler(dev_id);
	return IRQ_HANDLED;
}

static int allwinner_sun4i_timer_set_oneshot(struct clock_source *cs) {

	allwinner_sun4i_clkevt_time_stop((void *)(uintptr_t)BASE_ADDR, 0);
	allwinner_sun4i_clkevt_time_start((void *)(uintptr_t)BASE_ADDR, 0, false);

	allwinner_sun4i_show_regs();

	return ENOERR;
}


static int allwinner_sun4i_timer_set_periodic(struct clock_source *cs) {

	allwinner_sun4i_clkevt_time_stop((void *)(uintptr_t)BASE_ADDR, 0);
	allwinner_sun4i_clkevt_time_setup((void *)(uintptr_t)BASE_ADDR, 0, 25000); //cs->event_device->event_hz);
	allwinner_sun4i_clkevt_time_start((void *)(uintptr_t)BASE_ADDR, 0, true);

	allwinner_sun4i_show_regs();

	return ENOERR;
}

static int allwinner_sun4i_timer_set_next_event(struct clock_source *cs,
		uint32_t next_event) {

	allwinner_sun4i_clkevt_time_stop((void *)(uintptr_t)BASE_ADDR, 0);
	allwinner_sun4i_clkevt_time_setup((void *)(uintptr_t)BASE_ADDR, 0, 25000 - TIMER_SYNC_TICKS); //cs->event_device->event_hz);
	allwinner_sun4i_clkevt_time_start((void *)(uintptr_t)BASE_ADDR, 0, true);

	allwinner_sun4i_show_regs();

	return ENOERR;
}

static struct time_event_device allwinner_sun4i_timer_event_device  = {
	.set_oneshot = allwinner_sun4i_timer_set_oneshot,
	.set_periodic = allwinner_sun4i_timer_set_periodic,
	.set_next_event = allwinner_sun4i_timer_set_next_event,
	.name = "allwinner_sun4i_timer",
	.irq_nr = IRQ_NUM
};

static int allwinner_sun4i_timer_init(struct clock_source *cs) {
	uint32_t val;

	REG32_STORE(BASE_ADDR + TIMER_INTVAL_REG(1), ~0);
	REG32_STORE(BASE_ADDR + TIMER_CTL_REG(1),
			TIMER_CTL_ENABLE | TIMER_CTL_RELOAD |
			TIMER_CTL_CLK_SRC(TIMER_CTL_CLK_SRC_OSC24M));

	REG32_STORE(BASE_ADDR + TIMER_CTL_REG(0),
			TIMER_CTL_CLK_SRC(TIMER_CTL_CLK_SRC_OSC24M));

	/* Make sure timer is stopped before playing with interrupts */
	allwinner_sun4i_clkevt_time_stop((void *)(uintptr_t)BASE_ADDR, 0);

	/* clear timer0 interrupt */
	allwinner_sun4i_timer_clear_interrupt((void *)(uintptr_t)BASE_ADDR);

	/* Enable timer0 interrupt */
	val = REG32_LOAD(BASE_ADDR + TIMER_IRQ_EN_REG);
	REG32_STORE(BASE_ADDR + TIMER_IRQ_EN_REG, val | TIMER_IRQ_EN(0));

	allwinner_sun4i_show_regs();

	return irq_attach(IRQ_NUM, allwinner_sun4i_timer_handler, 0, cs, "allwinner sun4i timer");
}

STATIC_IRQ_ATTACH(IRQ_NUM, allwinner_sun4i_timer_handler, &this_clock_source);

PERIPH_MEMORY_DEFINE(allwinner_sun4i_timer, BASE_ADDR, 0x100);

CLOCK_SOURCE_DEF(allwinner_sun4i_timer, allwinner_sun4i_timer_init, NULL,
	&allwinner_sun4i_timer_event_device, NULL);
