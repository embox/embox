/**
 * @file
 *
 * @date Nov 24, 2020
 * @author Anton Bondarev
 */

#include <util/log.h>

#include <sys/mman.h>

#include <drivers/common/memory.h>
#include <hal/clock.h>
#include <hal/reg.h>
#include <hal/system.h>

#include <kernel/irq.h>
#include <kernel/time/clock_source.h>

#include <framework/mod/options.h>

#define ARM_NS_EL1_PHY_TIMER_IRQN   OPTION_GET(NUMBER, irq_num)

#define CNTP_CTL_ENABLE  (1 << 0)
#define CNTP_CTL_MASK    (1 << 1)
#define CNTP_CTL_STATUS  (1 << 2)

static inline uint32_t el1_phy_timer_get_tval(void) {
	uint32_t val;
	asm volatile("mrs %0, cntp_tval_el0" : "=r" (val));
	return val;
}

static inline void el1_phy_timer_set_tval(uint32_t val) {
	asm volatile("msr cntp_tval_el0, %0" : : "r" (val));
}

static inline uint32_t el1_phy_timer_get_ctrl(void) {
	uint32_t val;
	asm volatile("mrs cntp_ctl_el0, %0" : : "r" (val));
	return val;
}

static inline void el1_phy_timer_set_ctrl(uint32_t val) {
	asm volatile("msr cntp_ctl_el0, %0" : : "r" (val));
}

static inline uint32_t arch_timer_get_cntfrq(void)
{
	uint32_t val;
	asm volatile("mrs %0,   cntfrq_el0" : "=r" (val));
	return val;
}

static irq_return_t el1_phy_timer_handler(unsigned int irq_nr, void *dev_id) {
	uint32_t freq;

	freq = arch_timer_get_cntfrq();

	el1_phy_timer_set_tval(freq / 1000);
	clock_tick_handler(dev_id);
	return IRQ_HANDLED;
}

static int el1_phy_timer_set_periodic(struct clock_source *cs) {
	uint32_t freq;

	freq = arch_timer_get_cntfrq();

	el1_phy_timer_set_tval(freq / 1000);
	el1_phy_timer_set_ctrl(CNTP_CTL_ENABLE);

	return ENOERR;
}

static struct time_event_device el1_phy_timer_event_device  = {
	.set_periodic = el1_phy_timer_set_periodic,
	.name = "el1_phy_timer",
	.irq_nr = ARM_NS_EL1_PHY_TIMER_IRQN
};

static int el1_phy_timer_init(struct clock_source *cs) {

	return irq_attach(ARM_NS_EL1_PHY_TIMER_IRQN, el1_phy_timer_handler, 0, cs, "ARM NS EL1 generic timer");
}

CLOCK_SOURCE_DEF(arm_el1_phy_timer, el1_phy_timer_init, NULL,
	&el1_phy_timer_event_device, NULL);
