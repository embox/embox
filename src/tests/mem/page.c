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

	page = phymem_alloc(1);

	test_assert_not_null(page);

	phymem_free(page, 1);
}

TEST_CASE("multi page allocation") {
	void *page;

	page = phymem_alloc(10);

	test_assert_not_null(page);

	phymem_free(page, 10);
}

TEST_CASE("Mixed single and multi page allocation") {
	void *page;

	page = phymem_alloc(1);
	test_assert_not_null(page);
	phymem_free(page, 1);

	page = phymem_alloc(10);
	test_assert_not_null(page);
	phymem_free(page, 10);

	page = phymem_alloc(1);
	test_assert_not_null(page);
	phymem_free(page, 1);
}

TEST_CASE("Try allocate too lot of pages") {

	/* FIXME */
	test_assert_null(phymem_alloc(__phymem_allocator->pages_n + 1));
}
