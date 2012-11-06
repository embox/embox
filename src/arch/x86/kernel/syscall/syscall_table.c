/**
 * @file
 * @brief
 *
 * @date 10.10.2012
 * @author Anton Bulychev
 */

#include <types.h>

#include <kernel/syscall.h>

#define SYSCALL_NRS_TOTAL 5

void *SYSCALL_TABLE[SYSCALL_NRS_TOTAL] = {
	NULL,
	sys_exit,
	NULL,
	NULL,
	sys_write
};
