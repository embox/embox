/**
 * @file
 *
 * @brief
 *
 * @date 12.07.2018
 * @author Filipp Chubukov
 */

#include <embox/test.h>
#include <kernel/printk.h>
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

/* A request larger than the pool has to be refused.
 *
 * It was not: search_multi_page() kept the index in a size_t and compared it
 * against the `-1' that search_first_free() returned as an unsigned int. On a
 * 64-bit target 0x00000000ffffffff is not (size_t)-1, so the sentinel was
 * carried on as an index, and page_i2ptr() -- which takes an int -- turned it
 * into pages_start + (-1 * page_size): one page BELOW the pool. On a pool
 * large enough for the scan to go round again it does not come back at all.
 *
 * On a 32-bit target size_t is as wide as the sentinel and nothing was ever
 * wrong, which is why this went unseen.
 *
 * Note for a bisect: at a commit that has this case without the fix in
 * src/mem/pagealloc/bitmask.c it can hang rather than fail. */
TEST_CASE("a request larger than the pool is refused") {
	static char buff[0x2000];
	struct page_allocator *allocator;
	void *p;

	allocator = page_allocator_init(buff, sizeof(buff), 0x100);
	test_assert_not_null(allocator);

	/* One more page than exists, and then a size that is absurd rather than
	 * merely too large -- the second is what a byte count mistaken for a page
	 * count looks like. The pointer and the pool are printed because "not
	 * NULL" does not say whether the allocator handed out memory it does not
	 * have or simply lost track of the index. */
	p = page_alloc(allocator, (size_t)allocator->pages_n + 1);
	if (p != NULL) {
		printk("page_alloc: %u+1 pages of %u answered %p; pool is %u pages at "
		       "%p..%p\n",
		    allocator->pages_n, (unsigned)allocator->page_size, p,
		    allocator->pages_n, allocator->pages_start,
		    (char *)allocator->pages_start
		        + allocator->pages_n * allocator->page_size);
	}
	test_assert_null(p);

	p = page_alloc(allocator, (size_t)-1 / 0x100);
	if (p != NULL) {
		printk("page_alloc: an absurd request answered %p\n", p);
	}
	test_assert_null(p);

	/* Still usable afterwards: a refused request must not have marked
	 * anything busy on its way out. */
	p = page_alloc(allocator, 1);
	test_assert_not_null(p);
	page_free(allocator, p, 1);
}
