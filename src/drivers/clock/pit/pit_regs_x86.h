/**
 * @file pit_regs_x86.h
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 22.12.2017
 */

#ifndef PIT_REGS_X86_H_
#define PIT_REGS_X86_H_

#include <asm/io.h>
#include <stdint.h>

static inline void pit_out8(uint8_t val, int port) {
	out8(val, port);
}

static inline uint8_t pit_in8(int port) {
	return in8(port);
}

#endif /* PIT_REGS_X86_H_ */
