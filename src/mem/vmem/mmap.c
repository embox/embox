#include <errno.h>
#include <stddef.h>
#include <sys/mman.h>


extern void *mmap_userspace_add(void *addr, size_t len, int prot);

void *mmap(void *addr, size_t len, int prot, int flags, int fd, off_t off) {
	if (len == 0) {
		SET_ERRNO(EINVAL);
		return MAP_FAILED;
	}

	if (flags & MAP_ANONYMOUS) {
		void *ptr = mmap_userspace_add(addr, len, prot);

		if (ptr == NULL) {
			errno = EPERM;
		}

		return MAP_FAILED;
	} else {
		SET_ERRNO(ENOTSUP);
		return MAP_FAILED;
	}
}
