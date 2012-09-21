/**
 * @file
 * @brief TODO documentation for softirq_test.c -- Eldar Abusalimov
 *
 * @date Aug 23, 2011
 * @author Eldar Abusalimov
 */

#include <embox/test.h>

#include <ctype.h>
#include <stddef.h>
#include <kernel/softirq.h>

EMBOX_TEST_SUITE("basic softirq tests");

static void test_softirq_handler(unsigned int softirq_nr, void *data) {
	int code = 'k' + softirq_nr - SOFTIRQ_NR_TEST; /* j/k/l */

	test_emit(code);
	test_emit((int) data);
	test_emit(toupper(code));
}

static void delegating_softirq_handler(unsigned int softirq_nr, void *data) {
	unsigned int target_softirq = (unsigned int) data;
	int code = 'k' + softirq_nr - SOFTIRQ_NR_TEST; /* j/k/l */

	test_emit(code);
	test_assert_zero(softirq_raise(target_softirq));
	test_emit(toupper(code));
}

TEST_CASE("softirq_raise called outside any hardware ISR should invoke "
		"the handler immediately") {
	test_emit('a');
	test_assert_zero(softirq_install(
			SOFTIRQ_NR_TEST, test_softirq_handler, (void *) 'x'));

	test_emit('b');
	test_assert_zero(softirq_raise(SOFTIRQ_NR_TEST));
	test_emit('c');

	test_assert_zero(softirq_install(SOFTIRQ_NR_TEST, NULL, NULL));

	test_assert_emitted("abkxKc");
}

TEST_CASE("softirq_raise called within softirq_lock/softirq_unlock "
		"must defer a call to the handler") {
	test_emit('a');
	test_assert_zero(softirq_install(
			SOFTIRQ_NR_TEST, test_softirq_handler, (void *) 'x'));

	softirq_lock();
	{
		test_emit('b');
		test_assert_zero(softirq_raise(SOFTIRQ_NR_TEST));
		test_emit('c');
	}
	softirq_unlock();

	test_emit('d');

	test_assert_zero(softirq_install(SOFTIRQ_NR_TEST, NULL, NULL));

	test_assert_emitted("abckxKd");
}

TEST_CASE("softirq with lower priority shouldn't preempt another with "
		"higher priority") {
	test_emit('a');
	test_assert_zero(softirq_install(SOFTIRQ_NR_TEST,
			delegating_softirq_handler, (void *) SOFTIRQ_NR_TEST_HI));
	test_assert_zero(softirq_install(SOFTIRQ_NR_TEST_HI,
			delegating_softirq_handler, (void *) SOFTIRQ_NR_TEST_LO));
	test_assert_zero(softirq_install(SOFTIRQ_NR_TEST_LO,
			test_softirq_handler, (void *) 'x'));

	test_emit('b');
	test_assert_zero(softirq_raise(SOFTIRQ_NR_TEST));
	test_emit('c');

	test_assert_zero(softirq_install(SOFTIRQ_NR_TEST, NULL, NULL));

	test_assert_emitted("abkjJKlxLc");
}
