/**
 * @file
 * @brief
 *
 * @date 06.11.2012
 * @author Anton Bulychev
 */

#include <types.h>
#include <unistd.h>
#include <kernel/task.h>
#include <kernel/irq.h>

long sys_exit(int error_code) {
	ipl_enable();
	task_exit(NULL);
}

size_t sys_write(int fd, const void *buf, size_t nbyte) {
	ipl_enable();
	return write(fd, buf, nbyte);
}

