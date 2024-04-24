/**
 * @file
 * @brief
 *
 * @date 27.06.2012
 * @author Anton Bondarev
 */

#ifndef MIPS_IO_H_
#define MIPS_IO_H_

#include <stdint.h>

#include <asm/addrspace.h>

#define IO_OFFSET (0x1fd00000 + KSEG1) /* ISA_BASE + kseg1 */

#define out8(v, a)                                                     \
	do {                                                               \
		*((volatile uint8_t *)((size_t)a + IO_OFFSET)) = (uint8_t)(v); \
	} while (0)
#define in8(a) (*(volatile uint8_t *)((size_t)a + IO_OFFSET))

#define out32(v, a)                                                      \
	do {                                                                 \
		*((volatile uint32_t *)((size_t)a + IO_OFFSET)) = (uint32_t)(v); \
	} while (0)
#define in32(a)  (*(volatile uint32_t *)((size_t)a + IO_OFFSET))

#define mmiowb() asm volatile("sync" ::: "memory")

#define __sync() mmiowb()

#endif /* MIPS_IO_H_ */
