/**
 * @file mb_timer.c
 * @brief Implementation for microblaze timers
 * @date 19.11.2009
 * @author Anton Bondarev
 */

#include "autoconf.h"
#include "types.h"
#include "kernel/irq.h"


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

/*1<<(31-XXX_BIT) it's necessary put 31 here because microblaze have bit reverse*/
#define REVERSE_MASK(bit_num) (1<<(31-bit_num))

/** enable both timers t0 and t1. clearing this bit isn't change state ENT bit*/
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

/**
 * Structure one of two timers. Both timers need only for pwm mode
 */
typedef volatile struct mb_timer {
	uint32_t tcsr;            /**< control/status register TCSR */
	uint32_t tlr;             /**< load register TLR */
	uint32_t tcr;             /**< timer/counter register */
} mb_timer_t;

/**
 * Structure of mb_timers module. It contains two timer module.
 */
typedef volatile struct mb_timers {
	mb_timer_t tmr0;
	mb_timer_t tmr1;
}mb_timers;

mb_timers *timers = (mb_timers *)MB_XILINX_TIMERS_BASE;
#define timer0 (&timers->tmr0)

int timers_ctrl_init(IRQ_HANDLER irq_handler) {
	if (-1 == request_irq(XILINX_TIMER_IRQ, irq_handler)){
		return -1;
	}
	/*set clocks period*/
	timer0->tlr = CONFIG_SYS_TIMER_PRELOAD;
	/*clear interrupts bit and load value from tlr register*/
	timer0->tcsr = TIMER_INT | TIMER_RESET;
	/*start timer*/
	timer0->tcsr |= TIMER_ENABLE | TIMER_INT_ENABLE | TIMER_RELOAD | TIMER_DOWN_COUNT;
	return 0;
}

