/**
 * @file
 * @brief
 *
 * @date Mar 13, 2018
 * @author Anton Bondarev
 */

#include <stdint.h>
#include <inttypes.h>
#include <kernel/printk.h>

unsigned long cpuSyscall(int number, uint64_t arg1, uint64_t arg2,
			 uint64_t arg3, uint64_t arg4)
{
	printk("syscall handler %d (%"PRIu64", %"PRIu64", %"PRIu64", %"PRIu64")\n", number, arg1, arg2, arg3, arg4);

	return 0;
}


unsigned long cpuJmpSyscall(int number, uint64_t arg1, uint64_t arg2,
			 uint64_t arg3, uint64_t arg4)
{
	printk("jmpsyscall handler %d (%"PRIu64", %"PRIu64", %"PRIu64", %"PRIu64")\n", number, arg1, arg2, arg3, arg4);

	return 0;
}
