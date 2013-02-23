/**
 * @file
 * @brief TODO
 *
 * @date 08.02.2012
 * @author Anton Bulychev
 */

#include <types.h>
#include <string.h>

#include <kernel/percpu.h>

/* TODO: Delete it */
#ifdef SMP

void percpu_init(void) {
	for (unsigned int i = 1; i < NCPU; i++) {
		memcpy(__PERCPU_START + i*__PERCPU_LEN, __PERCPU_START, __PERCPU_LEN);
	}
}

#endif /* SMP */
