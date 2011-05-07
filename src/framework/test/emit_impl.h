/**
 * @file
 * @brief TODO
 *
 * @date 19.04.11
 * @author Eldar Abusalimov
 */

#ifndef FRAMEWORK_TEST_EMIT_IMPL_H_
#define FRAMEWORK_TEST_EMIT_IMPL_H_

#include <assert.h>
#include <stdbool.h>

#include <util/macro.h>

struct test_emit_buffer {
	char *ptr;
	char *buff;
	size_t buff_sz;
};

#define __TEST_EMIT_BUFFER_DEF(buffer_nm, size) \
	__TEST_EMIT_BUFFER_DEF__(buffer_nm, size, \
			MACRO_GUARD(__test_emit_##buffer_nm))

#define __TEST_EMIT_BUFFER_DEF__(buffer_nm, size, storage_nm) \
	static char storage_nm[(size) + 1];          \
	static struct test_emit_buffer buffer_nm = { \
		.ptr = storage_nm,                       \
		.buff = storage_nm,                      \
		.buff_sz = (size),                       \
	}

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

static inline char *test_emit_buffer_str(struct test_emit_buffer *b) {
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

static inline void test_emit(struct test_emit_buffer *b, char ch) {
	if (test_emit_buffer_overflown(b)) {
		return;
	}

	if (!test_emit_buffer_full(b)) {
		*b->ptr++ = ch;
		*b->ptr = '\0';
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

#ifdef __CDT_PARSER__

#undef __TEST_EMIT_BUFFER_DEF
#define __TEST_EMIT_BUFFER_DEF(buffer_nm, size) \
	static struct test_emit_buffer buffer_nm

#endif

#endif /* FRAMEWORK_TEST_EMIT_IMPL_H_ */
