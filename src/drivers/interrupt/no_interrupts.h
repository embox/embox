/**
 * @file
 * @brief No interrupts
 *
 * @date 24.03.2012
 * @author Anton Kozlov
 */

#ifndef DRIVERS_INTERRUPT_NO_INTERRUPTS
#define DRIVERS_INTERRUPT_NO_INTERRUPTS

#define __INTERRUPT_NRS_TOTAL 32

#ifndef __ASSEMBLER__

typedef unsigned long __interrupt_mask_t;

#endif /*__ASSEMBLER__*/

#endif /* DRIVERS_INTERRUPT_NO_INTERRUPTS */
