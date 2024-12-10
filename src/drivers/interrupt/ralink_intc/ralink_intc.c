/**
 * @file
 *
 * @date Dec 8, 2024
 * @author Serge Vasilugin
 */

#include <assert.h>
#include <stdint.h>

#include <asm/io.h>
#include <asm/mipsregs.h>
#include <framework/mod/options.h>
#include <hal/reg.h>
#include <kernel/irq.h>
#include <util/log.h>


#define READREG(r)		*(volatile unsigned int *)(r)
#define WRITEREG(r,v)		*(volatile unsigned int *)(r) = v

#define KSEG1ADDR(_x)		(((_x) & 0x1fffffff) | 0xa0000000)

#define INTC_BASE		0xb0000200

#define	INTC_READ(reg)		READREG(INTC_BASE + (reg))
#define	INTC_WRITE(reg, v)	WRITEREG(INTC_BASE + (reg), (v))

#define	RALINK_INTC_INTERRUPT_PIN	2
#define RALINK_INTC_IRQ_TOTAL		32
#define RALINK_INTC_IRQ_BASE		8

#define INTC_REG_STATUS0	0
#define INTC_REG_STATUS1	4
#define INTC_REG_TYPE		0x20
#define INTC_REG_RAW_STATUS	0x30
#define INTC_REG_ENABLE		0x34
#define INTC_REG_DISABLE	0x38

#define INTC_INT_GLOBAL		(1U<<31)

static int ralink_intc_init(void) {
	uint32_t c0;

	/* Init MIPS INTC */
	c0 = mips_read_c0_status();
	c0 &= ~(ST0_IM);
	mips_write_c0_status(c0);

	c0 = mips_read_c0_cause();
	c0 &= ~(ST0_IM);
	mips_write_c0_cause(c0);

	/* Init ralink INTC */
	/* disable all interrupts */
	INTC_WRITE(INTC_REG_DISABLE, ~0U);

	if(RALINK_INTC_INTERRUPT_PIN == 2) {
		/* route all INTC interrupts to MIPS HW0 interrupt */
		INTC_WRITE(INTC_REG_TYPE, 0);
	} else {
		/* route all INTC interrupts to MIPS HW1 interrupt */
		INTC_WRITE(INTC_REG_TYPE, ~0U);
	}

	INTC_WRITE(INTC_REG_ENABLE, INTC_INT_GLOBAL);

	log_info("config: base=0x%x, connected to mips intc HW%d\n", INTC_BASE, RALINK_INTC_INTERRUPT_PIN - 2 );

	return 0;
}

void irqctrl_enable(unsigned int irq) {
	uint32_t mask;
	uint32_t c0;

	assert(irq_nr_valid(irq));

	if (irq < RALINK_INTC_IRQ_BASE) {
		c0 = mips_read_c0_status();
		c0 |= 1 << (irq + ST0_IRQ_MASK_OFFSET);
		mips_write_c0_status(c0);
	}
	else {
		irq -= RALINK_INTC_IRQ_BASE;
		mask = (1 << (irq));
		INTC_WRITE(INTC_REG_ENABLE, mask);
		/* XXX */
		c0 = mips_read_c0_status();
		c0 |= 1 << (RALINK_INTC_INTERRUPT_PIN + ST0_IRQ_MASK_OFFSET);
		mips_write_c0_status(c0);
	}
}

void irqctrl_disable(unsigned int irq) {
	uint32_t mask;
	uint32_t c0;

	assert(irq_nr_valid(irq));

	if (irq < RALINK_INTC_IRQ_BASE) {
		c0 = mips_read_c0_status();
		c0 &= ~(1 << (irq + ST0_IRQ_MASK_OFFSET));
		mips_write_c0_status(c0);
	}
	else {
		irq -= RALINK_INTC_IRQ_BASE;
		mask = (1 << (irq));
		INTC_WRITE(INTC_REG_DISABLE, mask);
	}
}

void irqctrl_force(unsigned int irq) {
}

int irqctrl_pending(unsigned int irq) {
	return 0;
}

void irqctrl_eoi(unsigned int irq) {
	irqctrl_enable(irq);
}

unsigned int irqctrl_get_intid(void) {
	uint32_t pending;
	int i, j;

	pending = (mips_read_c0_cause() & mips_read_c0_status() & CAUSE_IM) >> ST0_IRQ_MASK_OFFSET;

	for ( i = RALINK_INTC_IRQ_BASE - 1; i >= 0; i--) {
		if (pending & (1U << i)) {
		    if (i == RALINK_INTC_INTERRUPT_PIN) break; /* jump to ralink intc */
		    return i;
		}
	}

	if (RALINK_INTC_INTERRUPT_PIN == 2)
		pending = INTC_READ(INTC_REG_STATUS0);
	else
		pending = INTC_READ(INTC_REG_STATUS1);

	if (!pending) {
		return -1;
	}

	/* skip INTC_INTC_GLOBAL - it is always on if RALINK_INTC_INTERRUPT_PIN arise */
	for (j = RALINK_INTC_IRQ_TOTAL - 2; j >= 0; j--) {
		if (pending & (1U << j)) {
			return RALINK_INTC_IRQ_BASE + j;
		}
	}

	return -1;
}

IRQCTRL_DEF(ralink_intc, ralink_intc_init);

//#include <drivers/common/memory.h>
//PERIPH_MEMORY_DEFINE(ralink_intc, INTC_BASE, 0x100);
