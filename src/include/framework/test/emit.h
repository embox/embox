/**
 * @file
 * @brief TODO
 *
 * @date 19.04.11
 * @author Eldar Abusalimov
 */

#ifndef FRAMEWORK_TEST_EMIT_H_
#define FRAMEWORK_TEST_EMIT_H_

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>

#include <sys/cdefs.h>

#include <util/macro.h>

struct test_emit_buffer {
	char *ptr;
	char *buff;
	size_t buff_sz;
};

#define TEST_EMIT_BUFFER_DEF(buffer_nm, size) \
	__TEST_EMIT_BUFFER_DEF__(buffer_nm, size, \
			MACRO_GUARD(__test_emit_##buffer_nm))

#define __TEST_EMIT_BUFFER_DEF__(buffer_nm, size, storage_nm) \
	static char storage_nm[(size) + 1];          \
	static struct test_emit_buffer buffer_nm = { \
		.ptr = storage_nm,                       \
		.buff = storage_nm,                      \
		.buff_sz = (size),                       \
	}

__BEGIN_DECLS

extern void test_emit_into(struct test_emit_buffer *buffer, char ch);

static inline struct test_emit_buffer *test_emit_buffer_reset(
		struct test_emit_buffer *b) {
	assert(b);

	b->ptr = b->buff;
	*b->ptr = '\0';

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

static inline char *test_get_emitted_into(struct test_emit_buffer *b) {
	assert(b);
	return b->buff;
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

static inline void test_emit(char ch) {
	extern struct test_emit_buffer *__test_emit_buffer_current(void);
	test_emit_into(__test_emit_buffer_current(), ch);
}

static inline char *test_get_emitted(void) {
	extern struct test_emit_buffer *__test_emit_buffer_current(void);
	return test_get_emitted_into(__test_emit_buffer_current());
}

__END_DECLS

#ifdef __CDT_PARSER__

#undef TEST_EMIT_BUFFER_DEF
#define TEST_EMIT_BUFFER_DEF(buffer_nm, size) \
	static struct test_emit_buffer buffer_nm

#endif

#endif /* FRAMEWORK_TEST_EMIT_H_ */
