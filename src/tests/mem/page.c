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

EMBOX_TEST_SUITE("page allocation test");

TEST_CASE("single page allocation") {
	void *page;

	page = page_alloc(1);

	test_assert_not_null(page);

	page_free(page, 1);
}

TEST_CASE("multi page allocation") {
	void *page;

	page = page_alloc(10);

	test_assert_not_null(page);

	page_free(page, 10);
}

TEST_CASE("Mixed single and multi page allocation") {
	void *page;

	page = page_alloc(1);
	test_assert_not_null(page);
	page_free(page, 1);

	page = page_alloc(10);
	test_assert_not_null(page);
	page_free(page, 10);

	page = page_alloc(1);
	test_assert_not_null(page);
	page_free(page, 1);
}


