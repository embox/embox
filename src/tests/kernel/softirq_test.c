/**
 * @file
 * @brief TODO documentation for softirq_test.c -- Eldar Abusalimov
 *
 * @date Aug 23, 2011
 * @author Eldar Abusalimov
 */

#include <embox/test.h>

#include <stdbool.h>
#include <string.h>
#include <errno.h>

#include <kernel/softirq.h>

EMBOX_TEST_SUITE("basic softirq tests");

TEST_EMIT_BUFFER_DEF(softirq_out, 'f' - 'a' + 1);

#define TEST_SOFTIRQ_NR 11

static void test_softirq_handler(softirq_nr_t softirq_nr, void *data) {
	test_emit(&softirq_out, 'c');
	test_emit(&softirq_out, (int) data);
	test_emit(&softirq_out, 'e');
}

TEST_CASE("softirq_raise called outside any hardware ISR should invoke "
		"the handler immediately") {
	test_emit_buffer_reset(&softirq_out);

	test_emit(&softirq_out, 'a');
	test_assert_zero(softirq_install(
					TEST_SOFTIRQ_NR, test_softirq_handler, (void *) 'd'));

	test_emit(&softirq_out, 'b');
	test_assert_zero(softirq_raise(TEST_SOFTIRQ_NR));

	test_emit(&softirq_out, 'f');
	// TODO detach softirq handler

	test_assert_str_equal(test_emit_buffer_str(&softirq_out), "abcdef");
}
