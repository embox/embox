/**
 * @file lapic_regs_e2k.h
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 21.12.2017
 */

#ifndef LAPIC_REGS_E2K_H_
#define LAPIC_REGS_E2K_H_

#include <asm/io.h>

static inline uint32_t lapic_read(uint64_t reg) {
	return e2k_read32((void *) reg);
}

static inline void lapic_write(uint64_t reg, uint32_t value) {
	e2k_write32(value, (void *) reg);
}

#endif /* LAPIC_REGS_E2K_H_ */
