/**
 * @file reg.h
 *
 * @brief Defines macros used for direct access to registers mapped into
 * the address space.
 *
 * @date 25.11.2009
 * @author Eldar Abusalimov
 */

#ifndef HAL_REG_H_
#define HAL_REG_H_

#include <asm/haldefs.h>

#define REG_STORE(addr, value) \
	__REG_STORE(addr, value)

#define REG_LOAD(addr) \
	__REG_LOAD(addr)

#define REG_ORIN(addr, mask) \
	__REG_ORIN(addr, mask)

#define REG_ANDIN(addr, mask) \
	__REG_ANDIN(addr, mask)

#endif /* HAL_REG_H_ */
