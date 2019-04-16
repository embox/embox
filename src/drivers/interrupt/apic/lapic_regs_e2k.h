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
	return e2k_read32((uintptr_t) reg);
}

static inline void lapic_write(uint64_t reg, uint32_t value) {
	e2k_write32(value, (uintptr_t) reg);
}

/* Get number of current IRQ number.
 * I do not find the similar thing for x86, so make it
 * E2K specific for now. */
#define APIC_VECT   (0xFEE00000 + 0xff0)

static inline int lapic_get_irq(void) {
	return lapic_read(APIC_VECT) - 32;
}

#endif /* LAPIC_REGS_E2K_H_ */
