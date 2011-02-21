/**
 * @file
 *
 * @brief Implements interrupt process level for microblaze architecture
 *
 * @details Microblaze hasn't ipl level on chip. We use interrupt controller
 * mask/unmask operations for implementing ipl. While initialize we enable bit
 * @link #MSR_IE_BIT @endlink for interrupts enable in MSR register.
 *
 * @date 13.03.10
 * @author Alexey Fomin
 */

#ifndef HAL_IPL_H_
# error "Do not include this file directly!"
#endif /* HAL_IPL_H_ */

#include <types.h>
#include <asm/msr.h>
#include <hal/interrupt.h>

typedef uint32_t __ipl_t;

inline static void ipl_init(void) {
	msr_set_bit(MSR_IE_BIT);
}
#include "stdio.h"
inline static __ipl_t ipl_save(void) {
#if 0
	/* it will be better to use irq_ctrl mask but we can't expect every
	 * irq_ctrl driver has implemented own get_mask and set mask functions
	 */
	__ipl_t ipl_status = irqc_get_mask();
	irqc_set_mask(0);

	return ipl_status;
#endif

	__ipl_t ipl = msr_get_bit(MSR_IE_BIT);

	msr_clr_bit(MSR_IE_BIT);

	//prom_printf("get ipl = 0x%X\n", ipl);
	return ipl;
}

inline static void ipl_restore(__ipl_t ipl) {
#if 0
	/* it will be better to use irq_ctrl mask but we can't expect every
	 * irq_ctrl driver has implemented own get_mask and set mask functions
	 */
	irqc_set_mask(ipl);
#endif
	//printf ("ipl = 0x%X\n", ipl);
	//prom_printf("set ipl = 0x%X\n", ipl);
	ipl ? msr_set_bit(MSR_IE_BIT) : msr_clr_bit(MSR_IE_BIT);


	//msr_set_value(msr_get_value() | ipl);
}
