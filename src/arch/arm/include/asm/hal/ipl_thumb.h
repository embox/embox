/**
 * @file
 *
 * @date 01.07.10
 * @author Anton Kozlov
 */

#ifndef HAL_IPL_H_
# error "Do not include this file directly!"
#endif /* HAL_IPL_H_ */

#include <types.h>

typedef uint32_t __ipl_t;


static inline void ipl_init(void) {

}

static inline __ipl_t ipl_save(void) {
	uint32_t r = 0;
	return r;
}

static inline void ipl_restore(__ipl_t ipl) {
}
