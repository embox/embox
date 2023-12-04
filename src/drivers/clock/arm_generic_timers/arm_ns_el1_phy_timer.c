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
#include <hal/system.h>
#include <kernel/irq.h>
#include <kernel/time/clock_source.h>
#include <kernel/time/time_device.h>

#define ARM_NS_EL1_PHY_TIMER_IRQN OPTION_GET(NUMBER, irq_num)

static inline void el1_phy_timer_set_tval(uint32_t val) {
	asm volatile("msr cntp_tval_el0, %0" : : "r"(val));
}

static irq_return_t el1_phy_timer_handler(unsigned int irq_nr, void *dev_id) {
	uint32_t freq;

	freq = ARCH_REG_LOAD(CNTFRQ_EL0);

	ARCH_REG_STORE(CNTP_TVAL_EL0, freq / 1000);
	clock_tick_handler(dev_id);
	return IRQ_HANDLED;
}

static int el1_phy_timer_set_periodic(struct clock_source *cs) {
	uint32_t freq;

	freq = ARCH_REG_LOAD(CNTFRQ_EL0);

	ARCH_REG_STORE(CNTP_TVAL_EL0, freq / 1000);
	ARCH_REG_STORE(CNTP_CTL_EL0, CNTP_CTL_EL0_EN);

	return ENOERR;
}

static struct time_event_device el1_phy_timer_event_device = {
    .set_periodic = el1_phy_timer_set_periodic,
    .name = "el1_phy_timer",
    .irq_nr = ARM_NS_EL1_PHY_TIMER_IRQN};

static int el1_phy_timer_init(struct clock_source *cs) {
	return irq_attach(ARM_NS_EL1_PHY_TIMER_IRQN, el1_phy_timer_handler, 0, cs,
	    "ARM NS EL1 generic timer");
}

CLOCK_SOURCE_DEF(arm_el1_phy_timer, el1_phy_timer_init, NULL,
    &el1_phy_timer_event_device, NULL);
