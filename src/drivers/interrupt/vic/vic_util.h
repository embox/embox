/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 05.09.23
 */
#ifndef SRC_DRIVERS_INTERRUPT_VIC_UTIL_H_
#define SRC_DRIVERS_INTERRUPT_VIC_UTIL_H_

#include <stdint.h>

#include <hal/reg.h>

#define VIC_REG_STORE(num, reg, val) REG32_STORE(vic_devices[num] + reg, val)
#define VIC_REG_LOAD(num, reg)       REG32_LOAD(vic_devices[num] + reg)

extern const uintptr_t vic_devices[];

#endif /* SRC_DRIVERS_INTERRUPT_VIC_UTIL_H_ */
