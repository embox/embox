/**
 * @file
 *
 * @date Aug 12, 2014
 * @author: Anton Bondarev
 */


#include <stddef.h>
#include <sys/mman.h>

#include <kernel/task/resource/mmap.h>
#include <mem/mapping/marea.h>
#include <mem/mmap.h>

extern int mmap_kernel_inited(void);

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

	marea = marea_create(physical, len + physical, prot);
	if (NULL == marea) {
		return NULL;
	}
	mmap_add_marea(emmap, marea);

	if (mmap_kernel_inited()) {
		mmap_do_marea_map(emmap, marea);
	}
	return addr;
}
