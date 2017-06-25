/**
 * @file
 * @brief PowerPC Microprocessor Family clock device driver
 *
 * @date 06.11.12
 * @author Ilia Vaprol
 */

#include <asm/psr.h>
#include <asm/regs.h>
#include <embox/unit.h>
#include <hal/clock.h>
#include <hal/system.h>
#include <kernel/irq.h>
#include <kernel/time/clock_source.h>
#include <stdint.h>


EMBOX_UNIT_INIT(ppc_clk_init);

/** Test for different frequancy (300K ticks -- i.e. 5min)
 * 300MHz is 00:04:10.356 (300K)
 * 333MHz is 00:04:37.672 (300K)
 * 350MHz is 00:05:22.747 (300K)
 * 366MHz is 00:05:39.987 (300K)
 * 375MHz is 00:06:27.322 (343K)
 * We will use 333MHz as default settings
 */
#define PPCCLK_IRQ  10
#define PPCCLK_FREQ SYS_CLOCK
#define PPCCLK_DECR (PPCCLK_FREQ / 1000) /* precision for ms */

static irq_return_t clock_handler(unsigned int irq_nr, void *data) {
	__set_tsr(__get_tsr() & ~TSR_DIS);
	clock_tick_handler(irq_nr, data);
	return IRQ_HANDLED;
}

static int ppc_clk_config(struct time_dev_conf *conf) {
	__set_dec(PPCCLK_DECR);
	__set_decar(PPCCLK_DECR);
    __set_tcr(TCR_DIE | TCR_ARE);
	return 0;
}

static cycle_t ppc_clk_read(void) {
	uint32_t l, u, tmp;
	tmp = __get_tbu();
	do {
		u = tmp;
		l = __get_tbl();
		tmp = __get_tbu();
	} while (tmp != u);
	return ((uint64_t)u << 32) | l;
}

static struct time_event_device ppc_clk_event = {
	.config = ppc_clk_config,
	.event_hz = PPCCLK_FREQ / PPCCLK_DECR,
	.irq_nr = PPCCLK_IRQ,
};

static struct time_counter_device ppc_clk_counter = {
	.read = ppc_clk_read,
	.cycle_hz = PPCCLK_FREQ,
};

static struct clock_source ppc_clk_clock_source = {
	.name = "ppc_clk",
	.event_device = &ppc_clk_event,
	.counter_device = &ppc_clk_counter,
	.read = clock_source_read,
};

static int ppc_clk_init(void) {
	clock_source_register(&ppc_clk_clock_source);
	return irq_attach(PPCCLK_IRQ, clock_handler, 0, &ppc_clk_clock_source, "ppc_clk");
}
