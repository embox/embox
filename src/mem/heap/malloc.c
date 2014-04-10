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

#include <errno.h>
#include <string.h>
#include <unistd.h>

#include <kernel/task/resource/task_heap.h>
#include <kernel/task.h>

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

static void *pointer_to_segment(void *ptr) {
	struct mm_segment *mm;
	void *segment;
	struct dlist_head *task_mem_segments;
	struct task *task = task_self();

	assert(ptr);

	task_mem_segments = &task_heap_get(task)->mm;

	dlist_foreach_entry(mm, task_mem_segments, link) {
		segment = mm_to_segment(mm);
		if (pointer_inside_segment(segment, mm->size, ptr)) {
			return segment;
		}
	}

	return NULL;
}

void *memalign(size_t boundary, size_t size) {
	extern struct page_allocator *__heap_pgallocator;
	void *block;
	struct mm_segment *mm;
	size_t segment_pages_cnt, segment_bytes_cnt;
	struct dlist_head *task_mem_segments;
	struct task *task = task_self();

	int iter;

	task_mem_segments = &task_heap_get(task)->mm;

	block = NULL;
	iter = 0;

	do {
		assert(iter++ < 2, "%s\n", "memory allocation cyclic");

		dlist_foreach_entry(mm, task_mem_segments, link) {
			block = bm_memalign(mm_to_segment(mm), boundary, size);
			if (block != NULL) {
				return block;
			}
		}

		/* No corresponding heap was found */
		/* XXX allocate more approproate count of pages without redundancy */
		segment_pages_cnt = (size + boundary + 2 * PAGE_SIZE()) / PAGE_SIZE();
		mm = (struct mm_segment *) page_alloc(__heap_pgallocator, segment_pages_cnt);
		if (mm == NULL)
			return NULL;

		mm->size = segment_pages_cnt * PAGE_SIZE();
		segment_bytes_cnt = mm->size - sizeof *mm;
		dlist_head_init(&mm->link);
		dlist_add_next(&mm->link, task_mem_segments);

		bm_init(mm_to_segment(mm), segment_bytes_cnt);
	} while(!block);

	return NULL;
}

void *malloc(size_t size) {
	void *ptr;

	if (size == 0) {
		return NULL;
	}

	ptr = memalign(0, size);
	if (ptr == NULL) {
		SET_ERRNO(ENOMEM);
		return NULL;
	}

	return ptr;
}

void free(void *ptr) {
	void *segment;

	if (ptr == NULL)
		return;

	segment = pointer_to_segment(ptr);

	if (segment != NULL) {
		bm_free(segment, ptr);
	} else {
		/* No segment containing pointer @c ptr was found. */
		printk("***** free(): incorrect address space\n");
	}
}

void *realloc(void *ptr, size_t size) {
	void *ret;

	if (size == 0 && ptr != NULL) {
		free(ptr);
		return NULL; /* ok */
	}

	ret = memalign(4, size);

	if (ret == NULL) {
		return NULL; /* error: errno set in malloc */
	}

	if (ptr == NULL) {
		return ret;
	}

	/* The content of new region will be unchanged in the range from the start of the region up to
	 * the minimum of the old and new sizes. So simply copy size bytes (may be with redundant bytes) */
	memcpy(ret, ptr, size);
	free(ptr);

	return ret;
}

void *calloc(size_t nmemb, size_t size) {
	void *ret;
	size_t total_size;

	total_size = nmemb * size;
	if (total_size == 0) {
		return NULL; /* ok */
	}

	ret = malloc(total_size);
	if (ret == NULL) {
		return NULL; /* error: errno set in malloc */
	}

	memset(ret, 0, total_size);
	return ret;
}

int heap_init(const struct task *task) {
	struct task_heap *task_heap;

	task_heap = task_heap_get(task);
	dlist_init(&task_heap->mm);

	return 0;
}

int heap_fini(const struct task *task) {
	extern struct page_allocator *__heap_pgallocator;
	struct task_heap *task_heap;
	struct mm_segment *mm;
	void *block;

	task_heap = task_heap_get(task);

	dlist_foreach_entry(mm, &task_heap->mm, link) {
		block = mm;
		page_free(__heap_pgallocator, block, mm->size / PAGE_SIZE());
	}


	return 0;
}
