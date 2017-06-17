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

static void run_wait_task(void *(*fn)(void *), void *arg) {
	pid_t p;

	p = new_task("", fn, NULL);

	while (-EINTR == task_waitpid(p));
}

static void *mmap_test_child(void *arg) {
	int allocated = 0;
	while (1) {
		void *ptr = mmap(NULL, PAGE_SIZE(), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
		if (!ptr)
			break;
		if (!allocated && ptr) {
			allocated = 1;
			test_emit('a');
		}
	}
	test_emit('b');
	_exit(0);
}

TEST_CASE("unmap after exit") {
	run_wait_task(mmap_test_child, NULL);
	run_wait_task(mmap_test_child, NULL);
	test_assert_emitted("abab");
}
