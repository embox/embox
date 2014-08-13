/**
 * @file
 *
 * @date Aug 12, 2014
 * @author: Anton Bondarev
 */


#include <stddef.h>
#include <sys/mman.h>

#include <kernel/task/resource/mmap.h>

extern int mmap_kernel_inited(void);
extern struct marea *marea_create(uint32_t start, uint32_t end, uint32_t flags);
extern struct emmap *mmap_early_emmap(void);
extern void mmap_add_marea(struct emmap *mmap, struct marea *marea);

void *mmap_device_memory(void *addr,
                           size_t len,
                           int prot,
                           int flags,
                           uint64_t physical){
	struct emmap *emmap;
	struct marea *marea;

	if (0 == mmap_kernel_inited()) {
		emmap = mmap_early_emmap();
	} else {
		emmap = task_self_resource_mmap();
	}

	marea = marea_create(physical, len + physical, flags);
	if (NULL == marea) {
		return NULL;
	}
	mmap_add_marea(emmap, marea);

	return addr;
}
