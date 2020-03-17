/**
 * @file sp804_timer.c
 * @brief ARM Dual-Timer (SP804) Module driver
 * @author Nishant Malpani <nish.malpani25@gmail.com>
 * @date 2020-03-07
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
#define SP804_TIMCLK    100000000 /* Frequency of the SP804 clock timer - 100 MHz */

#define SP804_T1_LR     (SP804_BASE + 0x00) /* Timer1 Load Register */
#define SP804_T1_VR     (SP804_BASE + 0x04) /* Timer1 Current Value Register */
#define SP804_T1_CR     (SP804_BASE + 0x08) /* Timer1 Control Register */
#define SP804_T1_ICR    (SP804_BASE + 0x0C) /* Timer1 Interrupt Clear Register */
#define SP804_T1_RIS    (SP804_BASE + 0x10) /* Timer1 Raw Interrupt Status Register */
#define SP804_T1_MIS    (SP804_BASE + 0x14) /* Timer1 Masked Interrupt Status Register */
#define SP804_T1_BGL    (SP804_BASE + 0x18) /* Timer1 Background Load Register */

#define SP804_CR_MASK           (0b11101111) /* Mask for bits 0-7 */
#define SP804_CR_ONESHOT        (0 << 0) /* OneShot bit; 0 -> wrapping mode, 1 -> one-shot mode */
#define SP804_CR_TIMERSIZE      (0 << 1) /* TimerSize bit; 0 -> 16-bit counter, 1 -> 32-bit counter */
#define SP804_CR_TIMERPRE       (0 << 2) /* Prescale bits; 0 -> div by 1, 1 -> div by 16, 2 -> div by 256 */
#define SP804_CR_INTEN          (1 << 5) /* InterruptEnable bit */
#define SP804_CR_TIMERMODE      (0 << 6) /* TimerMode bit; 0 -> free-running mode, 1 -> periodic mode */
#define SP804_CR_TIMEREN        (1 << 7) /* TimerEnable bit */
#define SP804_ICR_CLEAR         (1 << 0) /* InterruptClear bit */

#define SP804_PRESCALE          1 /* Prescaler operation - divide the rate of master clock */
#define SP804_LR_VAL            ((SP804_TIMCLK) / (SP804_PRESCALE * SP804_TARGET_HZ))

static struct clock_source sp804_clock_source;
static irq_return_t clock_handler(unsigned int irq_nr, void *data) {
    clock_tick_handler(irq_nr, data);

    REG32_STORE(SP804_T1_ICR, SP804_ICR_CLEAR); /* Clear interrupts */
    REG32_STORE(SP804_T1_LR, SP804_LR_VAL);
    return IRQ_HANDLED;
}

static int sp804_init(void) {
    clock_source_register(&sp804_clock_source);

    return irq_attach(SP804_IRQ,
                      clock_handler,
                      0,
                      &sp804_clock_source,
                      "SP804");
}

static int sp804_config(struct time_dev_conf * conf) {
    REG32_CLEAR(SP804_T1_CR, SP804_CR_MASK); /* Unset bits 0-7, also disables the timer to proceed with new config settings */

    REG32_ORIN(SP804_T1_CR, (SP804_CR_ONESHOT | SP804_CR_TIMERMODE)); /* Select wrapping, free-running mode */
    REG32_ORIN(SP804_T1_CR, SP804_CR_TIMERSIZE); /* Select 32-bit counter mode */
    REG32_ORIN(SP804_T1_CR, SP804_CR_TIMERPRE); /* Set prescaler rate */
    REG32_ORIN(SP804_T1_CR, SP804_CR_INTEN); /* Enable interrupts */

    REG32_STORE(SP804_T1_LR, SP804_LR_VAL); /* Don't care in free-running mode */
    REG32_ORIN(SP804_T1_CR, SP804_CR_TIMEREN); /* Enable timer */

    return 0;
}

static cycle_t sp804_read(void) {
    return REG32_LOAD(SP804_T1_VR);
}

static struct time_event_device sp804_event = {
    .config   = sp804_config,
    .event_hz = SP804_TARGET_HZ,
    .irq_nr   = SP804_IRQ,
};

static struct time_counter_device sp804_counter = {
    .read     = sp804_read,
    .cycle_hz = SP804_TARGET_HZ,
};

static struct clock_source sp804_clock_source = {
    .name           = "sp804",
    .event_device   = &sp804_event,
    .counter_device = &sp804_counter,
    .read           = clock_source_read,
};

EMBOX_UNIT_INIT(sp804_init);

STATIC_IRQ_ATTACH(SP804_IRQ, clock_handler, &sp804_clock_source);

PERIPH_MEMORY_DEFINE(sp804, SP804_BASE, 0x1C);

