/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    08.03.2013
 */

#include <embox/unit.h>
#include <hal/clock.h>
#include <hal/system.h>
#include <kernel/irq.h>
#include <kernel/time/clock_source.h>
#include <kernel/host.h>

EMBOX_UNIT_INIT(umclock_init);

#define CLOCK_IRQ  2

static irq_return_t clock_handler(unsigned int irq_nr, void *data) {
	unsigned long long ovrn_count;

	emvisor_recvnbody(UV_PRDDOWNSTRM, &ovrn_count, sizeof(ovrn_count));

	while (ovrn_count--) {
		clock_tick_handler(irq_nr, data);
	}

	return IRQ_HANDLED;
}

static int ppc_clk_config(struct time_dev_conf *conf);

static cycle_t ppc_clk_read(void) {
	return 0;
}

static struct time_event_device umclock_ev = {
	.config = ppc_clk_config,
	.resolution = 1000,
	.irq_nr = CLOCK_IRQ,
};

static struct time_counter_device umclock_cd = {
	.read = ppc_clk_read,
	.resolution = 1000000,
};

static struct clock_source umclock_cs = {
	.name = "usermode clock",
	.event_device = &umclock_ev,
	.counter_device = &umclock_cd,
	.read = clock_source_read,
};

static int ppc_clk_config(struct time_dev_conf *conf) {
	struct emvisor_tmrset ts = {
		.count_fq = umclock_cd.resolution,
		.overfl_fq = umclock_ev.resolution
	};


	emvisor_send(UV_PWRUPSTRM, EMVISOR_TIMER_SET, &ts, sizeof(ts));

	return 0;
}
static int umclock_init(void) {
	clock_source_register(&umclock_cs);
	return irq_attach(CLOCK_IRQ, clock_handler, 0, &umclock_cs, "ppc_clk");
}
