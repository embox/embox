#include <embox/test.h>
#include <kernel/task.h>
#include <sys/mman.h>
#include <unistd.h>

EMBOX_TEST_SUITE("anonymous mmap");

TEST_CASE("allocate one page") {
	void *ptr = mmap(NULL, PAGE_SIZE(), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	test_assert_not_equal(ptr, MAP_FAILED);
	test_assert_zero(munmap(ptr, PAGE_SIZE()));
}

TEST_CASE("request a non-whole page") {
	int len = PAGE_SIZE() + 1;
	void *ptr = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	test_assert_not_equal(ptr, MAP_FAILED);
	test_assert_zero(munmap(ptr, len));
}

TEST_CASE("request zero-length") {
	void *ptr = mmap(NULL, 0, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	test_assert_equal(ptr, MAP_FAILED);
	test_assert_equal(errno, EINVAL);
}

static void run_wait_task(void *(*fn)(void *), void *arg) {
	pid_t p;

	p = new_task("", fn, arg);

	while (-EINTR == task_waitpid(p));
}

static void *mmap_test_child(void *arg) {
	int allocated = 0;
	while (1) {
		void *ptr = mmap(NULL, PAGE_SIZE(), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
		if (ptr == MAP_FAILED)
			break;
		allocated++;
	}
	*(int *) arg = allocated;
	_exit(0);
}

TEST_CASE("unmap after exit") {
	int alloc_count_a, alloc_count_b;
	run_wait_task(mmap_test_child, &alloc_count_a);
	run_wait_task(mmap_test_child, &alloc_count_b);
	test_assert_equal(alloc_count_a, alloc_count_b);
}
