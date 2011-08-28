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

typedef unsigned char __interrupt_nr_t;
typedef unsigned int __interrupt_mask_t;

/**
 * return isr register for mb_intc
 */
extern __interrupt_mask_t interrupt_get_status(void);

extern void irqc_set_mask(__interrupt_mask_t mask);
extern __interrupt_mask_t irqc_get_mask(void);

#endif /*__ASSEMBLER__*/

#endif /* HAL_INTERRUPT_MB_INTC_H_ */
