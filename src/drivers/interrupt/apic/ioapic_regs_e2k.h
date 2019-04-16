/**
 * @file ioapic_regs_e2k.h
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 21.12.2017
 */

#ifndef IOAPIC_REGS_E2K_H_
#define IOAPIC_REGS_E2K_H_

#include <stdint.h>

#define IOAPIC_DEF_ADDR	          (uintptr_t)0xFEC00000
#define IOREGSEL                  (IOAPIC_DEF_ADDR + 0x00)
#define IOREGWIN                  (IOAPIC_DEF_ADDR + 0x10)

static inline uint32_t ioapic_read(uint8_t reg) {
	e2k_write32(reg, IOREGSEL);
	return e2k_read32(IOREGWIN);
}

static inline void ioapic_write(uint8_t reg, uint32_t val) {
	e2k_write32(reg, IOREGSEL);
	e2k_write32(val, IOREGWIN);
}

#endif /* IOAPIC_REGS_E2K_H_ */
