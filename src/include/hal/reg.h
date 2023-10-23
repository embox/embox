/**
 * @file
 * @brief Defines macros used for direct access to registers mapped into
 * the address space.
 *
 * @date 25.11.09
 * @author Eldar Abusalimov
 */

#ifndef HAL_REG_H_
#define HAL_REG_H_

#include <stdint.h>

#include <asm/hal/reg.h>

#ifndef __REG_STORE
#define __REG_STORE(addr, value)                       \
	do {                                               \
		*((volatile unsigned long *)(addr)) = (value); \
	} while (0)
#endif /* __REG_STORE */

#ifndef __REG_LOAD
#define __REG_LOAD(addr) *((volatile unsigned long *)(addr))
#endif /* __REG_LOAD */

#ifndef __REG_ORIN
#define __REG_ORIN(addr, mask) \
	REG_STORE(addr, REG_LOAD(addr) | (unsigned long)(mask))
#endif /* __REG_ORIN */

#ifndef __REG_ANDIN
#define __REG_ANDIN(addr, mask) \
	REG_STORE(addr, REG_LOAD(addr) & (unsigned long)(mask))
#endif /* __REG_ANDIN */

#define REG_STORE(addr, value) __REG_STORE(addr, value)
#define REG_LOAD(addr)         __REG_LOAD(addr)
#define REG_ORIN(addr, mask)   __REG_ORIN(addr, mask)
#define REG_ANDIN(addr, mask)  __REG_ANDIN(addr, mask)

#define REG64_LOAD(addr) *((volatile uint64_t *)((uintptr_t)addr))

#define REG64_STORE(addr, val)                             \
	do {                                                   \
		*((volatile uint64_t *)((uintptr_t)addr)) = (val); \
	} while (0)

#define REG64_ORIN(addr, mask) \
	REG64_STORE(addr, REG64_LOAD(addr) | ((uint64_t)mask))

#define REG64_CLEAR(addr, mask) \
	REG64_STORE(addr, REG64_LOAD(addr) & (~((uint64_t)mask)))

#define REG32_LOAD(addr) *((volatile uint32_t *)((uintptr_t)addr))

#define REG32_STORE(addr, val)                             \
	do {                                                   \
		*((volatile uint32_t *)((uintptr_t)addr)) = (val); \
	} while (0)

#define REG32_ORIN(addr, mask) \
	REG32_STORE(addr, REG32_LOAD(addr) | ((uint32_t)mask))

#define REG32_ANDIN(addr, mask) \
	REG32_STORE(addr, REG32_LOAD(addr) & ((uint32_t)mask))

#define REG32_CLEAR(addr, mask) \
	REG32_STORE(addr, REG32_LOAD(addr) & (~((uint32_t)mask)))

#define REG16_LOAD(addr) *((volatile uint16_t *)((uintptr_t)addr))

#define REG16_STORE(addr, val)                             \
	do {                                                   \
		*((volatile uint16_t *)((uintptr_t)addr)) = (val); \
	} while (0)

#define REG16_ORIN(addr, mask) \
	REG16_STORE(addr, REG16_LOAD(addr) | ((uint16_t)mask))

#define REG16_ANDIN(addr, mask) \
	REG16_STORE(addr, REG16_LOAD(addr) & ((uint16_t)mask))

#define REG16_CLEAR(addr, mask) \
	REG16_STORE(addr, REG16_LOAD(addr) & (~((uint16_t)mask)))

#define REG8_LOAD(addr) *((volatile uint8_t *)((uintptr_t)addr))

#define REG8_STORE(addr, val)                             \
	do {                                                  \
		*((volatile uint8_t *)((uintptr_t)addr)) = (val); \
	} while (0)

#define REG8_ORIN(addr, mask) \
	REG8_STORE(addr, REG8_LOAD(addr) | ((uint8_t)mask))

#define REG8_ANDIN(addr, mask) \
	REG8_STORE(addr, REG8_LOAD(addr) & ((uint8_t)mask))

#define REG8_CLEAR(addr, mask) \
	REG8_STORE(addr, REG8_LOAD(addr) & (~((uint8_t)mask)))

#endif /* HAL_REG_H_ */
