/**
 * @file
 *
 * @brief
 *
 * @date 10.4.2020
 * @author Archit Checker <archit.checker5@gmail.com>
 */

#include <embox/test.h>
#include <sys/mman.h>
#include <unistd.h>

EMBOX_TEST_SUITE("mprotect");

TEST_CASE("mprotect an address not multiple of page-size") {
	int result = mprotect((void *) 1, PAGE_SIZE(), PROT_READ);

	test_assert_equal(result, -1);
	test_assert_equal(errno, EINVAL);
}

TEST_CASE("set read protection") {
	int *ptr = mmap(NULL, PAGE_SIZE(), PROT_READ | PROT_WRITE, MAP_ANONYMOUS, -1, 0);
	int result = mprotect(ptr, PAGE_SIZE(), PROT_READ);

	munmap(ptr, PAGE_SIZE());

	test_assert_equal(result, 0);
}

TEST_CASE("set write protection and write to memory") {
	int *ptr = mmap(NULL, PAGE_SIZE(), PROT_READ, MAP_ANONYMOUS, -1, 0);
	int result = mprotect(ptr, PAGE_SIZE(), PROT_READ | PROT_WRITE);

	*ptr = 'x';

	test_assert_equal(result, 0);
	test_assert_equal(*ptr, 'x');

	munmap(ptr, PAGE_SIZE());
}
