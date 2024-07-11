/**
 * @file
 * @brief
 *
 * @date 09.06.14
 * @author Ilia Vaprol
 */

#ifndef COMPAT_POSIX_SYS_MMAN_H_
#define COMPAT_POSIX_SYS_MMAN_H_

#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <mem/page.h>
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

extern int mprotect(void *, size_t, int);


/* todo: implement for InitFS files */
extern void *mmap(void *, size_t, int, int, int, off_t);
extern int munmap(void *, size_t);

extern int shm_open(const char *name, int oflag, mode_t mode);
extern int shm_unlink(const char *name);

/* The following flags shall be defined for msync(): */
#define MS_ASYNC       0x100 /* Perform asynchronous writes. */
#define MS_SYNC        0x200 /* Perform synchronous writes. */
#define MS_INVALIDATE  0x400 /* Invalidate mappings. */

extern int msync(void *addr, size_t len, int flags);

/* QNX */

#define PROT_NOCACHE 0x80

/* TODO move to compat/qnx */
extern void *mmap_device_memory(void *addr, size_t len, int prot, int flags,
    uintptr_t physical);

__END_DECLS

#endif /* COMPAT_POSIX_SYS_MMAN_H_ */
