/**
 * @file
 * @brief TODO
 *
 * @date Apr 19, 2011
 * @author Eldar Abusalimov
 */

#ifndef FRAMEWORK_TEST_EMIT_H_
#define FRAMEWORK_TEST_EMIT_H_

#include __impl_x(framework/test/emit_impl.h)

struct test_emit_buffer;

#define TEST_EMIT_BUFFER_DEF(buffer_nm, size) \
	  __TEST_EMIT_BUFFER_DEF(buffer_nm, size)

extern void test_emit(struct test_emit_buffer *buffer, char ch);
extern void test_emit_unique(struct test_emit_buffer *b, char ch);

extern struct test_emit_buffer *test_emit_buffer_init(
		struct test_emit_buffer *buffer, char *buff, size_t buff_sz);

extern struct test_emit_buffer *test_emit_buffer_reset(
		struct test_emit_buffer *buffer);

extern bool test_emit_buffer_overflown(struct test_emit_buffer *buffer);
extern bool test_emit_buffer_full(struct test_emit_buffer *buffer);
extern char *test_emit_buffer_str(struct test_emit_buffer *buffer);

#endif /* FRAMEWORK_TEST_EMIT_H_ */
