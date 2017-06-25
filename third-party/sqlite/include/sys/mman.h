#ifndef SQLITE_SYS_MMAN_H_
#define SQLITE_SYS_MMAN_H_

#include <stddef.h>
#include <sys/types.h>


#define MREMAP_MAYMOVE 0x01

extern void * mremap(void *old_addr, size_t old_len, size_t new_len,
		int flags, ... /* void *new_addr */);

#include_next <sys/mman.h>

#endif /* SQLITE_SYS_MMAN_H_ */
