/**
 * @file
 *
 * @date 15.03.2010
 * @author Anton Bondarev
 */

#ifndef TEST_IRQ_CORE_H_
#define TEST_IRQ_CORE_H_

#include <asm/hal/test/irq_core.h>

extern void testirq_save_status(uint32_t *status);

extern void testirq_restore_status(uint32_t *status);

#endif /* TEST_IRQ_CORE_H_ */
