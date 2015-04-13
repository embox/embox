/**
 * @file
 *
 * @date Aug 28, 2014
 * @author: Anton Bondarev
 */

#include <sys/mman.h>
#include <errno.h>
#include <stdint.h>
#include <mem/phymem.h>

#if 0
extern void *mmap_userspace_add(void *addr, size_t len, int prot);

void *mmap(void *addr, size_t len, int prot, int flags, int fd, off_t off) {
	mmap_userspace_add(addr, len, prot);
	return addr;
}

int munmap(void *addr, size_t size) {
	(void)size;

	errno = EPERM;
	return -1;
}
#endif
