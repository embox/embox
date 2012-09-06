/**
 * @file
 * @brief Microblaze interrupt controller HAL definitions.
 *
 * @date 23.11.09
 * @author Anton Bondarev
 */

#ifndef HAL_INTERRUPT_MB_INTC_H_
#define HAL_INTERRUPT_MB_INTC_H_

#define __INTERRUPT_NRS_TOTAL 16

#ifndef __ASSEMBLER__

typedef unsigned int __interrupt_mask_t;

/**
 * return ISR register for mb_intc.
 */
extern unsigned int mb_intc_get_pending(void);

extern void irqc_set_mask(__interrupt_mask_t mask);
extern __interrupt_mask_t irqc_get_mask(void);

#endif /*__ASSEMBLER__*/

#endif /* HAL_INTERRUPT_MB_INTC_H_ */
