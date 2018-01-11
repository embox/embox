/**
 * @file ioapic_regs_x86.h
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 21.12.2017
 */

#ifndef IOAPIC_REGS_X86_H_
#define IOAPIC_REGS_X86_H_

#include <stdint.h>

#define IOAPIC_DEF_ADDR	          0xFEC00000
#define IOREGSEL                  (IOAPIC_DEF_ADDR + 0x00)
#define IOREGWIN                  (IOAPIC_DEF_ADDR + 0x10)

static inline uint32_t ioapic_read(uint8_t reg) {
	*((volatile uint32_t *)(IOREGSEL)) = reg;
	return *((volatile uint32_t *)(IOREGWIN));
}

static inline void ioapic_write(uint8_t reg, uint32_t val) {
	*((volatile uint32_t *)(IOREGSEL)) = reg;
	*((volatile uint32_t *)(IOREGWIN)) = val;
}

#endif /* IOAPIC_REGS_X86_H_ */
