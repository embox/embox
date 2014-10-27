/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    24.10.2014
 */

#ifndef HAL_MEM_H_
#define HAL_MEM_H_

#include <stdint.h>

typedef volatile uint32_t reg32_t;

static inline void regcpy32(reg32_t *dst, const uint32_t *src, unsigned int _count) {
	unsigned int count = _count;

	while (count--) {
		REG_STORE(dst++, *src++);
	}
}

#endif /* HAL_MEM_H_ */
