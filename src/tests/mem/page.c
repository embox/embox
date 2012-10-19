/**
 * @file
 *
 * @brief
 *
 * @date 25.11.2011
 * @author Anton Bondarev
 */
#include <embox/test.h>
#include <mem/page.h>
#include <mem/heap.h>
#include <mem/phymem.h>

EMBOX_TEST_SUITE("page allocation test");

TEST_CASE("single page allocation") {
	void *page;

	page = page_alloc(__phymem_allocator,1);

	test_assert_not_null(page);

	page_free(__phymem_allocator,page, 1);
}

TEST_CASE("multi page allocation") {
	void *page;

	page = page_alloc(__phymem_allocator,10);

	test_assert_not_null(page);

	page_free(__phymem_allocator,page, 10);
}

TEST_CASE("Mixed single and multi page allocation") {
	void *page;

	page = page_alloc(__phymem_allocator,1);
	test_assert_not_null(page);
	page_free(__phymem_allocator,page, 1);

	page = page_alloc(__phymem_allocator,2);
	test_assert_not_null(page);
	page_free(__phymem_allocator,page, 2);

	page = page_alloc(__phymem_allocator,1);
	test_assert_not_null(page);
	page_free(__phymem_allocator,page, 1);
}

TEST_CASE("Try allocate too lot of pages") {
	void *page;

	page = page_alloc(__phymem_allocator,HEAP_SIZE()/PAGE_SIZE() + 1);
	test_assert_null(page);
}
