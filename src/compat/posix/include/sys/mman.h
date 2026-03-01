/**
 * @file
 * @brief mman - memory management declarations
 *
 * @date 09.06.14
 * @author Ilia Vaprol
 */

#ifndef COMPAT_POSIX_SYS_MMAN_H_
#define COMPAT_POSIX_SYS_MMAN_H_

#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/cdefs.h>
#include <sys/types.h>

/* Values of the 'prot' arg of mmap() and mprotect() */
#define PROT_NONE    0x00     /* Page cannot be accessed */
#define PROT_READ    (1 << 0) /* Page can be read */
#define PROT_WRITE   (1 << 1) /* Page can be written */
#define PROT_EXEC    (1 << 2) /* Page can be executed */
#define PROT_NOCACHE (1 << 3) /* Page should not be cached */

/* Values of the 'flags' arg of mmap() */
#define MAP_SHARED     (1 << 0) /* Share this mapping */
#define MAP_PRIVATE    (1 << 1) /* Create a private copy-on-write mapping */
#define MAP_FIXED      (1 << 2) /* Map to specified address exactly */
#define MAP_ANONYMOUS  (1 << 3) /* The mapping is not backed by any file */
#define MAP_ANON       MAP_ANONYMOUS
#define MAP_TYPE       (MAP_SHARED | MAP_PRIVATE) /* Mask for type of mapping */
#define MAP_FILE       (1 << 4)  /* The mapping is backed by a file */
#define MAP_GROWSDOWN  (1 << 5)  /* Used to stack allocations */
#define MAP_DENYWRITE  (1 << 6)  /* Do not permit writes to file */
#define MAP_EXECUTABLE (1 << 7)  /* Mark it as an executable */
#define MAP_LOCKED     (1 << 8)  /* Lock pages mapped into memory */
#define MAP_NORESERVE  (1 << 9)  /* Don't check for reservations */
#define MAP_POPULATE   (1 << 10) /* Populate (prefault) page tables */
#define MAP_NONBLOCK   (1 << 11) /* Do not block on IO */

/* Return value of mmap() in case of an error */
#define MAP_FAILED ((void *)-1)

/* Values of the 'flags' arg of msync() */
#define MS_ASYNC      (1 << 0) /* Perform asynchronous writes */
#define MS_SYNC       (1 << 1) /* Perform synchronous writes */
#define MS_INVALIDATE (1 << 2) /* Invalidate mapping */

/* Values of the 'flags' arg of mlockall() */
#define MCL_CURRENT (1 << 0) /* Lock currently mapped pages */
#define MCL_FUTURE  (1 << 1) /* Lock pages that become mapped */

/* Values of the 'advice' arg of madvise() */
#define MADV_NORMAL     0
#define MADV_SEQUENTIAL 1
#define MADV_RANDOM     2
#define MADV_WILLNEED   3
#define MADV_DONTNEED   4

/* Values of the 'advice' arg of posix_madvise() */
#define POSIX_MADV_NORMAL     MADV_NORMAL
#define POSIX_MADV_SEQUENTIAL MADV_SEQUENTIAL
#define POSIX_MADV_RANDOM     MADV_RANDOM
#define POSIX_MADV_WILLNEED   MADV_WILLNEED
#define POSIX_MADV_DONTNEED   MADV_DONTNEED

__BEGIN_DECLS

extern int munmap(void *start, size_t length);
extern void *mmap(void *start, size_t length, int prot, int flags, int fd,
    off_t offset);

extern int mprotect(void *addr, size_t len, int prot);

extern int shm_open(const char *name, int oflag, mode_t mode);
extern int shm_unlink(const char *name);

__END_DECLS

static inline int mlock(const void *addr, size_t len) {
	return 0;
}

static inline int munlock(const void *addr, size_t len) {
	return 0;
}

static inline int mlockall(int flags) {
	return 0;
}

static inline int munlockall(void) {
	return 0;
}

static inline int msync(void *addr, size_t len, int flags) {
	return 0;
}

static inline int madvise(void *addr, size_t len, int advice) {
	return 0;
}

static inline int posix_madvise(void *addr, size_t len, int advice) {
	return 0;
}

#endif /* COMPAT_POSIX_SYS_MMAN_H_ */
