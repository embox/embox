/**
 * @file
 * @brief
 *
 * @date Mar 13, 2014
 * @author: Anton Bondarev
 */

#ifndef KERNEL_IRQ_STATIC_H_
#define KERNEL_IRQ_STATIC_H_

#define STATIC_IRQ_EXTENTION

#ifndef __ASSEMBLER__
#include <module/embox/kernel/irq_static_impl.h>
#endif

#define STATIC_IRQ_ATTACH(_irq_nr, _hnd, _data) \
	STATIC_IRQ_HANDLER_DEF(_irq_nr, __IRQ_HANDLER_NAME(_irq_nr), _hnd, _data) \
	__STATIC_IRQ_TABLE_ADD(_irq_nr, __IRQ_HANDLER_NAME(_irq_nr))

#define __IRQ_HANDLER_NAME(_irq_nr) \
	irq_handler_ ## _irq_nr

#define STATIC_IRQ_HANDLER_VAR_NAME(irq_num) \
	irq_handler_var##irq_num

#define STATIC_IRQ_HANDLER_SECTION(irq_num) \
	.static_irq_table.routine_ ## irq_num

#define __STATIC_IRQ_TABLE_ADD(irq_num, irq_handler) \
	void * STATIC_IRQ_HANDLER_VAR_NAME(irq_num) \
		__attribute__((section(MACRO_STRING(STATIC_IRQ_HANDLER_SECTION(irq_num))))) = (void *)irq_handler;

#endif /* KERNEL_IRQ_STATIC_H_ */
