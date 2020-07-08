/**
 * @file
 *
 * @brief RISC-V build-in timer
 *
 * @date 12.12.2019
 * @author Anastasia Nizharadze
 */

#include <errno.h>

#include <asm/regs.h>
#include <asm/interrupts.h>

#include <hal/clock.h>
#include <hal/system.h>
#include <hal/reg.h>
#include <kernel/irq.h>
#include <kernel/time/clock_source.h>

#include <embox/unit.h>

EMBOX_UNIT_INIT(riscv_clock_init);

#define HZ 1000
#define COUNT_OFFSET (RTC_CLOCK / HZ)
#define RTC_CLOCK    OPTION_GET(NUMBER, rtc_freq)

#define MTIME        OPTION_GET(NUMBER, base_mtime)
#define MTIMECMP     OPTION_GET(NUMBER, base_mtimecmp)

static int clock_handler(unsigned int irq_nr, void *dev_id) {
	REG64_STORE(MTIMECMP, REG64_LOAD(MTIME) + COUNT_OFFSET);

	clock_tick_handler(dev_id);

	return IRQ_HANDLED;
}

static int riscv_clock_setup(struct time_dev_conf * conf) {
	REG64_STORE(MTIMECMP, REG64_LOAD(MTIME) + COUNT_OFFSET);
	enable_timer_interrupts();

	return ENOERR;
}

static struct time_event_device riscv_event_device  = {
	.config = riscv_clock_setup,
	.event_hz = HZ,
	.name = "riscv_clk",
	.irq_nr = MACHINE_TIMER_INTERRUPT
};

static struct clock_source riscv_clock_source = {
	.name = "riscv_clk",
	.event_device = &riscv_event_device,
	.read = clock_source_read
};

static int riscv_clock_init(void) {
	int err;

	err = clock_source_register(&riscv_clock_source);
	if (err) {
		return err;
	}

	return 0;
}

RISCV_TIMER_IRQ_DEF(clock_handler, &riscv_clock_source);
