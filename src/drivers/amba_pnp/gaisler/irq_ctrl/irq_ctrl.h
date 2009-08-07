/**
 * \file irq_ctrl.h
 *
 * \date Apr 14, 2009
 * \author anton
 */

#ifndef IRQ_CTRL_H_
#define IRQ_CTRL_H_

typedef struct _IRQ_REGS {
	volatile unsigned int level;     /* 0x00 */
	volatile unsigned int pend;      /* 0x04 */
	volatile unsigned int force;     /* 0x08 */
	volatile unsigned int clear;     /* 0x0C */
	volatile unsigned int mpstatus;  /* 0x10 */
	volatile unsigned int dummy[11]; /* 0x14 - 0x3C */
	volatile unsigned int mask;      /* 0x40 */
} IRQ_REGS;

#endif /* IRQ_CTRL_H_ */
