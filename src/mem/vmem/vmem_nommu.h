/**
 * @file
 *
 * @date Aug 6, 2014
 * @author: Anton Bondarev
 */

#ifndef VMEM_NOMMU_H_
#define VMEM_NOMMU_H_

static inline void *mmap_device_memory(void *addr,
                           size_t len,
                           int prot,
                           int flags,
                           uint64_t physical){

	return addr;
}

static inline int munmap(void *addr, size_t size) {
	return 0;
}

static inline void *mmap(void *addr, size_t len, int prot, int flags, int fd, off_t off) {
	return NULL;
}

#endif /* VMEM_NOMMU_H_ */
