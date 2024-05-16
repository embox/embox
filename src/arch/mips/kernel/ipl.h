/**
 * @file
 *
 * @brief
 *
 * @date 22.06.2012
 * @author Anton Bondarev
 */

#ifndef MIPS_ASM_IPL_H_
#define MIPS_ASM_IPL_H_

#include <asm/mipsregs.h>

typedef unsigned int __ipl_t;

static inline void ipl_init(void) {
	unsigned int c0_reg;

	/* read status registers for cleaning interrupts mask */
	c0_reg = mips_read_c0_status();
	// c0_reg &= ~(ST0_IM);          /* clear all interrupts mask */
	c0_reg |= ST0_IE;             /* global enable interrupt */
	mips_write_c0_status(c0_reg); /* write back status register */

	/* read cause register for cleaning all pending bits */
	// c0_reg = mips_read_c0_cause();
	// c0_reg &= ~(ST0_IM);           /* clear all interrupts pending bits */
	// mips_write_c0_cause(c0_reg);   /* write back cause register */
}

static inline __ipl_t ipl_save(void) {
	unsigned int c0_reg;
	/* read mips status register */
	c0_reg = mips_read_c0_status();
	/* write back status register with cleaned irq mask*/
	mips_write_c0_status(c0_reg & ~(ST0_IE));

	/* return previous interrupt mask */
	return c0_reg;
}

static inline void ipl_restore(__ipl_t ipl) {
	unsigned int c0_reg;

	/* read status registers for cleaning interrupts mask */
	c0_reg = mips_read_c0_status();

	if (ipl & ST0_IE) {
		mips_write_c0_status(c0_reg | ST0_IE);
	}
	else {
		mips_write_c0_status(c0_reg & ~ST0_IE);
	}
}

#endif /* MIPS_ASM_IPL_H_ */
