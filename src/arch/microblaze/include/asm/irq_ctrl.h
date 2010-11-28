/**
 * @file
 * @brief This file defines microblaze interrupt controller specific settings
 *
 * @date Apr 14, 2009
 * @author Anton Bondarev
 */

#ifndef MICROBLAZE_IRQ_CTRL_H_
#define MICROBLAZE_IRQ_CTRL_H_

#ifndef __ASSEMBLER__

typedef char         irq_num_t;
typedef unsigned int irq_mask_t;

/**
 * return isr register for mb_intc
 */
extern irq_mask_t interrupt_get_status(void);

#endif /*__ASSEMBLER__*/

#endif /* MICROBLAZE_IRQ_CTRL_H_ */

