#include <errno.h>
#include <stdint.h>

#include <drivers/common/memory.h>
#include <drivers/esp32_intmatrix.h>
#include <framework/mod/options.h>
#include <hal/clock.h>
#include <hal/ipl.h>
#include <hal/reg.h>
#include <hal/system.h>
#include <kernel/irq.h>
#include <kernel/time/clock_source.h>
#include <kernel/time/time.h>

#define RTC_FREQ  OPTION_GET(NUMBER, rtc_freq)
#define BASE_ADDR OPTION_GET(NUMBER, base_addr)
#define IRQ_NUM   OPTION_GET(NUMBER, irq_num)

#if defined(__EMBOX_PLATFORM__esp32c5__)
#define SYSTIMER_TARGET0_INTR_SOURCE 61U
#endif

#define SYSTIMER_CONF           (BASE_ADDR + 0x000)
#define SYSTIMER_TARGET0_CONF   (BASE_ADDR + 0x034)
#define SYSTIMER_TARGET0_HI     (BASE_ADDR + 0x01c)
#define SYSTIMER_TARGET0_LO     (BASE_ADDR + 0x020)
#define SYSTIMER_COMP0_LOAD     (BASE_ADDR + 0x050)
#define SYSTIMER_INT_ENA        (BASE_ADDR + 0x064)
#define SYSTIMER_INT_CLR        (BASE_ADDR + 0x06c)
#define SYSTIMER_UNIT0_OP       (BASE_ADDR + 0x004)
#define SYSTIMER_UNIT0_LOAD_HI  (BASE_ADDR + 0x00c)
#define SYSTIMER_UNIT0_LOAD_LO  (BASE_ADDR + 0x010)
#define SYSTIMER_UNIT0_LOAD     (BASE_ADDR + 0x05c)
#define SYSTIMER_UNIT0_VALUE_HI (BASE_ADDR + 0x040)
#define SYSTIMER_UNIT0_VALUE_LO (BASE_ADDR + 0x044)

#define SYSTIMER_TIMER_UNIT_UPDATE      (1 << 30)
#define SYSTIMER_TIMER_UNIT_VALUE_VALID (1 << 29)
#define SYSTIMER_TARGET_PERIOD_MODE     (1 << 30)
#define SYSTIMER_TARGET_PERIOD_MASK     ((1U << 26) - 1)
#define SYSTIMER_TARGET0_WORK_EN        (1U << 24)
#define SYSTIMER_TARGET_HI_MASK         ((1U << 20) - 1)
#define SYSTIMER_CLK_EN                 (1U << 31)
#define SYSTIMER_UNIT0_WORK_EN          (1U << 30)

#define SYSTIMER_COUNTER_MASK ((1ULL << 52) - 1)

static uint64_t last_event_cycles;
static uint32_t period_cycles;

static inline uint64_t esp32_systimer_get_raw_cycles(void) {
	uint32_t hi, lo;
	REG32_ORIN(SYSTIMER_UNIT0_OP, SYSTIMER_TIMER_UNIT_UPDATE);
	while (!(REG32_LOAD(SYSTIMER_UNIT0_OP) & SYSTIMER_TIMER_UNIT_VALUE_VALID)) {}
	hi = REG32_LOAD(SYSTIMER_UNIT0_VALUE_HI);
	lo = REG32_LOAD(SYSTIMER_UNIT0_VALUE_LO);
	return (((uint64_t)hi << 32) | lo) & SYSTIMER_COUNTER_MASK;
}

static int esp32_systimer_set_mode(struct clock_source *cs) {
	/* The core sets the mode flag before calling set_next_event(). */
	REG32_CLEAR(SYSTIMER_CONF, SYSTIMER_TARGET0_WORK_EN);
	REG32_CLEAR(SYSTIMER_INT_ENA, 1);
	REG32_STORE(SYSTIMER_INT_CLR, 1);

	return 0;
}

static int esp32_systimer_set_next_event(struct clock_source *cs,
    uint32_t next_event) {
	uint32_t mode = cs->event_device->flags & CLOCK_EVENT_MODE_MASK;
	uint64_t target;
	ipl_t ipl;

	if (!next_event) {
		return -EINVAL;
	}
	if (mode != CLOCK_EVENT_PERIODIC_MODE && mode != CLOCK_EVENT_ONESHOT_MODE) {
		return -EINVAL;
	}
	if (mode == CLOCK_EVENT_PERIODIC_MODE
	    && next_event > SYSTIMER_TARGET_PERIOD_MASK) {
		return -EINVAL;
	}

	ipl = ipl_save();
	REG32_CLEAR(SYSTIMER_CONF, SYSTIMER_TARGET0_WORK_EN);
	REG32_CLEAR(SYSTIMER_INT_ENA, 1);
	REG32_STORE(SYSTIMER_INT_CLR, 1);

	/* Both modes use UNIT0; replace the previous mode and period. */
	if (mode == CLOCK_EVENT_PERIODIC_MODE) {
		REG32_STORE(SYSTIMER_TARGET0_CONF,
		    SYSTIMER_TARGET_PERIOD_MODE | next_event);
	}
	else {
		REG32_STORE(SYSTIMER_TARGET0_CONF, 0);
		last_event_cycles = esp32_systimer_get_raw_cycles();
		target = last_event_cycles + next_event;
		REG32_STORE(SYSTIMER_TARGET0_HI,
		    (uint32_t)(target >> 32) & SYSTIMER_TARGET_HI_MASK);
		REG32_STORE(SYSTIMER_TARGET0_LO, (uint32_t)target);
	}

	REG32_STORE(SYSTIMER_COMP0_LOAD, 1);
	REG32_ORIN(SYSTIMER_INT_ENA, 1);
	period_cycles = next_event;
	if (mode == CLOCK_EVENT_PERIODIC_MODE) {
		/* Sample as close as possible to enabling the periodic alarm. */
		last_event_cycles = esp32_systimer_get_raw_cycles();
	}
	REG32_ORIN(SYSTIMER_CONF, SYSTIMER_TARGET0_WORK_EN);
	ipl_restore(ipl);

	return 0;
}

static struct time_event_device esp32_systimer_ed = {
    .set_periodic = esp32_systimer_set_mode,
    .set_oneshot = esp32_systimer_set_mode,
    .set_next_event = esp32_systimer_set_next_event,
};

static uint64_t esp32_systimer_get_time_ns(struct clock_source *cs) {
	uint64_t cycles = esp32_systimer_get_raw_cycles();

	/* Split the conversion to avoid overflowing cycles * NSEC_PER_SEC. */
	return (cycles / RTC_FREQ) * NSEC_PER_SEC
	       + (cycles % RTC_FREQ) * NSEC_PER_SEC / RTC_FREQ;
}

static cycle_t esp32_systimer_get_cycles(struct clock_source *cs) {
	ipl_t ipl = ipl_save();
	uint64_t cycles;

	cycles = (esp32_systimer_get_raw_cycles() - last_event_cycles)
	         & SYSTIMER_COUNTER_MASK;
	ipl_restore(ipl);

	return cycles;
}

static struct time_counter_device esp32_systimer_cd = {
    .get_cycles = esp32_systimer_get_cycles,
    .get_time = esp32_systimer_get_time_ns,
    .cycle_hz = RTC_FREQ,
};

static irq_return_t esp32_systimer_irq_handler(unsigned int irq_nr, void *data) {
	struct clock_source *cs = data;
	uint64_t elapsed;
	unsigned int ticks = 1;

	if (cs->event_device->flags & CLOCK_EVENT_ONESHOT_MODE) {
		REG32_CLEAR(SYSTIMER_CONF, SYSTIMER_TARGET0_WORK_EN);
		REG32_CLEAR(SYSTIMER_INT_ENA, 1);
	}
	REG32_STORE(SYSTIMER_INT_CLR, 1);

	if (cs->event_device->flags & CLOCK_EVENT_PERIODIC_MODE) {
		elapsed = (esp32_systimer_get_raw_cycles() - last_event_cycles)
		          & SYSTIMER_COUNTER_MASK;
		ticks = elapsed / period_cycles;
		last_event_cycles = (last_event_cycles + (uint64_t)ticks * period_cycles)
		                    & SYSTIMER_COUNTER_MASK;
	}
	else {
		last_event_cycles = (last_event_cycles + period_cycles)
		                    & SYSTIMER_COUNTER_MASK;
	}

	if (ticks) {
		clock_handle_ticks(data, ticks);
	}

	return IRQ_HANDLED;
}

static int esp32_systimer_init(struct clock_source *cs) {
	int res;

	REG32_ORIN(SYSTIMER_CONF, SYSTIMER_CLK_EN);

	/* Leave TARGET0 disabled until set_next_event() supplies an interval. */
	REG32_CLEAR(SYSTIMER_CONF, SYSTIMER_TARGET0_WORK_EN);
	REG32_CLEAR(SYSTIMER_INT_ENA, 1);
	REG32_STORE(SYSTIMER_INT_CLR, 1);
	REG32_STORE(SYSTIMER_TARGET0_CONF, 0); /* Compare against UNIT0. */

#if defined(__EMBOX_PLATFORM__esp32c5__)
	res = esp32_intmatrix_route(SYSTIMER_TARGET0_INTR_SOURCE, IRQ_NUM);
	if (res != 0) {
		return res;
	}
#endif

	res = irq_attach(IRQ_NUM, esp32_systimer_irq_handler, 0, cs, "esp32_systimer");
	if (res != 0) {
		return res;
	}

	REG32_ORIN(SYSTIMER_CONF, SYSTIMER_UNIT0_WORK_EN);

	return 0;
}

CLOCK_SOURCE_DEF(esp32_systimer, esp32_systimer_init, NULL, &esp32_systimer_ed,
    &esp32_systimer_cd);

PERIPH_MEMORY_DEFINE(esp32_systimer, (uintptr_t)BASE_ADDR, 0x80);
