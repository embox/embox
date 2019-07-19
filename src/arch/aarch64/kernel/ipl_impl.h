/**
 * @file ipl_impl.h
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 19.07.2019
 */

#ifndef AARCH64_IPL_IMPL_H_
#define AARCH64_IPL_IMPL_H_

#ifndef __ASSEMBLER__

#include <stdint.h>

#include <asm/hal/reg.h>

typedef int __ipl_t;

static inline void ipl_init(void) {
	/* NIY */
}

static inline __ipl_t ipl_save(void) {
	/* NIY */
	return 0;
}

static inline void ipl_restore(__ipl_t ipl) {
	/* NIY */
}

#endif /* __ASSEMBLER__ */

#endif /* AARCH64_IPL_IMPL_H_ */
