#ifndef SQLITE_SYS_MMAN_H_
#define SQLITE_SYS_MMAN_H_

#include <stddef.h>
#include <sys/types.h>

#define PROT_READ      0x01
#define PROT_WRITE     0x02
#define MAP_SHARED     0x01
#define MAP_FAILED     ((void *)((unsigned long)-1UL))
#define MREMAP_MAYMOVE 0x01

extern void * mmap(void *addr, size_t len, int prot, int flags,
		int fildes, off_t off);
extern void * mremap(void *old_addr, size_t old_len, size_t new_len,
		int flags, ... /* void *new_addr */);
extern int munmap(void *addr, size_t len);

#include_next <sys/mman.h>

#endif /* SQLITE_SYS_MMAN_H_ */
