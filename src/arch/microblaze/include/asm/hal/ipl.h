/**
 * @file
 *
 * @brief Implements interrupt process level for microblaze architecture
 *
 * @details Microblaze hasn't ipl level on chip. We use interrupt controller
 * mask/unmask operations for implementing ipl. While initialize we enable bit
 * @link #MSR_IE_BIT @endlink for interrupts enable in MSR register.
 *
 * @date 13.03.10
 * @author Alexey Fomin
 */

#ifndef HAL_IPL_H_
# error "Do not include this file directly!"
#endif /* HAL_IPL_H_ */

#include <types.h>
#include <asm/msr.h>
#include <hal/interrupt.h>

typedef uint32_t __ipl_t;

static inline void ipl_init(void) {
	msr_set_ie();
}
#include "stdio.h"
static inline __ipl_t ipl_save(void) {
	__ipl_t ipl = msr_get_bit(MSR_IE_BIT);

	msr_clr_ie();

	return ipl;
}

static inline void ipl_restore(__ipl_t ipl) {

	ipl ? msr_set_ie() : msr_clr_ie();
}
