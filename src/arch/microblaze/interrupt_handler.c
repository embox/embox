/**
 * @file interrupt_handler.c
 *
 * @brief This file contains @link interrupt_handler() @endlink function. It's proxy between
 *        asm code and kernel interrupt handler @link irq_dispatch() @endlink
 *        function.
 *
 * @date 27.11.2009
 * @author Anton Bondarev
 */
#include "autoconf.h"
#include "hal/irq_ctrl.h"
#include "kernel/irq.h"

void interrupt_handler() {
	irq_mask_t irq_stat;
	while (0 != (irq_stat = irqc_get_isr_reg())) {
		int irq_num;
		for (irq_num = 0; irq_num < MAX_IRQ_NUMBER; irq_num++) {
			if (irq_stat & (1 << irq_num)) {
				irqc_clear(irq_num);
				irq_dispatch(irq_num);
			}
		}
	}
}
