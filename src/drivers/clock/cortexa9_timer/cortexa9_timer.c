/**
 * @file
 * @brief Cortex A9 MPCore Timer.
 * @note See Cortex-A9 MPCore Technical Reference Manual for more details
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2016-03-28
 */

#include <stddef.h>
#include <stdint.h>
#include <sys/mman.h>

#include <drivers/common/memory.h>
#include <embox/unit.h>
#include <hal/clock.h>
#include <hal/reg.h>
#include <hal/system.h>
#include <kernel/irq.h>
#include <kernel/printk.h>
#include <kernel/time/clock_source.h>
#include <kernel/time/time.h>
#include <util/log.h>

#define PERIPH_BASE_ADDR OPTION_GET(NUMBER, periph_base_addr)
#define PTIMER_IRQ       OPTION_GET(NUMBER, irq_num)

#define PTIMER_BASE_ADDR (PERIPH_BASE_ADDR + 0x0600)

#define PTIMER_LOAD  (PTIMER_BASE_ADDR + 0x00) /* Load Register */
#define PTIMER_COUNT (PTIMER_BASE_ADDR + 0x04) /* Counter Register */
#define PTIMER_CTRL  (PTIMER_BASE_ADDR + 0x08) /* Control Register */
#define PTIMER_ISR   (PTIMER_BASE_ADDR + 0x0C) /* Interrupt Status Register */

#define PTIMER_CTRL_TE (1 << 0) /* Timer Enable */
#define PTIMER_CTRL_AR (1 << 1) /* Auto Reload */
#define PTIMER_CTRL_IE (1 << 2) /* IRQ Enable */

#define PTIMER_ISR_EF (1 << 0) /* Event Flag */

#define PERIPHCLK  (SYS_CLOCK / 2)
#define LOAD_VALUE ((PERIPHCLK / JIFFIES_PERIOD) - 1)

static int cortexa9_timer_set_periodic(struct clock_source *cs) {
	log_debug("LOAD_VALUE = %d", LOAD_VALUE);

	REG32_STORE(PTIMER_LOAD, LOAD_VALUE);

	REG32_STORE(PTIMER_CTRL, PTIMER_CTRL_TE | PTIMER_CTRL_AR | PTIMER_CTRL_IE);

	return 0;
}

static cycle_t cortexa9_timer_read(struct clock_source *cs) {
	return (cycle_t)LOAD_VALUE - REG32_LOAD(PTIMER_COUNT);
}

static struct time_event_device cortexa9_timer_event = {
    .set_periodic = cortexa9_timer_set_periodic,
    .irq_nr = PTIMER_IRQ,
};

static struct time_counter_device cortexa9_timer_counter = {
    .read = cortexa9_timer_read,
    .cycle_hz = PERIPHCLK,
};

static irq_return_t cortexa9_timer_irq_handler(unsigned int irq_nr,
    void *data) {
	clock_tick_handler(data);

	REG32_STORE(PTIMER_ISR, PTIMER_ISR_EF);

	return IRQ_HANDLED;
}

static int cortexa9_timer_init(struct clock_source *cs) {
	REG32_STORE(PTIMER_CTRL, 0);

	return irq_attach(PTIMER_IRQ, cortexa9_timer_irq_handler, 0, cs,
	    "cortexa9_timer");
}

CLOCK_SOURCE_DEF(cortexa9_timer, cortexa9_timer_init, NULL,
    &cortexa9_timer_event, &cortexa9_timer_counter);

PERIPH_MEMORY_DEFINE(cortexa9_timer, PTIMER_BASE_ADDR, 0x10);
