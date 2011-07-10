/**
 * @file
 * @brief test page allocator
 * @details Some random tests for page allocator
 *
 * @date 17.04.10
 * @author Fedor Burdun
 */

#include <embox/test.h>
#include <stdio.h>
#include <mem/pagealloc/opallocator.h>

#define VST

#define TEST_COUNT 4
#define TEST_STACK_SIZE	(20*TEST_COUNT+2)

#if 0
//#define DONOTUSE_PAGE_ALLOC       /* for testing test of page alloc =) */
//#define INTERACTIVE_TEST          /* */
//#define VERBOSE_DEBUG_OUT         /* Some advanced output (list of page marks) */
//#define VERBOSE_DEBUG_OUT_MALLOC
//#define STACK_POP_FROM_HEAD       /* or tail */
#define TEST_COUNT 10
#define MAX_PAGE_FOR_ALLOC 0x10

/**
 * count the number of free pages
 */
static size_t free_page_count(void) {
#ifdef DONOTUSE_PAGE_ALLOC
	return 0;
#endif
	/* go from prev to next */
	if (get_cmark_p()==NULL) {
		return 0;
	}
	size_t fr = get_cmark_p()->psize;
	pmark_t* cur = get_cmark_p()->pnext;
	while (cur != get_cmark_p()) {
		fr = fr + cur->psize;
		cur = cur->pnext;
	}
	return fr;
}

/**
 * count the number of allowed pages
 */
static size_t allow_page_count() {
#ifdef DONOTUSE_PAGE_ALLOC
	return 0;
#endif
	/* go from next to prev */
	if (get_cmark_p()==NULL) {
		return PAGE_QUANTITY;
	}
	size_t fr = get_cmark_p()->psize;
	pmark_t* cur = get_cmark_p()->pprev;
	while (cur != get_cmark_p()) {
		fr = fr + cur->psize;
		cur = cur->pprev;
	}
	return PAGE_QUANTITY-fr;
}

/**
 * Debug output memory (print list of markers)
 */
static void do_allpage() {
	pmark_t* pcur = get_cmark_p();
	TRACE("Print map of memory: \n");
	if (pcur == NULL) { /* don't exist list of empty page */
		TRACE("\tList of free page is empty\n");
		return;
	}

	do {
		TRACE("\tStruct: %08x\n\t\tsize: %d\n\t\tnext: %08x\n\t\tprev: %08x\n\n",
			pcur, pcur->psize, pcur->pnext, pcur->pprev);
		pcur = pcur->pnext;
	} while (pcur != get_cmark_p());
}

/**
 * memory error counter
 */
static int count_of_error = 0;

/**
 * simply memory checker (sum free and alocated memory must be equal size of pool)
 */
static void memory_check() {
	size_t allocp = allow_page_count();
	size_t freep  = free_page_count();
	TRACE("Allocated: %d ; Free %d \n", allocp, freep);
	if (allocp+freep != PAGE_QUANTITY) {
		TRACE("WARNING: Sum of allocated and free page don't equal page quality!!!\n");
		++count_of_error;
	}
}

#endif

EMBOX_TEST(run);

static int run(void) {
#ifdef VST

#ifndef EXTENDED_TEST
	extern char _heap_start;
	extern char _heap_end;
#endif

#define PAGE_QUANTITY (((size_t) (&_heap_end - &_heap_start)) / CONFIG_PAGE_SIZE)

	void * pointers[TEST_STACK_SIZE];
	void **first,**last;
	int test_id=0;
	size_t i;
	int callowed=0,cfree=0;
	first = pointers; last = pointers;

	TRACE("\nTest page_alloc. Size of pool: %d (pages).\n",PAGE_QUANTITY);

	/* push 2 pop 1 - while it is best idea */

	do {
		if (++test_id<TEST_COUNT) {

			for (i=0;i<8;++i) {

				if (NULL == (*last = opalloc())) {
					TRACE("Alloc page: %p\n", (void*) NULL);
				} else {
					TRACE("Alloc page: %p\n", (void*) last);
					last = ++last < pointers+TEST_STACK_SIZE ? last : pointers;
					++callowed;
				}
			}

		}
		TRACE("Free page: %p\n", (void*) first);
		opfree(*first);
		first = ++first < pointers + TEST_STACK_SIZE ? first : pointers;
		cfree += 1;

	} while (first!=last); /* queue is not empty */

	TRACE("was allowed %d was free %d: ",callowed,cfree);

	return callowed == cfree ? 0 : 1;
#else
	void * pointers[TEST_STACK_SIZE];
	void **first, **last;
	int test_id=0;
	int callowed=0,cfree=0;
	first = pointers;
	last = pointers;

	/* push 2 pop 1 - while it is best idea */

	do {
		if (++test_id<TEST_COUNT) {
			if (NULL == (*last = page_alloc())) {
			} else {
				last = ++last < pointers+TEST_STACK_SIZE ? last : pointers;
				++callowed;
			}

			if (NULL == (*last = page_alloc())) {
			} else {
				last = ++last < pointers+TEST_STACK_SIZE ? last : pointers;
				++callowed;
			}
		}
		page_free(*first);
		first = ++first < pointers+TEST_STACK_SIZE ? first : pointers;
		cfree += 1;

	} while (first!=last); /* queue is not empty */

	TRACE("was allowed %d was free %d: ",callowed,cfree);

	return callowed == cfree ? 0 : 1;
#endif
}
