/**
 * @file mmap.c
 * @brief Various memory mapping
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 28.02.2018
 */

#include <errno.h>
#include <stddef.h>
#include <sys/mman.h>

#include <util/log.h>

#include <module/embox/fs/syslib/idesc_mmap_api.h>

void *mmap(void *addr, size_t len, int prot, int flags, int fd, off_t off) {
	if (fd > 0) {
		/* Call device-specific handler.
		 * Working with plain files is not supported for now */
		return idesc_mmap(addr, len, prot, flags, fd, off);
	}
	return addr;
}

int munmap(void *addr, size_t size) {
	return 0;
}
