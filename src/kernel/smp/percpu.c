/**
 * @file
 * @brief TODO
 *
 * @date 08.02.2012
 * @author Anton Bulychev
 */

#include <stdint.h>
#include <string.h>

#include <kernel/percpu.h>

/* TODO: Delete it */
#ifdef SMP

void percpu_init(void) {
	unsigned int cpu;
	char *cpu_data_block;

	cpu = 1;
	cpu_data_block = __PERCPU_START;

	while (cpu < NCPU) {
		++cpu;
		cpu_data_block += __PERCPU_BLOCK_SZ;

		memcpy(cpu_data_block, __PERCPU_START, __PERCPU_BLOCK_SZ);
	}
}

#endif /* SMP */
