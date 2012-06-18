/**
 * @file
 *
 * @brief Implementation for microblaze timers
 *
 * @date 19.11.09
 * @author Anton Bondarev
 */

#include <types.h>
#include <bitops.h>
#include <embox/unit.h>
#include <kernel/irq.h>
#include <kernel/panic.h>

#include <hal/clock.h>
#include <kernel/clock_source.h>
#include <kernel/clock_event.h>
#include <kernel/ktime.h>

#include <module/embox/arch/system.h>

#define SYS_CLOCK     OPTION_MODULE_GET(embox__arch__system,NUMBER,core_freq)
#define TIMER_PRELOAD (SYS_CLOCK/1000)

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

static struct clock_source mb_timer_clock_source = {
	.name = "mb_timer"
};


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
static irq_return_t clock_handler(irq_nr_t irq_nr, void *dev_id) {
	timer0->tcsr |= TIMER_INT;
	clock_tick_handler(irq_nr,dev_id);
	return IRQ_HANDLED;
}
static int mb_clock_init(void);


static void mb_clock_setup(uint32_t mode) {

}


static struct clock_event_device mb_timer_device = {
	.set_mode = mb_clock_setup,
	.init = mb_clock_init,
	.cs = &mb_timer_clock_source,
	.resolution = 1000,
	.name = "mb_timer"
};

CLOCK_EVENT_DEVICE(&mb_timer_device);

static cycle_t mb_timer_read(const struct cyclecounter *cc) {
	return timer0->tcr;

}

static struct cyclecounter cc = {
	.read = mb_timer_read,
	.mult = 1,
	.shift = 0
};

static int mb_clock_init(void) {
	/*set clocks period*/
	timer0->tlr = TIMER_PRELOAD;
	/*clear interrupts bit and load value from tlr register*/
	timer0->tcsr = TIMER_INT | TIMER_RESET;
	/*start timer*/
	timer0->tcsr = TIMER_ENABLE | TIMER_INT_ENABLE | TIMER_RELOAD
			| TIMER_DOWN_COUNT;

	if (0 != irq_attach(CONFIG_XILINX_TIMER_IRQ, clock_handler, 0, NULL, "mbtimer")) {
		panic("mbtimer irq_attach failed");
	}

	mb_timer_clock_source.flags = 1;
	mb_timer_clock_source.resolution = 1000;
	mb_timer_clock_source.dev = &mb_timer_device;
	mb_timer_clock_source.cc = &cc;
	clock_source_register(&mb_timer_clock_source);

	/* Calculate mult/shift constants to set clock_source used by timer */
	clock_source_calc_mult_shift(&mb_timer_clock_source, SYS_CLOCK, 0);

	return 0;
}
