/**
 * \file irq_ctrl.c
 *
 * \date Apr 14, 2009
 * \uthor: anton
 * \brif Realize hal layer interupt ctrl for leon 3 processor
 */
#include "types.h"
#include "common.h"
#include "irq_ctrl.h"
#include "plug_and_play.h"
#include "pnp_id.h"



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


static IRQ_REGS * dev_regs = NULL;

static AMBA_DEV amba_dev;

/**
 * init hardware irq ctrl
 * Caprute APB dev
 * init pointer to struct regs
 * @return 0 if success
 * @return -1 another way
 */
int irq_ctrl_init(){
	TRY_CAPTURE_APB_DEV (&amba_dev, VENDOR_ID_GAISLER, DEV_ID_GAISLER_INTERRUPT_UNIT);

	amba_dev.show_info = NULL;//show_module_info;
	dev_regs = (IRQ_REGS *)amba_dev.bar[0].start;

	dev_regs->level = 0;
	dev_regs->mask = 0;
	dev_regs->pend = 0;
	dev_regs->force = 0;
	dev_regs->clear = 0xFFFFFFFF;
	return 0;
}

int irq_ctrl_enable_irq(int irq_num){
	if (NULL == dev_regs)
		irq_ctrl_init();
	SetBit(dev_regs->mask, irq_num);
	return 0;
}

int irq_ctrl_disable_irq(int irq_num){
	if (NULL == dev_regs)
		irq_ctrl_init();
	ClearBit(dev_regs->mask, irq_num);
	return 0;
}
