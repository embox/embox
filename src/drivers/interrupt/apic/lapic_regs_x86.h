/**
 * @file lapic_regs_x86.h
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 21.12.2017
 */

#ifndef LAPIC_REGS_X86_H_
#define LAPIC_REGS_X86_H_

#include <stdint.h>

static inline uint32_t lapic_read(uint32_t reg) {
	return *((volatile uint32_t *) reg);
}

static inline void lapic_write(uint32_t reg, uint32_t value) {
	*((volatile uint32_t *) reg) = value;
}

#endif /* LAPIC_REGS_X86_H_ */
