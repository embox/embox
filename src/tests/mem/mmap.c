#include <embox/test.h>
#include <sys/mman.h>

EMBOX_TEST_SUITE("anonymous mmap");

TEST_CASE("allocate one page") {
	void *ptr = mmap(NULL, PAGE_SIZE(), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	test_assert_not_null(ptr);
	test_assert_zero(munmap(ptr, PAGE_SIZE()));
}

TEST_CASE("request a non-whole page") {
	int len = PAGE_SIZE() + 1;
	void *ptr = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	test_assert_not_null(ptr);
	test_assert_zero(munmap(ptr, len));
}

TEST_CASE("request zero-length") {
	void *ptr = mmap(NULL, 0, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	test_assert_null(ptr);
	test_assert_equal(errno, EINVAL);
}
