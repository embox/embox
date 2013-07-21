/**
 * @file
 * @brief
 *
 * @date 21.07.13
 * @author Ilia Vaprol
 */

#ifndef ARCH_SPARC_LIB_CMPXCHG_IMPL_H_
#define ARCH_SPARC_LIB_CMPXCHG_IMPL_H_

#include <linux/compiler.h>

#ifndef __ASSEMBLER__

#define __HAVE_ARCH_CMPXCHG

/* FIXME pseudo cmpxchg */
static inline unsigned long cmpxchg(unsigned long *ptr,
		unsigned long old_val, unsigned long new_val) {
	unsigned long ret;

	ret = *ptr;
	if (ret == old_val) {
		*ptr = new_val;
	}

	__barrier();

	return ret;
}

#endif /* !__ASSEMBLER__ */

#endif /* !ARCH_SPARC_LIB_CMPXCHG_IMPL_H_ */
