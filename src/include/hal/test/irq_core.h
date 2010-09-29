/**
 * @file
 *
 * @date 15.03.2010
 * @author Anton Bondarev
 */

#ifndef TESTIRQ_CORE_H_
#define TESTIRQ_CORE_H_

#include <asm/hal/test/testirq_core.h>

extern void testirq_save_status(uint32_t *status);

extern void testirq_restore_status(uint32_t *status);

#endif /* TESTIRQ_CORE_H_ */
