/**
 * @file
 *
 * @brief Implementation for microblaze timers
 *
 * @date 19.11.09
 * @author Anton Bondarev
 */

#include <drivers/common/memory.h>

#include <asm/bitops.h>
#include <hal/clock.h>
#include <hal/system.h>
#include <kernel/irq.h>
#include <kernel/panic.h>
#include <kernel/time/clock_source.h>
#include <kernel/time/ktime.h>

#include <embox/unit.h>

#include <module/embox/driver/clock/mb_timer.h>

#define CONFIG_XILINX_TIMER_BASEADDR OPTION_GET(NUMBER,mbtimer_base)
#define CONFIG_XILINX_TIMER_IRQ      OPTION_GET(NUMBER,irq_num)

#define TIMER_PRELOAD (SYS_CLOCK / 1000)

/*bits definition of cntl/status (tcsr) register*/
#define TIMER_ENALL_BIT  21      /**< ENALL */
#define TIMER_PWM_BIT    22      /**< PWM */
#define TIMER_INT_BIT    23      /**< TxINT */
#define TIMER_ENT_BIT    24      /**< ENT */
#define TIMER_ENIT_BIT   25      /**< ENIT */
#define TIMER_LOAD_BIT   26      /**< LOAD */
#define TIMER_ARHT_BIT   27      /**< ARHT */
#define TIMER_CAPT_BIT   28      /**< CAPT */
#define TIMER_GENT_BIT   29      /**< GENT */
#define TIMER_UDT_BIT    30      /**< UDT */
#define TIMER_MDT        31      /**< MDT */

/** enable both timers t0 and t1.
 * clearing this bit isn't change state ENT bit */
#define TIMER_ENABLE_ALL    REVERSE_MASK(TIMER_ENALL_BIT)
/** interrupt was pending. Write '1' for clearing this bit*/
#define TIMER_INT           REVERSE_MASK(TIMER_INT_BIT)
/** enable timer*/
#define TIMER_ENABLE        REVERSE_MASK(TIMER_ENT_BIT)
/** enable timer interrupt*/
#define TIMER_INT_ENABLE    REVERSE_MASK(TIMER_ENIT_BIT)
/** load value from tlr register*/
#define TIMER_RESET         REVERSE_MASK(TIMER_LOAD_BIT)
/** set reload mode*/
#define TIMER_RELOAD        REVERSE_MASK(TIMER_ARHT_BIT)
/** set down count mode*/
#define TIMER_DOWN_COUNT    REVERSE_MASK(TIMER_UDT_BIT)

static struct time_event_device mb_ed;

/**
 * Structure one of two timers. Both timers need only for pwm mode
 */
typedef volatile struct timer_regs {
	uint32_t tcsr; /**< control/status register TCSR */
	uint32_t tlr; /**< load register TLR */
	uint32_t tcr; /**< timer/counter register */
} timer_regs_t;

/**
 * Microblaze timer module contains two timer module, each of them is described
 * in @link srtuct timer_regs @endlink
 */
typedef volatile struct mb_timers {
	timer_regs_t tmr0;
	timer_regs_t tmr1;
} mb_timers_t;

static mb_timers_t *timers = (mb_timers_t *) CONFIG_XILINX_TIMER_BASEADDR;
#define timer0 (&timers->tmr0)

/*
 * we must use proxy for interrupt handler because we must clean bit in register
 * timer.
 */
static irq_return_t clock_handler(unsigned int irq_nr, void *dev_id) {
	clock_tick_handler(dev_id);
	timer0->tcsr |= TIMER_INT;
	return IRQ_HANDLED;
}

static cycle_t mb_get_cycles(struct clock_source *cs) {
	return TIMER_PRELOAD - timer0->tcr;

}

static int mb_timer_init(struct clock_source *cs) {
	if (0 != irq_attach(CONFIG_XILINX_TIMER_IRQ, clock_handler, 0, cs, "mbtimer")) {
		panic("mbtimer irq_attach failed");
	}
	return 0;
}

static int mb_clock_setup(struct clock_source *cs) {
	/*set clocks period*/
	timer0->tlr = TIMER_PRELOAD;
	/*clear interrupts bit and load value from tlr register*/
	timer0->tcsr = TIMER_INT | TIMER_RESET;
	/*start timer*/
	timer0->tcsr = TIMER_ENABLE | TIMER_INT_ENABLE | TIMER_RELOAD
			| TIMER_DOWN_COUNT;

	return 0;
}

static struct time_event_device mb_ed = {
	.set_periodic = mb_clock_setup,
	.name = "mb_timer",
	.irq_nr = CONFIG_XILINX_TIMER_IRQ
};

static struct time_counter_device mb_cd = {
	.get_cycles = mb_get_cycles,
	.cycle_hz = SYS_CLOCK,
};

PERIPH_MEMORY_DEFINE(mb_timer, CONFIG_XILINX_TIMER_BASEADDR, sizeof(struct timer_regs));

CLOCK_SOURCE_DEF(mb_timer, mb_timer_init, NULL,
	&mb_ed, &mb_cd);
