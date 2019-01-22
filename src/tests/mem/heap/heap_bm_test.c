/**
 * @file
 *
 * @brief
 *
 * @date 26.12.2018
 * @author Alexander Kalmuk
 */
#include <embox/test.h>
#include <mem/heap_bm.h>
#include <mem/page.h>

EMBOX_TEST_SUITE("heap_bm test");

#define HEAP_BM_INFO_MAX_SIZE 64
#define SIZE_128_B            128

extern struct page_allocator *__heap_pgallocator;

TEST_CASE("Check if heap_bm concatenates free blocks") {
	void *heap_start_ptr;
	void *ptr, *ptr1;
	int size;

	heap_start_ptr = page_alloc(__heap_pgallocator, 1);
	test_assert_not_null(heap_start_ptr);

	bm_init(heap_start_ptr, PAGE_SIZE());

	/* Dummy search for the possible largest size to allocate */
	for (size = PAGE_SIZE(); size > 0; size--) {
		ptr = bm_memalign(heap_start_ptr, 8, size);
		if (ptr) {
			break;
		}
	}
	test_assert((size > PAGE_SIZE() - HEAP_BM_INFO_MAX_SIZE)
		&& (size < PAGE_SIZE()));
	bm_free(heap_start_ptr, ptr);

	ptr = bm_memalign(heap_start_ptr, 8, SIZE_128_B);
	test_assert_not_null(ptr);
	ptr1 = bm_memalign(heap_start_ptr, 8, SIZE_128_B);
	test_assert_not_null(ptr1);

	bm_free(heap_start_ptr, ptr);
	bm_free(heap_start_ptr, ptr1);

	ptr = bm_memalign(heap_start_ptr, 8, size);
	test_assert_not_null(ptr);
	bm_free(heap_start_ptr, ptr);

	page_free(__heap_pgallocator, heap_start_ptr, 1);
}
