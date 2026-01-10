#include <errno.h>
#include <stdint.h>


#include <hal/clock.h>
#include <hal/reg.h>
#include <hal/system.h>
#include <kernel/irq.h>
#include <kernel/time/clock_source.h>
#include <kernel/time/time.h>
#include <drivers/common/memory.h>

#include <framework/mod/options.h>

#define BASE_ADDR       OPTION_GET(NUMBER,irq_num)
#define IRQ_NUM         OPTION_GET(NUMBER,irq_num)

struct esp32_systimer_val64 {
	volatile uint32_t hi;
	volatile uint32_t low;
} __attribute__((packed));

struct esp32_systimer_regs {
	volatile uint32_t conf;                                  /* 0x000 */
	volatile uint32_t unit_op[2];                            /* 0x004 */
	volatile struct esp32_systimer_val64 unit_load_val[2];   /* 0x00c */
	volatile struct esp32_systimer_val64 target_val[3];      /* 0x01c */
	volatile uint32_t target_conf[3];                        /* 0x034 */
	volatile struct esp32_systimer_val64 unit_val[2];        /* 0x040 */
	volatile uint32_t comp_load[3];                          /* 0x050 */
	volatile uint32_t unit_load[2];                          /* 0x05c */
	volatile uint32_t int_ena;                               /* 0x064 */
	volatile uint32_t int_raw;                               /* 0x068 */
	volatile uint32_t int_clr;                               /* 0x06c */
	volatile uint32_t int_st;                                /* 0x070 */
	uint32_t reserved_074;
	uint32_t reserved_078;
	uint32_t reserved_07c;
	uint32_t reserved_080;
	uint32_t reserved_084;
	uint32_t reserved_088;
	uint32_t reserved_08c;
	uint32_t reserved_090;
	uint32_t reserved_094;
	uint32_t reserved_098;
	uint32_t reserved_09c;
	uint32_t reserved_0a0;
	uint32_t reserved_0a4;
	uint32_t reserved_0a8;
	uint32_t reserved_0ac;
	uint32_t reserved_0b0;
	uint32_t reserved_0b4;
	uint32_t reserved_0b8;
	uint32_t reserved_0bc;
	uint32_t reserved_0c0;
	uint32_t reserved_0c4;
	uint32_t reserved_0c8;
	uint32_t reserved_0cc;
	uint32_t reserved_0d0;
	uint32_t reserved_0d4;
	uint32_t reserved_0d8;
	uint32_t reserved_0dc;
	uint32_t reserved_0e0;
	uint32_t reserved_0e4;
	uint32_t reserved_0e8;
	uint32_t reserved_0ec;
	uint32_t reserved_0f0;
	uint32_t reserved_0f4;
	uint32_t reserved_0f8;
	volatile uint32_t date;                                  /* 0x070 */
} __attribute__((packed));

static int esp32c3_systimer_set_periodic(struct clock_source *cs) {

	return 0;
}

static struct time_event_device esp32c3_systimer_ed = {
	.set_periodic = esp32c3_systimer_set_periodic,
};

/*
esp32c3_systimer_cd has not been implemented yet

static cycle_t esp32c3_systimer_get_cycles(struct clock_source *cs) {
	return 0;
}

static struct time_counter_device esp32c3_systimer_cd = {
	.get_cycles = esp32c3_systimer_get_cycles,
	.cycle_hz = 1000000,
};

*/

static irq_return_t esp32c3_systimer_irq_handler(unsigned int irq_nr,
														void *data) {

	//systimer_ll_clear_alarm_int(systimer_hal.dev, SYSTIMER_ALARM_OS_TICK_CORE0);
	clock_tick_handler(data);

	return IRQ_HANDLED;
}


static int esp32c3_systimer_init(struct clock_source *cs) {
	int res;
	res = irq_attach(IRQ_NUM, esp32c3_systimer_irq_handler, 0, cs, "esp32_systimer");
	if (res != 0) {
		return 0;
	}

	// systimer_ll_enable_clock(hal->dev, true);
	// systimer_ll_connect_alarm_counter(hal->dev, alarm_id, counter_id);
	// systimer_ll_enable_counter(hal->dev, counter_id, true);
	// systimer_ll_counter_can_stall_by_cpu(hal->dev, counter_id, cpu_id, can);

	// systimer_ll_enable_alarm(systimer_hal.dev, SYSTIMER_ALARM_OS_TICK_CORE0, false);
	// systimer_ll_set_alarm_target(systimer_hal.dev, SYSTIMER_ALARM_OS_TICK_CORE0, alarm.val);
	// systimer_ll_apply_alarm_value(systimer_hal.dev, SYSTIMER_ALARM_OS_TICK_CORE0);
	// systimer_ll_enable_alarm(systimer_hal.dev, SYSTIMER_ALARM_OS_TICK_CORE0, true);
	// systimer_ll_enable_alarm_int(systimer_hal.dev, SYSTIMER_ALARM_OS_TICK_CORE0, true);
	return 0;
}

CLOCK_SOURCE_DEF(esp32c3_systimer, esp32c3_systimer_init, NULL,
	&esp32c3_systimer_ed, NULL/* &esp32c3_systimer_cd */);

PERIPH_MEMORY_DEFINE(esp32c3_systimer, (uintptr_t) BASE_ADDR, 0x80);