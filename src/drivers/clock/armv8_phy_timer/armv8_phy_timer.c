/**
 * @file
 *
 * @date Nov 24, 2020
 * @author Anton Bondarev
 */
#include <errno.h>
#include <stddef.h>
#include <stdint.h>

#include <drivers/common/memory.h>
#include <framework/mod/options.h>
#include <hal/clock.h>
#include <hal/reg.h>
#include <kernel/irq.h>
#include <kernel/time/clock_source.h>
#include <kernel/time/time_device.h>

#define IRQ_NUM OPTION_GET(NUMBER, irq_num)

static irq_return_t phy_timer_handler(unsigned int irq_nr, void *dev_id) {
	uint32_t freq;

	freq = ARCH_REG_LOAD(CNTFRQ_EL0);

	ARCH_REG_STORE(CNTP_TVAL_EL0, freq / 1000);
	clock_tick_handler(dev_id);
	return IRQ_HANDLED;
}

static int phy_timer_set_periodic(struct clock_source *cs) {
	uint32_t freq;

	freq = ARCH_REG_LOAD(CNTFRQ_EL0);

	ARCH_REG_STORE(CNTP_TVAL_EL0, freq / 1000);
	ARCH_REG_STORE(CNTP_CTL_EL0, CNTP_CTL_EL0_EN);

	return ENOERR;
}

static struct time_event_device phy_timer_event_device = {
    .set_periodic = phy_timer_set_periodic,
    .name = "armv8_phy_timer",
    .irq_nr = IRQ_NUM};

static int phy_timer_init(struct clock_source *cs) {
	return irq_attach(IRQ_NUM, phy_timer_handler, 0, cs, "armv8_phy_timer");
}

CLOCK_SOURCE_DEF(armv8_phy_timer, phy_timer_init, NULL, &phy_timer_event_device,
    NULL);
