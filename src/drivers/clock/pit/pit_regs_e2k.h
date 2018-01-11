/**
 * @file pit_regs_e2k.h
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 22.12.2017
 */

#ifndef PIT_REGS_E2K_H_
#define PIT_REGS_E2K_H_

#include <asm/io.h>
#include <stdint.h>

static inline void pit_out8(uint8_t val, int port) {
	e2k_out8(val, port);
}

static inline uint8_t pit_in8(int port) {
	return e2k_in8(port);
}

#endif /* PIT_REGS_E2K_H_ */
