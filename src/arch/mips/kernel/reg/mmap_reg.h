/**
 * @brief store and load without caching and mmu
 *
 * @date 30.04.24
 * @author Aleksey Zhmulin
 */

#ifndef MIPS_KERNEL_REG_REG_H_
#define MIPS_KERNEL_REG_REG_H_

#include <stdint.h>

#include <asm/addrspace.h>

#define __MMAP_REG_STORE(inttype, addr, val)                       \
	do {                                                           \
		*(volatile inttype *)((uintptr_t)KSEG1ADDR(addr)) = (val); \
	} while (0)

#define __MMAP_REG_LOAD(inttype, addr) \
	*(volatile inttype *)((uintptr_t)KSEG1ADDR(addr))

#endif /* MIPS_KERNEL_REG_REG_H_ */
