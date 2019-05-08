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

void *mmap(void *addr, size_t len, int prot, int flags, int fd, off_t off) {
	return addr;
}

int munmap(void *addr, size_t size) {
	return 0;
}
