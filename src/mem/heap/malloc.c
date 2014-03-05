/**
 * @file
 * @brief Heap implementation based on boundary markers algorithm.
 * @details
 *    Segment structure:
 *    |struct mm_segment| *** space for bm ***|8 bytes of reserved space|
 *
 *    TODO:
 *    Should be improved by usage of page_alloc when size is divisible by PAGE_SIZE()
 *    Also SLAB allocator can be used when size is 16, 32, 64, 128...
 *
 * @date 04.03.2014
 * @author Alexander Kalmuk
 */

#include <errno.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#include <kernel/printk.h>
#include <mem/heap_bm.h>
#include <mem/page.h>
#include <util/dlist.h>

/* this space at the end of each segment is used by the underlying bm_init() */
#define RESERVED_SPACE_PER_SEGMENT 8

/* TODO make it per task field */
static DLIST_DEFINE(task_mem_segments);

struct mm_segment {
	struct dlist_head link;
	size_t size;
};

static inline bool pointer_inside_segment(void *segment, size_t size, void *pointer) {
	return (pointer > segment && pointer < (segment + size));
}

static inline void *mm_to_segment(struct mm_segment *mm) {
	return ((char *) mm + sizeof *mm);
}

static void *ptr_to_segment(void *ptr) {
	struct mm_segment *mm, *mm_next;
	void *segment;

	assert(ptr);

	dlist_foreach_entry(mm, mm_next, &task_mem_segments, link) {
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
	struct mm_segment *mm, *mm_next;
	size_t segment_pages_cnt;
	int iter;

	/* task_mem_segments = task_self()->mm->link */

	block = NULL;
	iter = 0;

	do {
		assert(iter++ < 2, "%s\n", "memory allocation cyclic");

		dlist_foreach_entry(mm, mm_next, &task_mem_segments, link) {
			block = bm_memalign(mm_to_segment(mm), boundary, size);
			if (block != NULL) {
				return block;
			}
		}

		/* No corresponding heap was found */
		segment_pages_cnt = (size + boundary + PAGE_SIZE()) / PAGE_SIZE();
		mm = (struct mm_segment *) page_alloc(__heap_pgallocator, segment_pages_cnt);
		if (mm == NULL)
			return NULL;

		mm->size = segment_pages_cnt * PAGE_SIZE() - sizeof *mm - RESERVED_SPACE_PER_SEGMENT;
		dlist_head_init(&mm->link);
		dlist_add_next(&mm->link, &task_mem_segments);

		bm_init(mm_to_segment(mm), mm->size);
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

	segment = ptr_to_segment(ptr);

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
