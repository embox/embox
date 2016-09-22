/**
 * @file
 *
 * @date 21.09.2016
 * @author Anton Bondarev
 */
#include <errno.h>
#include <stdio.h>

#include <sys/mman.h>

#include <util/log.h>

int msync(void *addr, size_t length, int flags) {
	(void)addr;
	(void)length;
	(void)flags;
	log_debug(">>> msync(%p)", addr);
	errno = ENOMEM;
	return -1;
}
