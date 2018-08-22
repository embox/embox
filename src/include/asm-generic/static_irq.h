/**
 * @file
 * @brief
 *
 * @date Mar 13, 2014
 * @author: Anton Bondarev
 */

#ifndef STATIC_IRQ_H_
#define STATIC_IRQ_H_

#include <module/embox/kernel/irq_api.h>
#include <framework/mod/options.h>

#ifdef STATIC_IRQ_EXTENTION

#define SIRQ_TABLE_ENTRY(n) \
	. = n * 4; \
	*(.trap_table.routine_##n);

/* Currently, we allow to define handlers starting from
 * SVCall, please take a look at exception_table.S and
 * http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0553a/CIHIGCIF.html */
#define STATIC_IRQ_TABLE \
	SIRQ_TABLE_ENTRY(11) \
	SIRQ_TABLE_ENTRY(12) \
	SIRQ_TABLE_ENTRY(13) \
	SIRQ_TABLE_ENTRY(14) \
	SIRQ_TABLE_ENTRY(15) \
	SIRQ_TABLE_ENTRY(63) \
	SIRQ_TABLE_ENTRY(77)

#define ARM_M_IRQ_HANDLER_VAR_NAME(irq_num) \
	arm_m_irq_handler##irq_num

#define ARM_M_IRQ_HANDLER_SECTION_STR(irq_num) \
	".trap_table.routine_"#irq_num

#define ARM_M_IRQ_HANDLER_DEF(irq_num, irq_handler) \
	void * ARM_M_IRQ_HANDLER_VAR_NAME(irq_num) \
		__attribute__((section(ARM_M_IRQ_HANDLER_SECTION_STR(irq_num)))) = (void *)irq_handler;


#else

#define STATIC_IRQ_TABLE

#endif /* STATIC_IRQ_EXTENTION */


#endif /* STATIC_IRQ_H_ */
