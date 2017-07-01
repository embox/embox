/**
 * @file
 *
 * @date Aug 6, 2014
 * @author: Anton Bondarev
 */

#ifndef VMEM_NOMMU_H_
#define VMEM_NOMMU_H_

#include <stddef.h>
#include <stdint.h>

static inline void *mmap_device_memory(void *addr,
                           size_t len,
                           int prot,
                           int flags,
                           uint64_t physical){
	(void) addr;
	(void) len;
	(void) prot;
	(void) flags;
	(void) physical;

	return addr;
}

static inline int munmap(void *addr, size_t size) {
	(void) addr;
	(void) size;

	return 0;
}

#include <module/embox/fs/syslib/idesc_mmap.h>

static inline void *mmap(void *addr, size_t len, int prot, int flags, int fd, off_t off) {
	(void) addr;
	(void) len;
	(void) prot;
	(void) flags;
	(void) fd;
	(void) off;

	if(fd) {
		return idesc_mmap(addr, len, prot, flags, fd, off);
	}
	return NULL;
}

#endif /* VMEM_NOMMU_H_ */
