/**
 * @file
 * @brief
 *
 * @date 10.10.2012
 * @author Anton Bulychev
 */

#include <types.h>
#include <unistd.h>
#include <kernel/task.h>
#include <kernel/irq.h>

#define SYSCALL_NRS_TOTAL 5

long sys_exit(int error_code) {
	ipl_enable();
	task_exit(NULL);
}

size_t sys_write(int fd, const void *buf, size_t nbyte) {
	return write(fd, buf, nbyte);
}

void *SYSCALL_TABLE[SYSCALL_NRS_TOTAL] = {
	NULL,
	sys_exit,
	NULL,
	NULL,
	sys_write
};
