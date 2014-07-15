/**
 * @file
 * @brief
 *
 * @date Mar 13, 2014
 * @author: Anton Bondarev
 */

#ifndef IRQ_STATIC_H_
#define IRQ_STATIC_H_

#define STATIC_IRQ_EXTENTION

#include <asm-generic/static_irq.h>

#define STATIC_IRQ_ATTACH(_irq_nr, _hnd, _data) \
	__STATIC_IRQ_ATTACH(_irq_nr, __static_irq__ ## _irq_nr, _hnd, _data)

#define __STATIC_IRQ_ATTACH(_irq_nr, _static_hnd, _hnd, _data) \
	static void _static_hnd(void) { \
		_hnd(_irq_nr, _data); \
	} \
	ARM_M_IRQ_HANDLER_DEF(_irq_nr, _static_hnd);

#endif /* IRQ_STATIC_H_ */
