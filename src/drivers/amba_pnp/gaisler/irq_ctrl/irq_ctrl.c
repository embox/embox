/**
 * \file irq_ctrl.c
 *
 * \date Apr 14, 2009
 * \author anton
 * \brief Realize hal layer interupt ctrl for leon 3 processor
 */
#include "types.h"
#include "common.h"
#include "irq_ctrl.h"
#include "amba_pnp.h"

static IRQ_REGS * dev_regs = NULL;

#include "amba_drivers_helper.h"
#undef module_init
#define module_init() irq_ctrl_init()

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
	if (INTERRUPT_BASE != amba_dev.bar[0].start) {
        TRACE("irq_controller base is %x instead of correct value %x\n",
                amba_dev.bar[0].start, INTERRUPT_BASE);
        return -1;
    }
	dev_regs = (IRQ_REGS *)amba_dev.bar[0].start;

	dev_regs->level = 0;
	dev_regs->mask = 0;
	dev_regs->pend = 0;
	dev_regs->force = 0;
	dev_regs->clear = 0xFFFFFFFF;
	return 0;
}

int irq_ctrl_enable_irq(int irq_num){
	CHECK_INIT_MODULE();
	SetBit(dev_regs->mask, irq_num);
	return 0;
}

int irq_ctrl_disable_irq(int irq_num){
	CHECK_INIT_MODULE();
	ClearBit(dev_regs->mask, irq_num);
	return 0;
}

