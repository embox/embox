/**
 * @file
 * @brief
 *
 * @date 10.10.2012
 * @author Anton Bulychev
 */

#include <types.h>
#include <unistd.h>


#define SYSCALL_NRS_TOTAL 5

size_t sys_write(int fd, const void *buf, size_t nbyte) {
	return write(fd, buf, nbyte);
}

void *SYSCALL_TABLE[SYSCALL_NRS_TOTAL] = {
	NULL,
	NULL,
	NULL,
	NULL,
	sys_write
};
