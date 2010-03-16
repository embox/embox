/**
 * @file
 *
 * @details This file contains @link interrupt_handler() @endlink function.
 *        It's proxy between asm code and kernel interrupt handler
 *        @link irq_dispatch() @endlink function.
 *
 * @date 27.11.2009
 * @author Anton Bondarev
 */

#include <kernel/irq.h>
#include <hal/interrupt.h>

void interrupt_handler(void) {
	__interrupt_mask_t irq_stat;

	while (0 != (irq_stat = irqc_get_isr_reg())) {
		interrupt_nr_t irq_num;
		for (irq_num = 0; irq_num < INTERRUPT_NRS_TOTAL; irq_num++) {
			if (irq_stat & (1 << irq_num)) {
				interrupt_clear(irq_num);
#ifdef CONFIG_IRQ
				irq_dispatch(irq_num);
#endif
			}
		}
	}

}
