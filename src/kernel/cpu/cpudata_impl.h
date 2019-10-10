/**
 * @file
 * @brief
 *
 * @date 18.07.13
 * @author Ilia Vaprol
 */

#ifndef KERNEL_CPU_CPUDATA_IMPL_H_
#define KERNEL_CPU_CPUDATA_IMPL_H_

#ifndef __ASSEMBLER__

#include <stddef.h>

extern char __cpudata_start, __cpudata_block_end, __cpudata_end;

#define __CPUDATA_START     ((char *)&__cpudata_start)
#define __CPUDATA_BLOCK_END ((char *)&__cpudata_block_end)
#define __CPUDATA_BLOCK_SZ  ((size_t)(__CPUDATA_BLOCK_END - __CPUDATA_START))
#define __CPUDATA_END       ((char *)&__cpudata_end)

#define __cpudata__         __attribute__((section(".embox.cpudata.data")))

#define __cpudata_cpu_ptr(cpu_id, name) \
	((__typeof__(name))(((char *)(name)) + (cpu_id * __CPUDATA_BLOCK_SZ)))

#endif /* !__ASSEMBLER__ */

#endif /* !KERNEL_CPU_CPUDATA_IMPL_H_ */
