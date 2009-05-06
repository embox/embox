/**
 * \file irq_ctrl.h
 *
 * \date Apr 14, 2009
 * \author anton
 */

#ifndef IRQ_CTRL_H_
#define IRQ_CTRL_H_

#ifdef LEON2
typedef struct _IRQ_REGS
{
	volatile unsigned short level;
	volatile unsigned short mask;
	volatile unsigned int pend;
	volatile unsigned int force;
	volatile unsigned int clear;
}IRQ_REGS;
#endif

#ifdef LEON3
typedef struct _IRQ_REGS {
	volatile unsigned int level; /* 0x00 */
	volatile unsigned int pend; /* 0x04 */
	volatile unsigned int force; /* 0x08 */
	volatile unsigned int clear; /* 0x0C */
	volatile unsigned int mpstatus; /* 0x10 */
	volatile unsigned int dummy[11]; /* 0x14 - 0x3C */
	volatile unsigned int mask; /* 0x40 */
} IRQ_REGS;
#endif

#endif /* IRQ_CTRL_H_ */
