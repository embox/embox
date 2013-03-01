/**
 * @file
 * @brief Implements interrupt process level for microblaze architecture
 *
 * @details Microblaze hasn't ipl level on chip. We use interrupt controller
 * mask/unmask operations for implementing ipl. While initialize we enable bit
 * @link #MSR_IE_BIT @endlink for interrupts enable in MSR register.
 *
 * @date 13.03.10
 * @author Alexey Fomin
 */

#ifndef MICROBLAZE_IPL_IMPL_H_
#define MICROBLAZE_IPL_IMPL_H_

#ifndef __ASSEMBLER__

#include <asm/msr.h>
#include <stdint.h>

typedef uint32_t __ipl_t;

static inline void ipl_init(void) {
	msr_set_ie();
}

static inline __ipl_t ipl_save(void) {
	__ipl_t ipl;
	ipl = msr_get_bit(MSR_IE_BIT);
	msr_clr_ie();
	return ipl;
}

static inline void ipl_restore(__ipl_t ipl) {
	ipl ? msr_set_ie() : msr_clr_ie();
}

#endif /* !__ASSEMBLER__ */

#endif /* MICROBLAZE_IPL_IMPL_H_ */
