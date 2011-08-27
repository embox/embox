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

#define TEST_SOFTIRQ_NR 11

static void test_softirq_handler(softirq_nr_t softirq_nr, void *data) {
	test_emit('c');
	test_emit((int) data);
	test_emit('e');
}

TEST_CASE("softirq_raise called outside any hardware ISR should invoke "
		"the handler immediately") {
	test_emit('a');
	test_assert_zero(softirq_install(
					TEST_SOFTIRQ_NR, test_softirq_handler, (void *) 'd'));

	test_emit('b');
	test_assert_zero(softirq_raise(TEST_SOFTIRQ_NR));

	test_emit('f');

	test_assert_zero(softirq_install(TEST_SOFTIRQ_NR, NULL, NULL));

	test_assert_emitted("abcdef");
}
