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

#define PROT_NONE  0x0
#define PROT_READ  0x1
#define PROT_WRITE 0x2
#define PROT_EXEC  0x4

#define MAP_SHARED  0x1
#define MAP_PRIVATE 0x2
#define MAP_FIXED   0x4
#define MAP_ANONYMOUS 0x8

#define MAP_FAILED (void*)-1

extern void  *mmap(void *, size_t, int, int, int, off_t);
extern int    mprotect(void *, size_t, int);
extern int    munmap(void *, size_t);

__END_DECLS

#endif /* COMPAT_POSIX_SYS_MMAN_H_ */
