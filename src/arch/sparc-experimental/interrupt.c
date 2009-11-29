/**
 * @file interrupt.c
 *
 * @brief Gaisler Research IRQMP Interrupt controller driver.
 *
 * @date 14.04.2009
 * @author Anton Bondarev
 * @author Eldar Abusalimov
 */

#include <assert.h>
#include <types.h>

#include <hal/interrupt.h>
#include <hal/reg.h>

#ifdef CONFIG_AMBAPP
#include <drivers/amba_pnp.h>
#endif /* CONFIG_AMBAPP */

typedef struct irq_regs {
	uint32_t level; /* 0x00 */
	uint32_t pend; /* 0x04 */
	uint32_t force; /* 0x08 */
	uint32_t clear; /* 0x0C */
	uint32_t mpstatus; /* 0x10 */
	uint32_t dummy[11]; /* 0x14 - 0x3C */
	uint32_t mask; /* 0x40 */
} irq_regs_t;

static volatile irq_regs_t *dev_regs;

#ifdef CONFIG_AMBAPP
static AMBA_DEV amba_dev;
#endif /* CONFIG_AMBAPP */

void interrupt_init() {
	assert_null(dev_regs);

#ifdef CONFIG_AMBAPP
	if (-1 == capture_amba_dev(dev, venID, devID, FALSE, FALSE)){
		LOG_ERROR("can't capture apb dev venID=0x%X, devID=0x%X\n", venID, devID);
		return -1;
	}
#else

#endif /* CONFIG_AMBAPP */

	REG_STORE(dev_regs->level, 0);
	REG_STORE(dev_regs->mask, 0);
	REG_STORE(dev_regs->pend, 0);
	REG_STORE(dev_regs->force, 0);
	REG_STORE(dev_regs->clear, 0xFFFFFFFF);
}

void interrupt_enable(interrupt_nr_t interrupt_nr);

void interrupt_disable(interrupt_nr_t interrupt_nr);

void interrupt_clear(interrupt_nr_t interrupt_nr);

void interrupt_force(interrupt_nr_t interrupt_nr);
