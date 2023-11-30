/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    02.08.2013
 */

#ifndef MSP430_KERNEL_REG_REG_H_
#define MSP430_KERNEL_REG_REG_H_

#define __REG_STORE(addr, value)              \
	do {                                      \
		*((volatile char *)(addr)) = (value); \
	} while (0)

#define __REG_LOAD(addr)        *((volatile char *)(addr))

#define __REG_ORIN(addr, mask)  __REG_STORE(addr, REG_LOAD(addr) | (char)(mask))

#define __REG_ANDIN(addr, mask) __REG_STORE(addr, REG_LOAD(addr) & (char)(mask))

#define __REG_CLEAR(addr, mask) \
	__REG_STORE(addr, REG_LOAD(addr) & ~(char)(mask))

#endif /* MSP430_KERNEL_REG_REG_H_ */
