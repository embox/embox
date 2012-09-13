/**
 * @file
 * @brief Core definitions for on-CPU interrupt support.
 *
 * @date 06.09.2012
 * @author Eldar Abusalimov
 */

#ifndef ARCH_INTERRUPT_H_
#define ARCH_INTERRUPT_H_

#include <module/embox/arch/interrupt_api.h>

#ifndef INTERRUPT_IMPLEMENTED
#define __INTERRUPT_NRS_TOTAL 0
#endif /* INTERRUPT_IMPLEMENTED */

/**
 * Total amount of interrupt lines that CPU can handle.
 *
 * @note Implementation should provide @c __INTERRUPT_NRS_TOTAL definition
 * indicating positive constant.
 */
#define INTERRUPT_NRS_TOTAL \
	((unsigned int) __INTERRUPT_NRS_TOTAL)

/**
 * Checks if the specified @c interrupt_nr is less then #INTERRUPT_NRS_TOTAL
 * value.
 */
#define interrupt_nr_valid(interrupt_nr) \
	((unsigned int) interrupt_nr < INTERRUPT_NRS_TOTAL)

#endif /* ARCH_INTERRUPT_H_ */
