/**
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 29.05.24
 */

#include <assert.h>
#include <endian.h>
#include <stddef.h>
#include <stdint.h>

#include <framework/mod/options.h>
#include <hal/clock.h>
#include <hal/reg.h>
#include <hal/system.h>
#include <kernel/irq.h>
#include <kernel/time/clock_source.h>
#include <kernel/time/time.h>

#define SC64_TIMER_BASE_ADDR OPTION_GET(NUMBER, base_addr)
#define SC64_TIMER_IRQ_NUM   OPTION_GET(NUMBER, irq_num)

#define SC64_TIMER_ID    (SC64_TIMER_BASE_ADDR + 0x000)
#define SC64_TIMER_CAUSE (SC64_TIMER_BASE_ADDR + 0x008)

#define SC64_TIMER_TS_CTRL (SC64_TIMER_BASE_ADDR + 0x140)
#define SC64_TIMER_TS_SSI  (SC64_TIMER_BASE_ADDR + 0x144)
#define SC64_TIMER_TS_STS  (SC64_TIMER_BASE_ADDR + 0x148)
#define SC64_TIMER_TS_STN  (SC64_TIMER_BASE_ADDR + 0x14c)
#define SC64_TIMER_TS_SUP  (SC64_TIMER_BASE_ADDR + 0x150)
#define SC64_TIMER_TS_NUP  (SC64_TIMER_BASE_ADDR + 0x154)
#define SC64_TIMER_TS_TSA  (SC64_TIMER_BASE_ADDR + 0x158)
#define SC64_TIMER_TS_TTS  (SC64_TIMER_BASE_ADDR + 0x15c)
#define SC64_TIMER_TS_TTN  (SC64_TIMER_BASE_ADDR + 0x160)
#define SC64_TIMER_TS_HWS  (SC64_TIMER_BASE_ADDR + 0x164)
#define SC64_TIMER_TS_STAT (SC64_TIMER_BASE_ADDR + 0x168)

#define SC64_TIMER_TS_CTRL_TSENA     (1U << 0)
#define SC64_TIMER_TS_CTRL_TSCFUPDT  (1U << 1)
#define SC64_TIMER_TS_CTRL_TSINIT    (1U << 2)
#define SC64_TIMER_TS_CTRL_TSUPDT    (1U << 3)
#define SC64_TIMER_TS_CTRL_TSTRIG    (1U << 4)
#define SC64_TIMER_TS_CTRL_TSADDREG  (1U << 5)
#define SC64_TIMER_TS_CTRL_TSCTRLSSR (1U << 9)

#define SC64_TIMER_TS_STAT_TSSOVF    (1U << 0)
#define SC64_TIMER_TS_STAT_TSTARGT   (1U << 1)
#define SC64_TIMER_TS_STAT_TSTRGTERR (1U << 3)

#define TARGET_TIME_INC (NSEC_PER_SEC / JIFFIES_PERIOD)
#define NSEC_PER_CYCLE  (NSEC_PER_SEC / SYS_CLOCK)

#define TIMER_REG_STORE(addr, val) REG32_STORE(addr, htole32(val))
#define TIMER_REG_LOAD(addr)       le32toh(REG32_LOAD(addr))

static uint64_t sc64_timer_get_system_time(void) {
	uint32_t sec;
	uint32_t nsec;

	do {
		sec = TIMER_REG_LOAD(SC64_TIMER_TS_STS);
		nsec = TIMER_REG_LOAD(SC64_TIMER_TS_STN);
	} while (sec != TIMER_REG_LOAD(SC64_TIMER_TS_STS));

	return (uint64_t)sec * NSEC_PER_SEC + nsec;
}

static uint64_t sc64_timer_get_target_time(void) {
	uint32_t sec;
	uint32_t nsec;

	do {
		sec = TIMER_REG_LOAD(SC64_TIMER_TS_TTS);
		nsec = TIMER_REG_LOAD(SC64_TIMER_TS_TTN);
	} while (sec != TIMER_REG_LOAD(SC64_TIMER_TS_TTS));

	return (uint64_t)sec * NSEC_PER_SEC + nsec;
}

static void sc64_timer_set_target_time(uint64_t nsec) {
	TIMER_REG_STORE(SC64_TIMER_TS_TTN, nsec % NSEC_PER_SEC);
	TIMER_REG_STORE(SC64_TIMER_TS_TTS, nsec / NSEC_PER_SEC);
}

static cycle_t sc64_timer_get_cycles(struct clock_source *cs) {
	return (sc64_timer_get_system_time() & TARGET_TIME_INC) / NSEC_PER_CYCLE;
}

static uint64_t sc64_timer_get_time(struct clock_source *cs) {
	return sc64_timer_get_system_time();
}

static int sc64_timer_set_periodic(struct clock_source *cs) {
	TIMER_REG_STORE(SC64_TIMER_TS_CTRL, 0x0);

	TIMER_REG_STORE(SC64_TIMER_TS_SSI, NSEC_PER_CYCLE);
	TIMER_REG_STORE(SC64_TIMER_TS_SUP, 0x0);
	TIMER_REG_STORE(SC64_TIMER_TS_NUP, 0x0);
	TIMER_REG_STORE(SC64_TIMER_TS_TTS, 0x0);
	TIMER_REG_STORE(SC64_TIMER_TS_TTN, TARGET_TIME_INC);

	TIMER_REG_STORE(SC64_TIMER_TS_CTRL,
	    SC64_TIMER_TS_CTRL_TSENA | SC64_TIMER_TS_CTRL_TSINIT
	        | SC64_TIMER_TS_CTRL_TSTRIG | SC64_TIMER_TS_CTRL_TSCTRLSSR);

	return 0;
}

static irq_return_t sc64_timer_int_handler(unsigned int irq_nr, void *dev_id) {
	uint64_t system_time;
	uint64_t target_time;
	unsigned int jiffies;

	TIMER_REG_STORE(SC64_TIMER_TS_STAT, SC64_TIMER_TS_STAT_TSTARGT);

	target_time = sc64_timer_get_target_time();
	system_time = sc64_timer_get_system_time();

	assert(system_time > target_time);

	system_time += TARGET_TIME_INC / 2;
	jiffies = ((system_time - target_time) / TARGET_TIME_INC) + 1;

	sc64_timer_set_target_time(target_time + (jiffies * TARGET_TIME_INC));

	TIMER_REG_STORE(SC64_TIMER_TS_CTRL, SC64_TIMER_TS_CTRL_TSENA
	                                        | SC64_TIMER_TS_CTRL_TSTRIG
	                                        | SC64_TIMER_TS_CTRL_TSCTRLSSR);

	clock_handle_ticks(dev_id, jiffies);

	return IRQ_HANDLED;
}

static struct time_event_device sc64_timer_event_device = {
    .name = "sc64_timer_v2",
    .irq_nr = SC64_TIMER_IRQ_NUM,
    .set_periodic = sc64_timer_set_periodic,
};

static struct time_counter_device sc64_timer_counter_device = {
    .get_cycles = sc64_timer_get_cycles,
    .get_time = sc64_timer_get_time,
    .cycle_hz = SYS_CLOCK,
};

static int sc64_timer_init(struct clock_source *cs) {
	return irq_attach(SC64_TIMER_IRQ_NUM, sc64_timer_int_handler, 0, cs,
	    "sc64_timer_v2");
}

CLOCK_SOURCE_DEF(sc64_timer_v2, sc64_timer_init, NULL, &sc64_timer_event_device,
    &sc64_timer_counter_device);
