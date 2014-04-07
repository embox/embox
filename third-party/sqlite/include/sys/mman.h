#ifndef SQLITE_SYS_MMAN_H_
#define SQLITE_SYS_MMAN_H_

#include <stddef.h>
#include <sys/types.h>

#define PROT_READ  0x10
#define PROT_WRITE 0x20
#define MAP_SHARED 0x00
#define MAP_FAILED ((void*)((unsigned long)-1))

extern void * mmap(void *addr, size_t len, int prot, int flags,
		int fildes, off_t off);
extern int munmap(void *addr, size_t len);

#include_next <sys/mman.h>

#endif /* SQLITE_SYS_MMAN_H_ */
