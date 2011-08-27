/**
 * @file
 * @brief TODO
 *
 * @date 19.04.11
 * @author Eldar Abusalimov
 */

#ifndef FRAMEWORK_TEST_EMIT_H_
#define FRAMEWORK_TEST_EMIT_H_

#include __impl_x(framework/test/emit_impl.h)

extern void test_emit(char ch);
extern char *test_get_emitted(void);

struct test_emit_buffer;

#define TEST_EMIT_BUFFER_DEF(buffer_nm, size) \
	  __TEST_EMIT_BUFFER_DEF(buffer_nm, size)

extern void test_emit_into(struct test_emit_buffer *buffer, char ch);
extern char *test_get_emitted_into(struct test_emit_buffer *buffer);

extern struct test_emit_buffer *test_emit_buffer_init(
		struct test_emit_buffer *buffer, char *buff, size_t buff_sz);

extern struct test_emit_buffer *test_emit_buffer_reset(
		struct test_emit_buffer *buffer);

extern bool test_emit_buffer_overflown(struct test_emit_buffer *buffer);
extern bool test_emit_buffer_full(struct test_emit_buffer *buffer);

#endif /* FRAMEWORK_TEST_EMIT_H_ */
