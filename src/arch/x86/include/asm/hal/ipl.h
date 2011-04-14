/**
 * @file
 *
 * @date 10.11.10
 * @author Nikolay Korotky
 */

#ifndef HAL_IPL_H_
# error "Do not include this file directly!"
#endif /* HAL_IPL_H_ */

#include <types.h>

#include <hal/interrupt.h>

typedef uint32_t __ipl_t;

static inline void ipl_init(void) {
}

static inline __ipl_t ipl_save(void) {
	return 0;
}

static inline void ipl_restore(__ipl_t ipl) {
}
