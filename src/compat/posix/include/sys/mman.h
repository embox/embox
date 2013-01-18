/**
 * @file
 *
 * @brief POSIX memory management declarations
 *
 * @details http://pubs.opengroup.org/onlinepubs/009604499/basedefs/sys/mman.h.html
 *
 * @date Jan 17, 2013
 * @author: Anton Bondarev
 */

#ifndef POSIX_SYS_MMAN_H_
#define POSIX_SYS_MMAN_H_

#include <sys/types.h>

#include <sys/cdefs.h>
__BEGIN_DECLS


/* The following flag options shall be defined: */
#define MAP_SHARED    0x00   /* Share changes. */
#define MAP_PRIVATE   0x01   /* Changes are private. */
#define MAP_FIXED     0x02   /* Interpret addr exactly. */

/*
 * If one or more of the Advisory Information, Memory Mapped Files, or
 * Shared Memory Objects options are supported, the following protection
 * options shall be defined:
 */
#define PROT_READ     0x10   /* Page can be read. */
#define PROT_WRITE    0x20   /* Page can be written. */
#define PROT_EXEC     0x40   /* Page can be executed. */
#define PROT_NONE     0x00    /* page can not be accessed */


/* The following flags shall be defined for msync(): */
#define MS_ASYNC      0x0100 /* Perform asynchronous writes. */
#define MS_SYNC       0x0200 /* Perform synchronous writes. */
#define MS_INVALIDATE 0x0400 /* Invalidate mappings. */

/*The following symbolic constants shall be defined for the mlockall() function:*/
#define MCL_CURRENT   1 /* Lock currently mapped pages. */
#define MCL_FUTURE    2 /* Lock pages that become mapped. */

/*The symbolic constant MAP_FAILED shall be defined to indicate a failure from the mmap() function. */
#define MAP_FAILED    (-1)

/* If the Advisory Information and either the Memory Mapped Files or
 * Shared Memory Objects options are supported, values for advice used by
 * posix_madvise() shall be defined as follows:
 */
#define POSIX_MADV_NORMAL     0 /* The application has no advice to give on its behavior with respect to the specified range. It is the default characteristic if no advice is given for a range of memory. */
#define POSIX_MADV_SEQUENTIAL 1 /* The application expects to access the specified range sequentially from lower addresses to higher addresses. */
#define POSIX_MADV_RANDOM     2 /* The application expects to access the specified range in a random order. */
#define POSIX_MADV_WILLNEED   3 /* The application expects to access the specified range in the near future. */
#define POSIX_MADV_DONTNEED   4 /* The application expects that it will not access the specified range in the near future. */

/* The following flags shall be defined for posix_typed_mem_open(): */
#define POSIX_TYPED_MEM_ALLOCATE         0x1 /* Allocate on mmap(). */
#define POSIX_TYPED_MEM_ALLOCATE_CONTIG  0x2 /* Allocate contiguously on mmap(). */
#define POSIX_TYPED_MEM_MAP_ALLOCATABLE  0x4 /* Map on mmap(), without affecting allocatability. */

struct posix_typed_mem_info {
	size_t  posix_tmi_length; /* Maximum length which may be allocated from a typed memory object.*/
};

extern int    mlock(const void *, size_t);

extern int    mlockall(int);
extern void  *mmap(void *, size_t, int, int, int, off_t);
extern int    mprotect(void *, size_t, int);
extern int    msync(void *, size_t, int);
extern int    munlock(const void *, size_t);
extern int    munlockall(void);
extern int    munmap(void *, size_t);
extern int    posix_madvise(void *, size_t, int);
extern int    posix_mem_offset(const void *, size_t, off_t *,
           size_t *, int *);
extern int    posix_typed_mem_get_info(int, struct posix_typed_mem_info *);
extern int    posix_typed_mem_open(const char *, int, int);
extern int    shm_open(const char *, int, mode_t);
extern int    shm_unlink(const char *);

__END_DECLS

#endif /* POSIX_SYS_MMAN_H_ */
