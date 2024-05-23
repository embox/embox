/**
 * @file
 * @brief
 *
 * @date 27.11.23
 * @author Aleksey Zhmulin
 */
#ifndef ARMLIB_REG_REG_H_
#define ARMLIB_REG_REG_H_

#include <stdint.h>

#define __ARCH_REG_STORE(reg, val) __ARCH_REG_STORE__##reg(val)

#define __ARCH_REG_LOAD(reg)       __ARCH_REG_LOAD__##reg()

#define __ARCH_REG_ORIN(reg, mask) \
	__ARCH_REG_STORE__##reg(__ARCH_REG_LOAD__##reg() | ((uint32_t)mask))

#define __ARCH_REG_ANDIN(reg, mask) \
	__ARCH_REG_STORE__##reg(__ARCH_REG_LOAD__##reg() & ((uint32_t)mask))

#define __ARCH_REG_CLEAR(reg, mask) \
	__ARCH_REG_STORE__##reg(__ARCH_REG_LOAD__##reg() & ~((uint32_t)mask))

#endif /* ARMLIB_REG_REG_H_ */
