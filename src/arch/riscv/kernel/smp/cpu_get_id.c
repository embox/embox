/**
 * @file
 * @brief
 *
 * @date 21.08.2024
 * @author Zeng Zixian
 */

#include <hal/cpu.h>
#include <asm/entry.h>
#include <asm/ptrace.h>
#include <asm/csr.h>
#include <riscv/smode.h>

#define INVALIDHARTID (unsigned int)(-1)
static unsigned int __cpuid_to_hartid_map[NCPU] = {
	[0] = 0, /* BSP always has mhartid and cpuid 0 */
	[1 ... NCPU-1] = INVALIDHARTID
};

/**
 * Since mhartids are not guaranteed to be consecutive
 * so each AP needs to register mhartid to cpuid at boot time
 *
 * return cpuid
 */
#if !RISCV_SMODE
unsigned int cpuid_register(void) {
	int cpuid = 1; /* skip BSP */
	unsigned int mhartid = csr_read(mhartid);
	for(;cpuid < NCPU; cpuid++) {
		if(__cpuid_to_hartid_map[cpuid] == INVALIDHARTID) {
			__cpuid_to_hartid_map[cpuid] = mhartid;
			break;
		}
	}
	return cpuid;
}

unsigned int cpu_get_id(void) {
	int cpuid = 0;
	unsigned int mhartid = csr_read(mhartid);
	for(cpuid = 0; cpuid < NCPU; cpuid++) {
		if(__cpuid_to_hartid_map[cpuid] == mhartid)
			break;
	}
	return cpuid;
}
#else
unsigned int cpuid_register(void) { return 0 }
unsigned int cpu_get_id(void) { return 0 }
#endif
