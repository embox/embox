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

#include <asm/hal/reg.h>
#include <stdint.h>

#ifndef __REG_STORE
#define __REG_STORE(addr, value) \
	do { *((volatile unsigned long *)(addr)) = (value); } while (0)
#endif /* __REG_STORE */

#ifndef __REG_LOAD
#define __REG_LOAD(addr) \
	*((volatile unsigned long *)(addr))
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

#define REG32_LOAD(addr) \
	*((volatile uint32_t *)(addr))

#define REG32_STORE(addr, val) \
	do { *((volatile uint32_t *)(addr)) = (val); } while (0)

#define REG16_LOAD(addr) \
	*((volatile uint16_t *)(addr))

#define REG16_STORE(addr, val) \
	do { *((volatile uint16_t *)(addr)) = (val); } while (0)

#define REG8_LOAD(addr) \
	*((volatile uint8_t *)(addr))

#define REG8_STORE(addr, val) \
	do { *((volatile uint8_t *)(addr)) = (val); } while (0)

#endif /* HAL_REG_H_ */
