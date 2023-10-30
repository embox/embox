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
#include <asm/modes.h>

typedef uint64_t __ipl_t;

static inline void ipl_init(void) {
	set_daif(get_daif() & ~(DAIF_I_BIT | DAIF_F_BIT));
}

static inline __ipl_t ipl_save(void) {
	__ipl_t r = get_daif();
	set_daif(r | DAIF_I_BIT | DAIF_F_BIT);
	return r;
}

static inline void ipl_restore(__ipl_t ipl) {
	set_daif(ipl);
}

#endif /* __ASSEMBLER__ */

#endif /* AARCH64_IPL_IMPL_H_ */
