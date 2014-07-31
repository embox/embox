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

#include <err.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include <mem/heap_bm.h>
#include <mem/page.h>

#include <util/dlist.h>

#include <kernel/printk.h>

/* TODO make it per task field */
//static DLIST_DEFINE(task_mem_segments);


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

static void *pointer_to_segment(void *ptr, struct dlist_head *mspace) {
	struct mm_segment *mm;
	void *segment;

	assert(ptr);
	assert(mspace);

	dlist_foreach_entry(mm, mspace, link) {
		segment = mm_to_segment(mm);
		if (pointer_inside_segment(segment, mm->size, ptr)) {
			return segment;
		}
	}

	return NULL;
}

void *mspace_memalign(size_t boundary, size_t size, struct dlist_head *mspace) {
	extern struct page_allocator *__heap_pgallocator;
	void *block;
	struct mm_segment *mm;
	size_t segment_pages_cnt, segment_bytes_cnt;
	int iter;

	assert(mspace);
	assert(__heap_pgallocator);

	block = NULL;
	iter = 0;

	do {
		assert(iter++ < 2, "%s\n", "memory allocation cyclic");

		dlist_foreach_entry(mm, mspace, link) {
			block = bm_memalign(mm_to_segment(mm), boundary, size);
			if (block != NULL) {
				return block;
			}
		}

		/* No corresponding heap was found */
		/* XXX allocate more approproate count of pages without redundancy */
		/* Note, overflow may occur */
		segment_pages_cnt = size / PAGE_SIZE() + boundary / PAGE_SIZE();
		segment_pages_cnt += (size % PAGE_SIZE() + boundary % PAGE_SIZE()
				+ 2 * PAGE_SIZE()) / PAGE_SIZE();

		mm = (struct mm_segment *) page_alloc(__heap_pgallocator, segment_pages_cnt);
		if (mm == NULL)
			return NULL;

		mm->size = segment_pages_cnt * PAGE_SIZE();
		segment_bytes_cnt = mm->size - sizeof *mm;
		dlist_head_init(&mm->link);
		dlist_add_next(&mm->link, mspace);

		bm_init(mm_to_segment(mm), segment_bytes_cnt);
	} while(!block);

	return NULL;
}

void *mspace_malloc(size_t size, struct dlist_head *mspace) {
	assert(mspace);
	return mspace_memalign(8, size, mspace);
}

int mspace_free(void *ptr, struct dlist_head *mspace) {
	void *segment;

	assert(ptr);
	assert(mspace);

	segment = pointer_to_segment(ptr, mspace);

	if (segment != NULL) {
		bm_free(segment, ptr);
	} else {
		/* No segment containing pointer @c ptr was found. */
		printk("***** free(): incorrect address space\n");
		return -1;
	}

	return 0;
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
		return err_ptr(EINVAL);
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
	extern struct page_allocator *__heap_pgallocator;
	struct mm_segment *mm;
	void *block;

	//task_heap = task_heap_get(mspace);

	dlist_foreach_entry(mm, mspace, link) {
		block = mm;
		page_free(__heap_pgallocator, block, mm->size / PAGE_SIZE());
	}

	return 0;
}

size_t mspace_deep_copy_size(struct dlist_head *mspace) {
	struct mm_segment *mm;
	size_t ret;

	ret = 0;
	dlist_foreach_entry(mm, mspace, link) {
		ret += mm->size;
	}
	return ret;
}


void mspace_deep_store(struct dlist_head *mspace, struct dlist_head *store_space, void *buf) {
	struct mm_segment *mm;
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
