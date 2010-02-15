/**
 * @file
 * @brief Defines macros used for direct access to registers mapped into
 * the address space.
 *
 * @date 25.11.2009
 * @author Eldar Abusalimov
 */

#ifndef HAL_REG_H_
#define HAL_REG_H_

#if 0
#include <asm/hal/reg.h>

#define REG_STORE(addr, value) \
	__REG_STORE(addr, value)

#define REG_LOAD(addr) \
	__REG_LOAD(addr)

#define REG_ORIN(addr, mask) \
	__REG_ORIN(addr, mask)

#define REG_ANDIN(addr, mask) \
	__REG_ANDIN(addr, mask)

#else

#define REG_STORE(addr, value) *(addr) = value

#define REG_LOAD(addr) *(addr)

#define REG_ORIN(addr, mask) \
		REG_STORE(addr, REG_LOAD(addr) | (unsigned long)(mask))

#define REG_ANDIN(addr, mask) \
		REG_STORE(addr, REG_LOAD(addr) & (unsigned long)(mask))

#endif

#endif /* HAL_REG_H_ */
