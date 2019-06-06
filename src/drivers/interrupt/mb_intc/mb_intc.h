/**
 * @file
 * @brief Microblaze interrupt controller HAL definitions.
 *
 * @date 23.11.09
 * @author Anton Bondarev
 */

#ifndef IRQCTRL_MB_INTC_IMPL_H_
#define IRQCTRL_MB_INTC_IMPL_H_

#define __IRQCTRL_IRQS_TOTAL 16

#ifndef __ASSEMBLER__

/**
 * return ISR register for mb_intc.
 */
extern unsigned int mb_intc_get_pending(void);

#endif /*__ASSEMBLER__*/

#endif /* IRQCTRL_MB_INTC_IMPL_H_ */
