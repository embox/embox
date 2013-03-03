/**
 * @file
 *
 * @date Oct 18, 2012
 * @author: Anton Bondarev
 */

#ifndef PPC_IO_H_
#define PPC_IO_H_

#include <stdint.h>

//FIXME BAD BAD BAD (this is for only serial diag 8250 on PPC architecture)

#define IO_OFFSET    0x83e01003

static inline void out8(uint8_t v, uint32_t a) {
	*((volatile uint8_t *)(((a - 0x3f8) << 2) + IO_OFFSET)) = (uint8_t)(v);
}

static inline uint8_t in8(uint32_t a) {
	return *(volatile uint8_t *)(((a - 0x3f8) << 2) + IO_OFFSET);
}


#endif /* PPC_IO_H_ */
