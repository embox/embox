/**
 * @file
 * @brief Heap implementation based on boundary markers algorithm.
 * @details
 *    Segment structure:
 *    |struct mm_segment| *** space for bm ***|
 *
 *    TODO:
 *    Should be improved by usage of page_alloc when size is divisible by PAGE_SIZE()
 *    Also SLAB allocator can be used when size is 16, 32, 64, 128...
 *
 * @date 04.03.2014
 * @author Alexander Kalmuk
 */

#include <util/err.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include <mem/heap_bm.h>
#include <mem/page.h>

#include <lib/libds/dlist.h>
#include <lib/libds/array.h>
#include <util/log.h>

#include <kernel/printk.h>
#include <kernel/panic.h>

#include <mem/heap/mspace_malloc.h>

#include <kernel/sched/sched_lock.h>

/* TODO make it per task field */
//static DLIST_DEFINE(task_mem_segments);

//#define DEBUG

extern struct page_allocator *__heap_pgallocator;
extern struct page_allocator *__heap_pgallocator2 __attribute__((weak));
extern struct page_allocator *__heap_fixed_pgallocator __attribute__((weak));

struct mm_heap_allocator {
	struct page_allocator **pg_allocator;
	heap_type_t type;
};

static struct mm_heap_allocator const mm_page_allocs[] = {
	{ &__heap_pgallocator, HEAP_RAM },
	{ &__heap_pgallocator2, HEAP_FAST_RAM },
	{ &__heap_fixed_pgallocator, HEAP_EXTERN_MEM },
};

static struct mm_heap_allocator const *mm_cur_allocator =
	&mm_page_allocs[0];

static void *mm_segment_alloc(int page_cnt) {
	void *ret;
	int i;

	ret = page_alloc(*mm_cur_allocator->pg_allocator, page_cnt);
	if (ret) {
		return ret;
	}

	/* Requsted memory wasn't allocated by mm_cur_allocator->pg_allocator above,
	 * because due to there is no more free memory. Try find new allocator */
	for (i = 0; i < ARRAY_SIZE(mm_page_allocs); i++) {
		if (mm_page_allocs[i].pg_allocator
				&& *mm_page_allocs[i].pg_allocator) {
			ret = page_alloc(*mm_page_allocs[i].pg_allocator, page_cnt);
			if (ret) {
				mm_cur_allocator = &mm_page_allocs[i];
				break;
			}
		}
	}

	return ret;
}

/* XXX This functionality is experimental and currently only used
 * in PISJP (stm32f7-discovery). Please, be careful if you want to use
 * this function. */
int mspace_set_heap(heap_type_t type, heap_type_t *prev_type) {
	if (prev_type) {
		*prev_type = mm_cur_allocator->type;
	}

	switch (type) {
	case HEAP_FAST_RAM:
	case HEAP_RAM:
	case HEAP_EXTERN_MEM:
		if (!mm_page_allocs[type].pg_allocator) {
			return -1;
		}
		mm_cur_allocator = &mm_page_allocs[type];
		break;
	default:
		log_error("Unknown heap type - %d\n", type);
		return -1;
	}

	return 0;
}

static void mm_segment_free(void *segment, int page_cnt) {
	int i;
	for (i = 0; i < ARRAY_SIZE(mm_page_allocs); i++) {
		if (mm_page_allocs[i].pg_allocator &&
				page_belong(*mm_page_allocs[i].pg_allocator, segment)) {
			page_free(*mm_page_allocs[i].pg_allocator, segment, page_cnt);
			break;
		}
	}
}

struct mm_segment {
	struct dlist_head link;
	size_t size;
};

static inline int pointer_inside_segment(void *segment, size_t size, void *pointer) {
	return (pointer > segment && pointer < (segment + size));
}

static inline void *mm_to_segment(struct mm_segment *mm) {
	assert(mm);
	return ((char *) mm + sizeof *mm);
}

static void *pointer_to_mm(void *ptr, struct dlist_head *mspace) {
	struct mm_segment *mm;

	assert(ptr);
	assert(mspace);

	dlist_foreach_entry(mm, mspace, link) {
		if (pointer_inside_segment(mm_to_segment(mm), mm->size, ptr)) {
			return mm;
		}
	}

	return NULL;
}

static void *mspace_do_alloc(size_t boundary, size_t size, struct dlist_head *mspace) {
	struct mm_segment *mm;
	dlist_foreach_entry(mm, mspace, link) {
		void *block = bm_memalign(mm_to_segment(mm), boundary, size);
		if (block != NULL) {
			return block;
		}
	}

	return NULL;
}

/*
 * It's basically required for more efficient memory consuming.
 * It's more efficient to allocate many pages at once rather than
 * many small pieces.
 **/
#if OPTION_GET(BOOLEAN, task_is_greed)
static struct mm_segment *mm_try_alloc_segment(size_t size, size_t boundary) {
	/* Try to allocate as much size as possible starting with 1Mb.
	 * Why 1Mb? Don't know, magical number.. Since it should be maximum size
	 * of all available heaps in Embox, but it's more difficult to implement. */
	const size_t max_size = (0x100000 + PAGE_SIZE()) / PAGE_SIZE();
	const size_t min_size = (size + boundary + PAGE_SIZE()) / PAGE_SIZE();
	size_t i;
	struct mm_segment *mm = NULL;

	for (i = max_size; i > min_size; i--) {
		mm = mm_segment_alloc(i);
		if (mm) {
			break;
		}
	}
	if (mm) {
		mm->size = i * PAGE_SIZE();
	}
	return mm;
}
#else
static struct mm_segment *mm_try_alloc_segment(size_t size, size_t boundary) {
	size_t pages_cnt;
	struct mm_segment *mm = NULL;

	pages_cnt = size / PAGE_SIZE() + boundary / PAGE_SIZE();
	pages_cnt += (size % PAGE_SIZE() + boundary % PAGE_SIZE()
			+ 2 * PAGE_SIZE()) / PAGE_SIZE();
	mm = mm_segment_alloc(pages_cnt);
	if (mm) {
		mm->size = pages_cnt * PAGE_SIZE();
	}
	return mm;
}
#endif

void *mspace_memalign(size_t boundary, size_t size, struct dlist_head *mspace) {
	/* No corresponding heap was found */
	struct mm_segment *mm;
	void *block = NULL;

	if (size == 0) {
		return NULL;
	}

	sched_lock();

	assert(mspace);

	block = mspace_do_alloc(boundary, size, mspace);
	if (block) {
		goto out_unlock;
	}

	mm = mm_try_alloc_segment(size, boundary);
	if (mm == NULL) {
		goto out_unlock;
	}
	dlist_head_init(&mm->link);
	dlist_add_next(&mm->link, mspace);

	bm_init(mm_to_segment(mm), mm->size - sizeof(struct mm_segment));

	block = mspace_do_alloc(boundary, size, mspace);
	if (!block) {
		panic("new memory block is not sufficient to allocate requested size");
	}

out_unlock:
	sched_unlock();

	return block;
}

void *mspace_malloc(size_t size, struct dlist_head *mspace) {
	assert(mspace);
	return mspace_memalign(8, size, mspace);
}

int mspace_free(void *ptr, struct dlist_head *mspace) {
	int res = 0;
	struct mm_segment *mm;

	assert(ptr);
	assert(mspace);

	sched_lock();

	mm = pointer_to_mm(ptr, mspace);

	if (mm != NULL) {
		void *segment;

		segment = mm_to_segment(mm);
		bm_free(segment, ptr);

		if (bm_heap_is_empty(segment)) {
			mm_segment_free(mm, mm->size / PAGE_SIZE());
			dlist_del(&mm->link);
		}
	} else {
		/* No segment containing pointer @c ptr was found. */
		res = -1;
#ifdef DEBUG
		printk("***** free(): incorrect address space\n");
#endif
		goto out;
	}

out:
	sched_unlock();

	return res;
}

void *mspace_realloc(void *ptr, size_t size, struct dlist_head *mspace) {
	void *ret;

	assert(mspace);
	assert(size != 0 || ptr == NULL);

	ret = mspace_memalign(8, size, mspace);

	if (ret == NULL) {
		return NULL; /* error: errno set in malloc */
	}

	if (ptr == NULL) {
		return ret;
	}

	/* The content of new region will be unchanged in the range from the start of the region up to
	 * the minimum of the old and new sizes. So simply copy size bytes (may be with redundant bytes) */
	memcpy(ret, ptr, size);
	if (0 > mspace_free(ptr, mspace)) {
		mspace_free(ret, mspace);
		return err2ptr(EINVAL);
	}

	return ret;
}

void *mspace_calloc(size_t nmemb, size_t size, struct dlist_head *mspace) {
	void *ret;
	size_t total_size;

	total_size = nmemb * size;

	assert(mspace);
	assert(total_size > 0);

	ret = mspace_malloc(total_size, mspace);
	if (ret == NULL) {
		return NULL; /* error: errno set in malloc */
	}

	memset(ret, 0, total_size);
	return ret;
}

int mspace_init(struct dlist_head *mspace) {
	dlist_init(mspace);
	return 0;
}

int mspace_fini(struct dlist_head *mspace) {
	struct mm_segment *mm = NULL;

	dlist_foreach_entry(mm, mspace, link) {
		mm_segment_free(mm, mm->size / PAGE_SIZE());
	}

	return 0;
}

size_t mspace_deep_copy_size(struct dlist_head *mspace) {
	struct mm_segment *mm = NULL;
	size_t ret;

	ret = 0;
	dlist_foreach_entry(mm, mspace, link) {
		ret += mm->size;
	}
	return ret;
}


void mspace_deep_store(struct dlist_head *mspace, struct dlist_head *store_space, void *buf) {
	struct mm_segment *mm = NULL;
	void *p;

	dlist_init(store_space);

	/* if mspace is empty list manipulation is illegal */
	if (dlist_empty(mspace)) {
		return;
	}

	dlist_del(mspace);
	dlist_add_prev(store_space, mspace->next);

	p = buf;
	dlist_foreach_entry(mm, store_space, link) {
		memcpy(p, mm, mm->size);
		p += mm->size;
	}

	dlist_del(store_space);
	dlist_add_prev(mspace, mspace->next);
}

void mspace_deep_restore(struct dlist_head *mspace, struct dlist_head *store_space, void *buf) {
	struct dlist_head *raw_mm;
	void *p;

	assert(mspace);
	assert(store_space);
	assert(buf);

	dlist_init(mspace);

	p = buf;
	raw_mm = store_space->next;

	/* can't use foreach, since it stores next pointer in accumulator */
	while (raw_mm != store_space) {
		struct mm_segment *buf_mm, *mm;

		buf_mm = p;

		mm = member_cast_out(raw_mm, struct mm_segment, link);
		memcpy(mm, buf_mm, buf_mm->size);

		p += buf_mm->size;
		raw_mm = raw_mm->next;
	}

	if (!dlist_empty(store_space)) {
		dlist_del(store_space);
		dlist_add_prev(mspace, store_space->next);
	}
}
