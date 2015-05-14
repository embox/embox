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
#define STATIC_IRQ_TABLE            \
		. = 0x10;                   \
		*(.trap_table.routine_4)    \
		. = 0x14;                   \
		*(.trap_table.routine_5)    \
		. = 0x18;                   \
		*(.trap_table.routine_6)    \
		. = 0x1C;                   \
		*(.trap_table.routine_7)    \
		. = 0x20;                   \
		*(.trap_table.routine_8)    \
		. = 0x24;                   \
		*(.trap_table.routine_9)    \
		. = 0x28;                   \
		*(.trap_table.routine_10)   \
		. = 0x2C;                   \
		*(.trap_table.routine_11)   \
		. = 0x30;                   \
		*(.trap_table.routine_12)   \
		. = 0x34;                   \
		*(.trap_table.routine_13)   \
		. = 0x38;                   \
		*(.trap_table.routine_14)   \
		. = 0x3C;                   \
		*(.trap_table.routine_15)   \
		. = 0x40;                   \
		*(.trap_table.assert)       \
		. = 0x134;                  \
		*(.trap_table.routine_61)   \
		. = 0x134;                  \
		*(.trap_table.routine_77)

#define ARM_M_IRQ_HANDLER_VAR_NAME(irq_num) \
	arm_m_irq_handler##irq_num

#define ARM_M_IRQ_HANDLER_SECTION_STR(irq_num) \
	".trap_table.routine_"#irq_num

#define ARM_M_IRQ_HANDLER_DEF(irq_num, irq_handler) \
	void * ARM_M_IRQ_HANDLER_VAR_NAME(irq_num) \
		__attribute__((section(ARM_M_IRQ_HANDLER_SECTION_STR(irq_num)))) = (void *)irq_handler;


#else

#define STATIC_IRQ_TABLE

#endif


#endif /* STATIC_IRQ_H_ */
