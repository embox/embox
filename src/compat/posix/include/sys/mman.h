/**
 * @file
 * @brief
 *
 * @date 09.06.14
 * @author Ilia Vaprol
 */

#ifndef COMPAT_POSIX_SYS_MMAN_H_
#define COMPAT_POSIX_SYS_MMAN_H_

#include <stddef.h>

#include <sys/cdefs.h>
__BEGIN_DECLS

#define PROT_NONE     0x00
#define PROT_READ     0x10
#define PROT_WRITE    0x20
#define PROT_EXEC     0x40

#define MAP_SHARED    0x1
#define MAP_PRIVATE   0x2
#define MAP_FIXED     0x4
#define MAP_ANONYMOUS 0x8

#define MAP_FAILED (void*)-1

//extern void  *mmap(void *, size_t, int, int, int, off_t);
extern int    mprotect(void *, size_t, int);
//extern int    munmap(void *, size_t);
#include <errno.h>

static inline void  *mmap(void *addr, size_t len, int prot, int flags, int fd, off_t off) {
	// ToDo: implement for InitFS files
	(void)addr;
	(void)len;
	(void)prot;
	(void)flags;
	(void)fd;
	(void)off;
	//printf(">>> mmap(%i)\n",fd);
	errno = EPERM;
	return NULL;
}
static inline int munmap(void *addr, size_t size) {
	(void)size;
	printf(">>> munmap(%p)\n",addr);
	errno = EPERM;
	return -1;
}

__END_DECLS

#endif /* COMPAT_POSIX_SYS_MMAN_H_ */
