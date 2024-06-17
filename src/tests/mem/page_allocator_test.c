/**
 * @file
 *
 * @brief
 *
 * @date 12.07.2018
 * @author Filipp Chubukov
 */

#include <embox/test.h>
#include <mem/page.h>

EMBOX_TEST_SUITE("page_allocator_init test");

TEST_CASE("Init page_allocator with one page") {
	struct page_allocator *allocator;
	char buff[0x10];

	allocator =  page_allocator_init(buff, 0x10, 0x10);
	test_assert_null(allocator);
}

TEST_CASE("Init page_allocator with len < page_size") {
	struct page_allocator *allocator;
	char buff[0x100];

	allocator =  page_allocator_init(buff, 0x10, 0x100);
	test_assert_null(allocator);
}
