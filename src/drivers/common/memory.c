/**
 * @file memory.c
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 2016-08-11
 */

#include <errno.h>
#include <stdlib.h>
#include <sys/mman.h>

#include <module/embox/arch/mmu.h>

#ifndef NOMMU

#include <drivers/common/memory.h>
#include <hal/mmu.h>
#include <kernel/task/resource/mmap.h>
#include <mem/mapping/marea.h>
#include <mem/misc/pool.h>
#include <mem/mmap.h>
#include <embox/unit.h>

ARRAY_SPREAD_DEF(struct periph_memory_desc *, __periph_mem_registry);

EMBOX_UNIT_INIT(periph_memory_init);

static struct _segment {
	uint32_t start;
	uint32_t end;
} _segments[64];

static struct emmap *self_mmap(void) {
	if (0 == mmap_kernel_inited()) {
		return mmap_early_emmap();
	} else {
		return task_self_resource_mmap();
	}
}

static int _seg_cmp(const void *fst, const void *snd) {
	return ((struct _segment *) fst)->start -
		((struct _segment *) snd)->start;
}

/**
 * @brief Group pripheral memory into marea's
 */
static int periph_memory_init(void) {
	struct periph_memory_desc *desc;
	int seg_cnt = 0;

	array_spread_foreach(desc, __periph_mem_registry) {
		_segments[seg_cnt++] = (struct _segment) {
			.start = desc->start / MMU_PAGE_SIZE,
			.end   = (desc->start + desc->len + MMU_PAGE_MASK) / MMU_PAGE_SIZE,
		};
	}

	qsort(&_segments, seg_cnt, sizeof(struct _segment), _seg_cmp);

	struct emmap *emmap = self_mmap();
	int l = 0, r = 1;

	while (l < seg_cnt) {
		uint32_t addr_start = _segments[l].start;
		uint32_t addr_end = _segments[r - 1].end;
		while (r < seg_cnt &&
			(_segments[r].start <= addr_end)) {
			if (_segments[r].end > addr_end)
				addr_end = _segments[r].end;
			r++;
		}

		addr_start *= MMU_PAGE_SIZE;
		addr_end   *= MMU_PAGE_SIZE;

		struct marea *marea = marea_create(
			addr_start,
			addr_end,
			PROT_READ | PROT_WRITE | PROT_NOCACHE,
			false
		);

		if (NULL == marea) {
			return -ENOMEM;
		}

		mmap_add_marea(emmap, marea);

		if (mmap_kernel_inited()) {
			mmap_do_marea_map(emmap, marea);
		}

		l = r;
		r = l + 1;
	}

	return 0;
}

#endif /* NOMMU */
