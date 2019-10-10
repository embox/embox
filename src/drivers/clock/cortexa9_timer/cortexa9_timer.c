/**
 * @file
 * @brief Cortex A9 MPCore Timer.
 * @note See Cortex-A9 MPCore Technical Reference Manual for more details
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2016-03-28
 */
#include <util/log.h>

#include <sys/mman.h>

#include <drivers/common/memory.h>
#include <hal/clock.h>
#include <hal/reg.h>
#include <hal/system.h>
#include <kernel/printk.h>
#include <kernel/irq.h>
#include <kernel/time/clock_source.h>
#include <embox/unit.h>

#define PERIPH_BASE_ADDR OPTION_GET(NUMBER, periph_base_addr)

#define PTIMER_BASE_ADDR (PERIPH_BASE_ADDR + 0x0600)

#define PTIMER_LOAD      (PTIMER_BASE_ADDR + 0x00)
#define PTIMER_COUNTER   (PTIMER_BASE_ADDR + 0x04)
#define PTIMER_CONTROL   (PTIMER_BASE_ADDR + 0x08)
#define PTIMER_IS	     (PTIMER_BASE_ADDR + 0x0C) /* Interrupt State Register */

#define PTIMER_ENABLE             0x1
#define PTIMER_AUTO_RELOAD        0x2
#define PTIMER_IRQ_ENABLE         0x4

#define PTIMER_IRQ                OPTION_GET(NUMBER, irq_num)

#define PTIMER_PRESCALER_SHIFT    8

#define PERIPHCLK       (SYS_CLOCK / 2)
#define TARGET_FREQ		OPTION_GET(NUMBER, freq)
#define LOAD_VALUE		PERIPHCLK / (TARGET_FREQ - 1)
//#define LOAD_VALUE 0x10000000

static struct clock_source this_clock_source;
static irq_return_t clock_handler(unsigned int irq_nr, void *data) {
	//static int cnt = 0;

	clock_tick_handler(irq_nr, data);
	REG_STORE(PTIMER_IS, 0x1);

	//log_debug("irq clock %d", cnt++);

	return IRQ_HANDLED;
}

static int this_init(void) {
	REG_STORE(PTIMER_CONTROL, 0);
	clock_source_register(&this_clock_source);
	return irq_attach(PTIMER_IRQ,
	                  clock_handler,
	                  0,
	                  &this_clock_source,
	                  "Cortex A9 systick timer");
}

static int this_config(struct time_dev_conf * conf) {
	uint32_t tmp;
	uint8_t  prescaler = 0;

	log_debug("LOAD_VALUE = %d", LOAD_VALUE);

	REG_STORE(PTIMER_LOAD, LOAD_VALUE);

	tmp  = REG_LOAD(PTIMER_CONTROL);
	tmp |= PTIMER_ENABLE | PTIMER_AUTO_RELOAD | PTIMER_IRQ_ENABLE;
	tmp |= prescaler << PTIMER_PRESCALER_SHIFT;

	REG_STORE(PTIMER_CONTROL, tmp);
	return 0;
}

static cycle_t this_read(void) {
	return LOAD_VALUE - REG_LOAD(PTIMER_COUNTER);
}

static struct time_event_device this_event = {
	.config = this_config,
	.event_hz = 1000,
	.irq_nr = PTIMER_IRQ,
};

static struct time_counter_device this_counter = {
	.read = this_read,
	.cycle_hz = 1000,
};

static struct clock_source this_clock_source = {
	.name = "system_tick",
	.event_device = &this_event,
	.counter_device = &this_counter,
	.read = clock_source_read,
};

EMBOX_UNIT_INIT(this_init);

STATIC_IRQ_ATTACH(PTIMER_IRQ, clock_handler, &this_clock_source);

PERIPH_MEMORY_DEFINE(cortexa9_timer, PTIMER_BASE_ADDR, 0x10);
