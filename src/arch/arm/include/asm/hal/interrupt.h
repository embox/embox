#ifndef HAL_INTERRUPT_H_
# error "Do not include this file directly!"
#endif /* HAL_INTERRUPT_H_ */

#define __INTERRUPT_NRS_TOTAL 32

#ifndef __ASSEMBLER__

typedef unsigned char __interrupt_nr_t;
typedef unsigned long __interrupt_mask_t;

#endif /*__ASSEMBLER__*/
