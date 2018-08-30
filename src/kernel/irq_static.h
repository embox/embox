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
#include <asm/regs.h>

#ifndef __ASSEMBLER__
#include <assert.h>
#include <kernel/irq_stack.h>
#endif

#define STATIC_IRQ_ATTACH(_irq_nr, _hnd, _data) \
	__STATIC_IRQ_ATTACH(_irq_nr, __static_irq__ ## _irq_nr, _hnd, _data)

#define __STATIC_IRQ_ATTACH(_irq_nr, _static_hnd, _hnd, _data) \
	static void _static_hnd(void) { \
		assertf(irq_stack_protection() == 0, \
			"Stack overflow detected on irq handler %s\n", __func__); \
		_hnd(_irq_nr, _data); \
	} \
	ARM_M_IRQ_HANDLER_DEF(_irq_nr, _static_hnd);

#endif /* IRQ_STATIC_H_ */
