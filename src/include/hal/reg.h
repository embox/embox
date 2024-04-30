/**
 * @file
 * @brief Defines macros used for direct access to registers.
 *
 * @date 25.11.09
 * @author Eldar Abusalimov
 */

#ifndef HAL_REG_H_
#define HAL_REG_H_

#include <stdint.h>

#include <module/embox/arch/reg.h>

/**
 * Architecture-specific registers.
 */
#define ARCH_REG_STORE(reg, val)  __ARCH_REG_STORE(reg, val)
#define ARCH_REG_LOAD(reg)        __ARCH_REG_LOAD(reg)
#define ARCH_REG_ORIN(reg, mask)  __ARCH_REG_ORIN(reg, mask)
#define ARCH_REG_ANDIN(reg, mask) __ARCH_REG_ANDIN(reg, mask)
#define ARCH_REG_CLEAR(reg, mask) __ARCH_REG_CLEAR(reg, mask)

#ifndef __MMAP_REG_STORE
#define __MMAP_REG_STORE(inttype, addr, val)              \
	do {                                                  \
		*((volatile inttype *)((uintptr_t)addr)) = (val); \
	} while (0)
#endif /* __MMAP_REG_STORE */

#ifndef __MMAP_REG_LOAD
#define __MMAP_REG_LOAD(inttype, addr) *((volatile inttype *)((uintptr_t)addr))
#endif /* __MMAP_REG_LOAD */

/**
 * Registers mapped into the address space.
 */
#define MMAP_REG_STORE(inttype, addr, val) __MMAP_REG_STORE(inttype, addr, val)
#define MMAP_REG_LOAD(inttype, addr)       __MMAP_REG_LOAD(inttype, addr)

#define MMAP_REG_ORIN(inttype, addr, mask) \
	MMAP_REG_STORE(inttype, addr,          \
	    MMAP_REG_LOAD(inttype, addr) | ((inttype)mask))

#define MMAP_REG_ANDIN(inttype, addr, mask) \
	MMAP_REG_STORE(inttype, addr,           \
	    MMAP_REG_LOAD(inttype, addr) & ((inttype)mask))

#define MMAP_REG_CLEAR(inttype, addr, mask) \
	MMAP_REG_STORE(inttype, addr,           \
	    MMAP_REG_LOAD(inttype, addr) & ~((inttype)mask))

#ifndef __REG_STORE
#define __REG_STORE(addr, value) MMAP_REG_STORE(unsigned long, addr, value)
#endif /* __REG_STORE */

#ifndef __REG_LOAD
#define __REG_LOAD(addr) MMAP_REG_LOAD(unsigned long, addr)
#endif /* __REG_LOAD */

#ifndef __REG_ORIN
#define __REG_ORIN(addr, mask) MMAP_REG_ORIN(unsigned long, addr, mask)
#endif /* __REG_ORIN */

#ifndef __REG_ANDIN
#define __REG_ANDIN(addr, mask) MMAP_REG_ANDIN(unsigned long, addr, mask)
#endif /* __REG_ANDIN */

#ifndef __REG_CLEAR
#define __REG_CLEAR(addr, mask) MMAP_REG_CLEAR(unsigned long, addr, mask)
#endif /* __REG_ANDIN */

#define REG_STORE(addr, value)   __REG_STORE(addr, value)
#define REG_LOAD(addr)           __REG_LOAD(addr)
#define REG_ORIN(addr, mask)     __REG_ORIN(addr, mask)
#define REG_ANDIN(addr, mask)    __REG_ANDIN(addr, mask)
#define REG_CLEAR(addr, mask)    __REG_CLEAR(addr, mask)

#define REG64_STORE(addr, value) MMAP_REG_STORE(uint64_t, addr, value)
#define REG64_LOAD(addr)         MMAP_REG_LOAD(uint64_t, addr)
#define REG64_ORIN(addr, mask)   MMAP_REG_ORIN(uint64_t, addr, mask)
#define REG64_ANDIN(addr, mask)  MMAP_REG_ANDIN(uint64_t, addr, mask)
#define REG64_CLEAR(addr, mask)  MMAP_REG_CLEAR(uint64_t, addr, mask)

#define REG32_STORE(addr, value) MMAP_REG_STORE(uint32_t, addr, value)
#define REG32_LOAD(addr)         MMAP_REG_LOAD(uint32_t, addr)
#define REG32_ORIN(addr, mask)   MMAP_REG_ORIN(uint32_t, addr, mask)
#define REG32_ANDIN(addr, mask)  MMAP_REG_ANDIN(uint32_t, addr, mask)
#define REG32_CLEAR(addr, mask)  MMAP_REG_CLEAR(uint32_t, addr, mask)

#define REG16_STORE(addr, value) MMAP_REG_STORE(uint16_t, addr, value)
#define REG16_LOAD(addr)         MMAP_REG_LOAD(uint16_t, addr)
#define REG16_ORIN(addr, mask)   MMAP_REG_ORIN(uint16_t, addr, mask)
#define REG16_ANDIN(addr, mask)  MMAP_REG_ANDIN(uint16_t, addr, mask)
#define REG16_CLEAR(addr, mask)  MMAP_REG_CLEAR(uint16_t, addr, mask)

#define REG8_STORE(addr, value)  MMAP_REG_STORE(uint8_t, addr, value)
#define REG8_LOAD(addr)          MMAP_REG_LOAD(uint8_t, addr)
#define REG8_ORIN(addr, mask)    MMAP_REG_ORIN(uint8_t, addr, mask)
#define REG8_ANDIN(addr, mask)   MMAP_REG_ANDIN(uint8_t, addr, mask)
#define REG8_CLEAR(addr, mask)   MMAP_REG_CLEAR(uint8_t, addr, mask)

#endif /* HAL_REG_H_ */
