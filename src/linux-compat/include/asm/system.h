/**
 * @file
 * @brief Linux-like wrapper macros for interrupts masking.
 *
 * @date 26.01.2010
 * @author Eldar Abusalimov
 */

#ifndef ASM_SYSTEM_H_
#define ASM_SYSTEM_H_

#include <hal/ipl.h>

#define local_irq_enable()       ipl_enable()
#define local_irq_disable()      ipl_disable()

#define local_irq_save(flags)    ((flags) = ipl_save())
#define local_irq_restore(flags) ipl_restore(flags)

#endif /* ASM_SYSTEM_H_ */
