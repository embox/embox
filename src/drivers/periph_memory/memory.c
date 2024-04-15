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

#include <mem/vmem.h>
#include <mem/sysmalloc.h>
#include <module/embox/arch/mmu.h>

#ifndef NOMMU

#include <drivers/common/memory.h>
#include <hal/mmu.h>
#include <kernel/task/kernel_task.h>
#include <kernel/task/resource/mmap.h>
#include <mem/misc/pool.h>
#include <mem/mmap.h>
#include <embox/unit.h>
#include <lib/libds/array.h>

ARRAY_SPREAD_DEF(struct periph_memory_desc *, __periph_mem_registry);

EMBOX_UNIT_INIT(periph_memory_init);

struct _segment {
	uintptr_t start;
	uintptr_t end;
};
static struct _segment _segments[PERIPH_MAX_SEGMENTS];

static int _seg_cmp(const void *fst, const void *snd) {
	return ((struct _segment *) fst)->start -
		((struct _segment *) snd)->start;
}

/**
 * @brief Group peripheral memory into mmap-s
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

	int l = 0, r = 1;

	while (l < seg_cnt) {
		uintptr_t addr_start;
		uintptr_t addr_end;

		addr_start = _segments[l].start;
		addr_end = _segments[r - 1].end;

		while (r < seg_cnt &&
			(_segments[r].start <= addr_end)) {
			if (_segments[r].end > addr_end)
				addr_end = _segments[r].end;
			r++;
		}

		addr_start *= MMU_PAGE_SIZE;
		addr_end   *= MMU_PAGE_SIZE;

		if (mmap_place(task_resource_mmap(task_kernel_task()), addr_start,
					addr_end - addr_start,
					PROT_READ | PROT_WRITE | PROT_NOCACHE)) {
			return -ENOMEM;
		}

		l = r;
		r = l + 1;
	}

	return 0;
}

int periph_desc(struct periph_memory_desc **buff) {
	struct periph_memory_desc *elem;

	int i = 0;
	array_spread_foreach(elem, __periph_mem_registry) {
		buff[i++] = elem;
	}

	return i;
}

#endif /* NOMMU */
