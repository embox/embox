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
#include <hal/clock.h>
#include <hal/system.h>
#include <hal/reg.h>
#include <kernel/irq.h>
#include <kernel/time/clock_source.h>

#include <embox/unit.h>

#define HZ 100
#define COUNT_OFFSET (RTC_CLOCK / HZ)
#define RTC_CLOCK    OPTION_GET(NUMBER, rtc_freq)
#define MTIME        OPTION_GET(NUMBER, base_mtime)
#define MTIMECMP     OPTION_GET(NUMBER, base_mtimecmp)

static irq_return_t clock_handler(unsigned int irq_nr, void *dev_id) {
	uint64_t *mtime = (uint64_t *)MTIME;
	uint64_t *mtimecmp = (uint64_t *)MTIMECMP;
	*mtimecmp = *(uint64_t *)mtime + (uint64_t)COUNT_OFFSET;

	clock_tick_handler(irq_nr, dev_id);
	return IRQ_HANDLED;
}

static int riscv_clock_setup(struct time_dev_conf * conf) {
	uint64_t *mtime = (uint64_t *)MTIME;
	uint64_t *mtimecmp = (uint64_t *)MTIMECMP;
	*mtimecmp = *(uint64_t *)mtime + (uint64_t)COUNT_OFFSET;

	return ENOERR;
}

static struct time_event_device riscv_event_device  = {
	.config = riscv_clock_setup,
	.event_hz = 1000,
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

	err = irq_attach(MACHINE_TIMER_INTERRUPT, clock_handler, 0, &riscv_clock_source, "riscv_clk");
	if (err) {
		return err;
	}

	return 0;
}

EMBOX_UNIT_INIT(riscv_clock_init);
