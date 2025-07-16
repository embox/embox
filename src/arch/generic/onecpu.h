/**
 * @file
 * @brief
 *
 * @date 24.07.13
 * @author Ilia Vaprol
 */

#ifndef ARCH_GENERIC_ONECPU_H_
#define ARCH_GENERIC_ONECPU_H_

#ifndef __ASSEMBLER__

#include <sys/cdefs.h>

__BEGIN_DECLS

static inline unsigned int cpu_get_id(void) {
	return 0;
}

__END_DECLS

#endif /* !__ASSEMBLER__ */

#endif /* ARCH_GENERIC_ONECPU_H_ */
