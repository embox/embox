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
#include <stdint.h>
#include <sys/types.h>

#include <sys/cdefs.h>
__BEGIN_DECLS

#define PROT_NONE     0x00
#define PROT_READ     0x01
#define PROT_WRITE    0x02
#define PROT_EXEC     0x04

#define MAP_SHARED    0x10
#define MAP_PRIVATE   0x20
#define MAP_FIXED     0x40
#define MAP_ANONYMOUS 0x80

#define MAP_FAILED (void*)-1

#if 1
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



#endif

extern void  *mmap(void *, size_t, int, int, int, off_t);
extern int    mprotect(void *, size_t, int);

/* QNX */

#define PROT_NOCACHE 0x80

/* TODO move to compat/qnx */
#include <module/embox/mem/vmem_api.h>
extern void *mmap_device_memory(void * addr,
                           size_t len,
                           int prot,
                           int flags,
                           uint64_t physical);
extern int munmap(void *, size_t);

__END_DECLS

#endif /* COMPAT_POSIX_SYS_MMAN_H_ */
