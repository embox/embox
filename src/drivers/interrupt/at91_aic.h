/**
 * @file
 * @brief Advanced Interrupt Controller driver definitions.
 *
 * @date 11.07.10
 * @author Anton Kozlov
 */

#ifndef HAL_INTERRUPT_AT91_AIC_H_
#define HAL_INTERRUPT_AT91_AIC_H_

#define __INTERRUPT_NRS_TOTAL 32

#ifndef __ASSEMBLER__

typedef unsigned char __interrupt_nr_t;
typedef unsigned long __interrupt_mask_t;

#endif /*__ASSEMBLER__*/

#endif /* HAL_INTERRUPT_AT91_AIC_H_ */
