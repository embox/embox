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
#include <asm/msr.h>
extern int blin_timer;
/* we havn't interrupts acknowledgment in microblaze architecture
 * and must receive interrupt number our self and then clear pending bit in
 * pending register
 */
void interrupt_handler(void) {
	__interrupt_mask_t irq_stat;

	while (0 != (irq_stat = interrupt_get_status())) {
		interrupt_nr_t irq_num;

		for (irq_num = 0; irq_num < INTERRUPT_NRS_TOTAL; irq_num++) {
			if (irq_stat & (1 << irq_num)) {
				//TODO we must clear whole pending register
				interrupt_clear(irq_num);

				/*now we allow nested irq*/
				msr_set_bit(MSR_IE_BIT);
#ifdef CONFIG_IRQ
				irq_dispatch(irq_num);
#endif
			}
		}
	}
}
