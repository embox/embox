/**
 * @file
 * @brief
 *
 * @date 18.07.13
 * @author Ilia Vaprol
 */

#ifndef KERNEL_SMP_PERCPU_IMPL_H_
#define KERNEL_SMP_PERCPU_IMPL_H_

#ifndef __ASSEMBLER__

#include <stddef.h>

extern char _percpu_start, _percpu_block_end, _percpu_end;

#define __PERCPU_START     ((char *)&_percpu_start)
#define __PERCPU_BLOCK_END ((char *)&_percpu_block_end)
#define __PERCPU_BLOCK_SZ  ((size_t)(__PERCPU_BLOCK_END - __PERCPU_START))
#define __PERCPU_END       ((char *)&_percpu_end)

#define __percpu__       __attribute__((section(".percpu.data")))

#define __percpu_cpu_ptr(cpu_id, name) \
	((typeof(name))(((char *)(name)) + (cpu_id * __PERCPU_BLOCK_SZ)))
//		assert((__PERCPU_START <= (char *)(name))
//				&& ((char *)(name) < __PERCPU_BLOCK_END));

#endif /* !__ASSEMBLER__ */

#endif /* !KERNEL_SMP_PERCPU_IMPL_H_ */
