#ifndef HAL_INTERRUPT_H_
# error "Do not include this file directly!"
#endif /* HAL_INTERRUPT_H_ */

#define __INTERRUPT_NRS_TOTAL 16

#ifndef __ASSEMBLER__

typedef unsigned char __interrupt_nr_t;
typedef unsigned int __interrupt_mask_t;

/**
 * return isr register for mb_intc
 */
extern __interrupt_mask_t irqc_get_isr_reg(void);

extern void irqc_set_mask(__interrupt_mask_t mask);
extern __interrupt_mask_t irqc_get_mask(void);

#endif /*__ASSEMBLER__*/
