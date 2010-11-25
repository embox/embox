/**
 * @file
 * @brief Gaisler Research IRQMP Interrupt controller driver.
 *
 * @date 14.04.2009
 * @author Anton Bondarev
 * @author Eldar Abusalimov
 */

#include <assert.h>
#include <errno.h>
#include <types.h>

#include <kernel/panic.h>
#include <kernel/printk.h>
#include <kernel/irq.h>
#include <hal/interrupt.h>
#include <hal/reg.h>
#include <drivers/amba_pnp.h>

struct irqmp_regs {
	/* 0x00 */uint32_t level;
	/* 0x04 */uint32_t pending;
	/* 0x08 */uint32_t force;
	/* 0x0C */uint32_t clear;
	/* 0x10 */uint32_t mpstatus;
	/* 0x14 */uint32_t broadcast;
	/* 0x18 */uint32_t dummy[10];
	/* 0x40 */uint32_t mask;
};

static volatile struct irqmp_regs *dev_regs;

static int dev_regs_init(void);

void interrupt_enable(interrupt_nr_t interrupt_nr) {
	assert(interrupt_nr_valid(interrupt_nr));
	assert(NULL != dev_regs);
	REG_ORIN(&dev_regs->mask, 1 << interrupt_nr);
}

void interrupt_disable(interrupt_nr_t interrupt_nr) {
	assert(interrupt_nr_valid(interrupt_nr));
	assert(NULL != dev_regs);
	REG_ANDIN(&dev_regs->mask, ~(1 << interrupt_nr));
}

void interrupt_clear(interrupt_nr_t interrupt_nr) {
	assert(interrupt_nr_valid(interrupt_nr));
	assert(NULL != dev_regs);
	REG_ORIN(&dev_regs->clear, 1 << interrupt_nr);
	REG_ANDIN(&dev_regs->force, ~(1 << interrupt_nr));
}

void interrupt_force(interrupt_nr_t interrupt_nr) {
	assert(interrupt_nr_valid(interrupt_nr));
	assert(NULL != dev_regs);
	REG_ORIN(&dev_regs->force, 1 << interrupt_nr);
}

void interrupt_init(void) {
	assert(NULL == dev_regs);

	if (0 != dev_regs_init()) {
		panic("Unable to initialize irqmp dev_regs");
	}
	assert(NULL != dev_regs);

	REG_STORE(&dev_regs->level, 0x0);
	REG_STORE(&dev_regs->mask, 0x0);
	REG_STORE(&dev_regs->pending, 0x0);
	REG_STORE(&dev_regs->force, 0x0);
	REG_STORE(&dev_regs->clear, ~0x0);
}

interrupt_mask_t interrupt_get_status(void) {
	return (REG_LOAD(&dev_regs->pending)) | REG_LOAD(&dev_regs->force);
}

#ifdef CONFIG_AMBAPP
static int dev_regs_init(void) {
	amba_dev_t amba_dev;
	if (-1 == capture_amba_dev(&amba_dev, CONFIG_VENDOR_ID_GAISLER,
			CONFIG_DEV_ID_GAISLER_INTERRUPT_UNIT, false, false)) {
		printk("can't capture apb dev venID=0x%X, devID=0x%X\n",
			CONFIG_VENDOR_ID_GAISLER,
			CONFIG_DEV_ID_GAISLER_INTERRUPT_UNIT);
		return -ENODEV;
	}
	dev_regs = (volatile struct irqmp_regs *) amba_dev.bar[0].start;
	return 0;
}
#elif defined(CONFIG_IRQMP_BASE)
static int dev_regs_init(void) {
	dev_regs = (volatile struct irqmp_regs *) CONFIG_IRQMP_BASE;
	return 0;
}
#else
# error "Either CONFIG_AMBAPP or CONFIG_IRQMP_BASE must be defined"
#endif /* CONFIG_AMBAPP */
