/**
 * @file
 * @brief Tests thread suspend/resume methods.
 *
 * @date Apr 17, 2011
 * @author Eldar Abusalimov
 */

#include <embox/test.h>

#include <stdbool.h>
#include <string.h>

#include <kernel/thread/api.h>

#define TEST_EMIT_BUFFER_DEF(buffer_nm, size) \
	TEST_EMIT_BUFFER_DEF__(buffer_nm, size, MACRO_GUARD(__test_emit_##buffer_nm))

#define TEST_EMIT_BUFFER_DEF__(buffer_nm, size, storage_nm) \
	static char storage_nm[(size) + 1]; \
	static struct test_emit_buffer buffer_nm = { \
		.ptr = storage_nm, \
		.buff = storage_nm, \
		.buff_sz = (size), \
	}

struct test_emit_buffer {
	char *ptr;
	char *buff;
	size_t buff_sz;
};

static inline struct test_emit_buffer *test_emit_buffer_reset(
		struct test_emit_buffer *b) {
	assert(b);

	b->buff[b->buff_sz] = '\0';
	b->ptr = b->buff;

	return b;
}

static inline struct test_emit_buffer *test_emit_buffer_init(
		struct test_emit_buffer *b, char *buff, size_t buff_sz) {
	assert(b);
	assert(buff);

	b->buff = buff;
	b->buff_sz = buff_sz;

	return test_emit_buffer_reset(b);
}

static inline bool test_emit_buffer_overflown(struct test_emit_buffer *b) {
	assert(b);
	assert(b->ptr);
	return b->ptr > b->buff + b->buff_sz;
}

static inline bool test_emit_buffer_full(struct test_emit_buffer *b) {
	assert(b);
	assert(b->ptr);
	return b->ptr == b->buff + b->buff_sz;
}

static inline void test_emit(struct test_emit_buffer *b, char ch) {
	if (test_emit_buffer_overflown(b)) {
		return;
	}

	if (!test_emit_buffer_full(b)) {
		*b->ptr++ = ch;
	} else {
		/* do overflow */
		b->ptr++;
	}
}

static inline void test_emit_unique(struct test_emit_buffer *b, char ch) {
	assert(b);
	assert(b->ptr);

	if (b->ptr == b->buff || *(b->ptr - 1) != ch) {
		test_emit(b, ch);
	}
}

// TODO define them in API. -- Eldar
#define THREAD_PRIORITY_LOW  191
#define THREAD_PRIORITY_HIGH 63

EMBOX_TEST_SUITE("Thread suspend/resume");

TEST_EMIT_BUFFER_DEF(buff1, 'f' - 'a' + 1);

static void *low_run(void *);
static void *high_run(void *);

TEST_CASE() {
	struct thread *low = NULL, *high = NULL;
	void *ret;

	test_emit_buffer_reset(&buff1);

	test_assert_zero(
			thread_create(&high, THREAD_FLAG_SUSPENDED, high_run, NULL));
	test_assert_not_null(high);
	test_assert_zero(thread_set_priority(high, THREAD_PRIORITY_HIGH));

	test_assert_zero(thread_create(&low, THREAD_FLAG_SUSPENDED, low_run, high));
	test_assert_not_null(low);
	test_assert_zero(thread_set_priority(low, THREAD_PRIORITY_LOW));

	test_emit(&buff1, 'a');

	test_assert_zero(thread_resume(low));
	test_assert_zero(thread_join(low, &ret));
	test_assert_null(ret);

	test_emit(&buff1, 'e');

	test_assert_zero(thread_resume(high));
	test_assert_zero(thread_join(high, &ret));
	test_assert_null(ret);

	test_assert_zero(strcmp(buff1.buff, "abcdef"));
}

static void *low_run(void *arg) {
	struct thread *high = (struct thread *) arg;

	test_emit(&buff1, 'b');
	thread_resume(high);
	test_emit(&buff1, 'd');

	return NULL;
}

static void *high_run(void *arg) {
	test_emit(&buff1, 'c');
	thread_suspend(thread_self());
	test_emit(&buff1, 'f');

	return NULL;
}

