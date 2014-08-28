/**
 * @file
 *
 * @date Aug 28, 2014
 * @author: Anton Bondarev
 */

#include <sys/mman.h>
#include <errno.h>

void *mmap(void *addr, size_t len, int prot, int flags, int fd, off_t off) {
	// ToDo: implement for InitFS files
	(void)addr;
	(void)len;
	(void)prot;
	(void)flags;
	(void)fd;
	(void)off;

	errno = EPERM;
	return NULL;
}

int munmap(void *addr, size_t size) {
	(void)size;

	errno = EPERM;
	return -1;
}
