/**
 * @file sp804_timer.c
 * @brief ARM Dual-Timer (SP804) Module driver
 * @author Nishant Malpani <nish.malpani25@gmail.com>
 * @date 2020-03-07
 *
 * @author Svirin Evgeny <eugenysvirin@gmail.com>
 * @date 2021-02-20
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

#define SP804_BASE      OPTION_GET(NUMBER, base_addr) /* Base address */
#define SP804_IRQ       OPTION_GET(NUMBER, irq_nr) /* Interrupt Request number */
#define SP804_TARGET_HZ OPTION_GET(NUMBER, target_freq) /* Frequency to run the timer at */
#define SP804_TIMCLK    OPTION_GET(NUMBER, TIMCLK_freq) /* Frequency of the SP804 clock timer - 1 mHz, frequency of subtraction from Timer1 Load Register */

#define SP804_T1_LR     (SP804_BASE + 0x00) /* Timer1 Load Register */
#define SP804_T1_VR     (SP804_BASE + 0x04) /* Timer1 Current Value Register */
#define SP804_T1_CR     (SP804_BASE + 0x08) /* Timer1 Control Register */
#define SP804_T1_ICR    (SP804_BASE + 0x0C) /* Timer1 Interrupt Clear Register */
#define SP804_T1_RIS    (SP804_BASE + 0x10) /* Timer1 Raw Interrupt Status Register */
#define SP804_T1_MIS    (SP804_BASE + 0x14) /* Timer1 Masked Interrupt Status Register */
#define SP804_T1_BGL    (SP804_BASE + 0x18) /* Timer1 Background Load Register */

#define SP804_CR_MASK       (0b11101111) /* Mask for bits 0-7 */
#define SP804_CR_WRAPPING   (0 << 0) /* OneShot bit */
#define SP804_CR_32BIT      (1 << 1) /* TimerSize bit */
#define SP804_CR_TIMERPRE   (0 << 2) /* Prescale bits */
#define SP804_CR_INTEN      (1 << 5) /* InterruptEnable bit */
#define SP804_CR_PERIODIC   (1 << 6) /* TimerMode bit */
#define SP804_CR_TIMEREN    (1 << 7) /* TimerEnable bit */
#define SP804_ICR_CLEAR     (1 << 0) /* InterruptClear bit */

#define SP804_PRESCALE          1 /* Prescaler operation - divide the rate of master clock */
#define SP804_LR_VAL            ((SP804_TIMCLK) / (SP804_PRESCALE * SP804_TARGET_HZ))

static irq_return_t clock_handler(unsigned int irq_nr, void *data) {
    clock_tick_handler(data);

    REG32_STORE(SP804_T1_ICR, SP804_ICR_CLEAR); /* Clear interrupts */
    REG32_STORE(SP804_T1_LR, SP804_LR_VAL);
    return IRQ_HANDLED;
}

static int sp804_timer_init(struct clock_source *cs) {
    return irq_attach(SP804_IRQ,
                      clock_handler,
                      0,
                      cs,
                      "SP804");
}

static int this_set_periodic(struct clock_source *cs) {
    /* Unset bits 0-7, also disables the timer to proceed with new config settings */
    REG32_CLEAR(SP804_T1_CR, SP804_CR_MASK);

    REG32_STORE(SP804_T1_LR, SP804_LR_VAL); /* Don't care in free-running mode */

    /* setup Timer 1 as 32-bit interruptable periodic clocksource */
    REG32_STORE(SP804_T1_CR, SP804_CR_32BIT | SP804_CR_INTEN | 
                SP804_CR_PERIODIC | SP804_CR_TIMEREN);

    return 0;
}

static cycle_t this_get_cycles(struct clock_source *cs) {
    return REG32_LOAD(SP804_T1_VR);
}

static struct time_event_device sp804_timer_event = {
    .set_periodic = this_set_periodic,
    .irq_nr = SP804_IRQ,
};

static struct time_counter_device sp804_timer_counter = {
    .get_cycles = this_get_cycles,
    .cycle_hz = 1000,
};

STATIC_IRQ_ATTACH(SP804_IRQ, clock_handler, &this_clock_source);

PERIPH_MEMORY_DEFINE(sp804, SP804_BASE, 0x1C);

CLOCK_SOURCE_DEF(sp804_timer, sp804_timer_init, NULL,
	&sp804_timer_event, &sp804_timer_counter);
