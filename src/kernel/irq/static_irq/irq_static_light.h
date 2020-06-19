/**
 * @file
 * @brief
 *
 * @date Mar 13, 2014
 * @author: Anton Bondarev
 */

#ifndef IRQ_STATIC_LIGHT_H_
#define IRQ_STATIC_LIGHT_H_

#define STATIC_IRQ_EXTENTION

#ifndef __ASSEMBLER__
#include <assert.h>
#include <kernel/irq_stack.h>
#endif

#define STATIC_IRQ_ATTACH(_irq_nr, _hnd, _data) \
	__STATIC_IRQ_DEF(_irq_nr, IRQ_HANDLER_NAME(_irq_nr), _hnd, _data) \
	__STATIC_IRQ_TABLE_ADD(_irq_nr, IRQ_HANDLER_NAME(_irq_nr));

#define STATIC_EXC_ATTACH(_exc_nr, _hnd, _data) \
	__STATIC_IRQ_DEF(_exc_nr, EXC_HANDLER_NAME(_exc_nr), _hnd, _data)

#define EXC_HANDLER_NAME(_exc_nr) \
	__exc_handler__ ## _exc_nr

#define IRQ_HANDLER_NAME(_irq_nr) \
	__static_irq__ ## _irq_nr

#define STATIC_IRQ_HANDLER_VAR_NAME(irq_num) \
	irq_handler##irq_num

#define STATIC_IRQ_HANDLER_SECTION(irq_num) \
	.static_irq_table.routine_ ## irq_num

#define __STATIC_IRQ_DEF(_irq_nr, _static_hnd, _hnd, _data) \
	void _static_hnd(void) { \
		assertf(irq_stack_protection() == 0, \
			"Stack overflow detected on irq handler %s\n", __func__); \
		critical_enter(CRITICAL_IRQ_HANDLER); \
		_hnd(_irq_nr, _data); \
		critical_leave(CRITICAL_IRQ_HANDLER); \
	}

#define __STATIC_IRQ_TABLE_ADD(irq_num, irq_handler) \
	void * STATIC_IRQ_HANDLER_VAR_NAME(irq_num) \
		__attribute__((section(MACRO_STRING(STATIC_IRQ_HANDLER_SECTION(irq_num))))) = (void *)irq_handler;

#endif /* IRQ_STATIC_LIGHT_H_ */
