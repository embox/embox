/**
 * @file
 * @brief
 *
 * @date 08.02.12
 * @author Anton Bulychev
 * @author Ilia Vaprol
 */

#include <kernel/cpu/cpudata.h>
#include <string.h>

void cpudata_init(void) {
	unsigned int cpu_id;
	char *cpudata_block;

	cpu_id = 0;
	cpudata_block = __CPUDATA_START;

	while (++cpu_id < NCPU) {
		cpudata_block += __CPUDATA_BLOCK_SZ;

		memcpy(cpudata_block, __CPUDATA_START, __CPUDATA_BLOCK_SZ);
	}
}
