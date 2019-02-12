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

#define STATIC_IRQ_TABLE \
	__static_irq_table_start = .; \
	*(.static_irq_table*); \
	__static_irq_table_end = .;

#define ARM_M_IRQ_HANDLER_VAR_NAME(irq_num) \
	arm_m_irq_handler##irq_num

#define ARM_M_IRQ_HANDLER_SECTION_STR(irq_num) \
	".static_irq_table.routine_"#irq_num

#define ARM_M_IRQ_HANDLER_DEF(irq_num, irq_handler) \
	void * ARM_M_IRQ_HANDLER_VAR_NAME(irq_num) \
		__attribute__((section(ARM_M_IRQ_HANDLER_SECTION_STR(irq_num)))) = (void *)irq_handler;


#else

#define STATIC_IRQ_TABLE

#endif /* STATIC_IRQ_EXTENTION */


#endif /* STATIC_IRQ_H_ */
