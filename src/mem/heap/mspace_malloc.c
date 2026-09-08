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
#include <stddef.h>
#include <string.h>
#include <unistd.h>

#include <mem/heap_bm.h>
#include <mem/page.h>

#include <lib/libds/dlist.h>
#include <lib/libds/array.h>
#include <util/log.h>
#include <util/math.h>

#include <kernel/printk.h>
#include <kernel/panic.h>

#include <mem/heap/mspace_malloc.h>

#include <kernel/sched/sched_lock.h>

/* TODO make it per task field */
//static DLIST_DEFINE(task_mem_segments);

/* Segments visited, by the two walks that visit them.
 *
 * An mspace is a list of segments and both halves of it are linear searches:
 * an allocation tries each segment until one has room, and a free searches
 * each until one contains the pointer. On a heap that grows out of physical
 * memory -- and this one does, a megabyte at a time -- the list is as long as
 * the heap is large, and the cost of the whole workload stops being linear in
 * the number of objects.
 *
 * Counted rather than reasoned about, because the two walks are not equally
 * bad: new segments go on the FRONT, so an allocation usually finds room in
 * the first one it tries, while a free of a block allocated long ago walks
 * past every segment made since. */
unsigned long mspace_alloc_steps;
unsigned long mspace_free_steps;
#define mspace_seg_steps mspace_free_steps

/* What the two walks are given so they stop being quadratic in the number of
 * live objects.
 *
 * WHY. With task_is_greed off -- the default -- a segment is sized for the
 * request that could not be served, so a heap of N objects is a list of
 * nearly N segments, and every allocation and every free walks all of them.
 *
 * TWO OBSERVATIONS, one for each walk:
 *
 *   - if no segment could serve `size` at `boundary`, none can serve anything
 *     larger either -- until a free returns space or a segment is added; both
 *     clear the remembered failure.
 *
 *   - a program frees in bursts near where it allocated, so the segment that
 *     answered the last free is a good guess for the next one. A guess, and
 *     verified like any other: wrong, and the walk happens anyway.
 *
 * The cache is one entry and belongs to whichever mspace asked last -- these
 * are per-task heaps, and a task that is not running is not allocating.
 * Everything here runs under sched_lock(), the same lock the walks do. */
static struct dlist_head *cache_owner;
static struct mm_segment *cache_hint;
static size_t cache_fail_size;
static size_t cache_fail_boundary;
static int cache_fail_valid;

static void mspace_cache_select(struct dlist_head *mspace) {
	if (cache_owner != mspace) {
		cache_owner = mspace;
		cache_hint = NULL;
		cache_fail_valid = 0;
	}
}

/* Space appeared, so what did not fit before might now. */
static void mspace_cache_grew(void) {
	cache_fail_valid = 0;
}

/* The list itself is about to be taken apart or moved. A hint into a segment
 * that is being freed is worse than no hint: pointer_inside_segment() would
 * read mm->size out of released memory and could claim to own a pointer it
 * does not. Every path that frees segments wholesale or relinks the list ends
 * up here. */
static void mspace_cache_forget(void) {
	cache_owner = NULL;
	cache_hint = NULL;
	cache_fail_valid = 0;
}

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

	/* The segment the last free landed in. */
	if (cache_hint != NULL) {
		mspace_free_steps++;
		if (pointer_inside_segment(mm_to_segment(cache_hint), cache_hint->size,
		        ptr)) {
			return cache_hint;
		}
	}

	dlist_foreach_entry(mm, mspace, link) {
		mspace_seg_steps++;
		if (pointer_inside_segment(mm_to_segment(mm), mm->size, ptr)) {
			cache_hint = mm;
			return mm;
		}
	}

	return NULL;
}

static void *mspace_do_alloc(size_t boundary, size_t size, struct dlist_head *mspace) {
	struct mm_segment *mm;

	/* Nothing here could serve a request this large
	 * last time, and nothing has been freed or added since. */
	if (cache_fail_valid && (size >= cache_fail_size)
	    && (boundary >= cache_fail_boundary)) {
		return NULL;
	}

	dlist_foreach_entry(mm, mspace, link) {
		mspace_alloc_steps++;
		void *block = bm_memalign(mm_to_segment(mm), boundary, size);
		if (block != NULL) {
			return block;
		}
	}

	cache_fail_size = size;
	cache_fail_boundary = boundary;
	cache_fail_valid = 1;

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

	mspace_cache_select(mspace);

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

	/* A segment with room in it is exactly the change the cache above is
	 * waiting for. */
	mspace_cache_grew();

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
	return mspace_memalign(max(8, __alignof__(max_align_t)), size, mspace);
}

int mspace_free(void *ptr, struct dlist_head *mspace) {
	int res = 0;
	struct mm_segment *mm;

	assert(ptr);
	assert(mspace);

	sched_lock();

	mspace_cache_select(mspace);

	mm = pointer_to_mm(ptr, mspace);

	if (mm != NULL) {
		void *segment;

		segment = mm_to_segment(mm);
		bm_free(segment, ptr);
		mspace_cache_grew();

		if (bm_heap_is_empty(segment)) {
			/* Unlink, THEN free.
			 *
			 * `struct mm_segment' lives at the start of the segment itself,
			 * so mm->link is inside the pages being returned. Freeing first
			 * and unlinking after means dlist_del() reads and writes the
			 * neighbours through pointers that are already in the page
			 * allocator's hands -- and on four cores another core can have
			 * allocated and started writing those pages in between. What it
			 * leaves is an mspace list with a node in memory that belongs to
			 * somebody else.
			 *
			 * The page count is taken before the unlink for the same reason
			 * the unlink comes before the free: read what you need while it
			 * is still yours. */
			int page_cnt = mm->size / PAGE_SIZE();

			if (cache_hint == mm) {
				cache_hint = NULL;
			}
			dlist_del(&mm->link);
			mm_segment_free(mm, page_cnt);
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

	ret = mspace_memalign(max(8, __alignof__(max_align_t)), size, mspace);

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
	mspace_cache_forget();
	dlist_init(mspace);
	return 0;
}

int mspace_fini(struct dlist_head *mspace) {
	struct mm_segment *mm = NULL;

	mspace_cache_forget();

	dlist_foreach_entry(mm, mspace, link) {
		mm_segment_free(mm, mm->size / PAGE_SIZE());
	}

	/* And the head is left pointing at segments that have just been given
	 * away, which is not a state anything should rely on -- and something
	 * does. Ending this with dlist_init(mspace) is one line and it stops
	 * x86/user_apps booting: a task's resources are torn down in link order,
	 * the heap goes before task_phymem_deinit(), and that function free()s
	 * the phymem_link nodes it allocated from the heap now gone.
	 *
	 * So the emptying belongs with a teardown order that puts the heap last,
	 * which is a change to the task resource framework and not to this file.
	 * Written down rather than done quietly. */

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

	mspace_cache_forget();

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

	mspace_cache_forget();

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
